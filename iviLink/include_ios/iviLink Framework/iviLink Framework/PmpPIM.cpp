/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 22.03.2013
 * Communicates with PIM on the other side (processing of profile creation
 * and death notifications/requests)
 */

#include <cassert>

#include "PmpPIM.hpp"
#include "BaseUid.hpp"
#include "Database.hpp"
#include "ServicesInfoProvider.hpp"
#include "PmpCommunicator.hpp"
#include "CServiceManager.hpp"
#include "CAuthenticationProfileImpl.hpp"
#include "Serialize.hpp"

using namespace iviLink::PMP;
using namespace iviLink::Profile;
using namespace iviLink;

Logger PmpPIM::mLogger = Logger::getInstance("iviLink::PMP::iOS::PmpPIM");

PmpPIM::PmpPIM()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

PmpPIM::~PmpPIM()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

PmpPIM * PmpPIM::getInstance()
{
    static PmpPIM * instance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^
            {
                instance = new PmpPIM;
            });
    return instance;
}

void PmpPIM::processFrame(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    assert(frame->client == PMP_CLIENT_PI_SRV);
    switch (frame->reqType)
    {
    case PMP_REQ_PIM_CREATE_PROFILE:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_PIM_CREATE_PROFILE");
        onCreateProfileRequest(frame);
        break;
    case PMP_REQ_PIM_PROFILE_DIED:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_PIM_PROFILE_DIED");
        onProfileDiedRequest(frame);
        break;
    default:
        LOG4CPLUS_FATAL(mLogger, "unknown request: " + convertIntegerToString(frame->reqType));
        assert(false);
        break;
    }

}

void PmpPIM::onCreateProfileRequest(iviLink::PMP::PMPFrame *frame)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    std::string profileUid = bufferToString(frame->data, true);
    UInt32 offset = stringInBufSize(profileUid);
    LOG4CPLUS_INFO(mLogger, "uid: " + profileUid);
    std::string profileImplUid = bufferToString(frame->data + offset, true);
    offset += stringInBufSize(profileImplUid);
    LOG4CPLUS_INFO(mLogger, "piuid: " + profileImplUid);
    std::stringstream ss;
    ss << profileImplUid;
    UInt32 piuidInt;
    ss >> piuidInt;
    LOG4CPLUS_INFO(mLogger, "piuid ff: " + convertIntegerToString(piuidInt));
    iviLink::fastForwardUidTo(piuidInt);
    std::string service = bufferToString(frame->data + offset, true);
    LOG4CPLUS_INFO(mLogger, "sid: " + service);
    App::Service::CServiceManager::getInstance()->incomingProfileRequest(service, profileUid,
            profileImplUid);
}

void PmpPIM::onProfileDiedRequest(PMPFrame* frame)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    std::string profileImplUid = bufferToString(frame->data, true);
    UInt32 offset = stringInBufSize(profileImplUid);
    LOG4CPLUS_INFO(mLogger, "piuid: " + profileImplUid);
    std::string service = bufferToString(frame->data + offset, true);
    LOG4CPLUS_INFO(mLogger, "sid: " + service);
    App::Service::CServiceManager::getInstance()->incomingProfileDiedRequest(service,
            profileImplUid);
}

ProfileHolder * PmpPIM::createProfile(Profile::IProfileCallbackProxy * proxy,
        std::string const& service, std::string const& profile, std::string const& piuid)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    UInt32 version = Database::getInstance()->getVersionForProfileInService(profile, service);
    BaseProfileApi * creation = NULL;
    if (profile == Database::getInstance()->getAuthenticationProfileUid().value())
    {
        creation = new CAuthenticationProfileImpl(proxy);
    }
    else
    {
        ServicesInfoProvider provider;
        creation = provider.createProfileImpl(BaseUid(profile), version, proxy);
    }
    if (creation == NULL)
    {
        LOG4CPLUS_WARN(mLogger,
                "could not create profile: " + profile + " for service: " + service);
        return NULL;
    }
    return new ProfileHolder(Profile::IUid(piuid), Service::Uid(service), creation);
}

void PmpPIM::sendCreateProfileRequest(iviLink::Profile::Uid const& profileUID,
        iviLink::Profile::IUid const& piuid, iviLink::Service::Uid const& sid)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger,
            "request to create: " + profileUID.value() + " with uid: " + piuid.value()
                    + " for service: " + sid.value());
    UInt32 size = sizeof(PMPFrame);
    size += stringInBufSize(profileUID.value());
    size += stringInBufSize(piuid.value());
    size += stringInBufSize(sid.value());

    PMPFrame * request = reinterpret_cast<PMPFrame*>(new UInt8[size]);
    request->client = PMP_CLIENT_PI_SRV;
    request->reqType = PMP_REQ_PIM_CREATE_PROFILE;
    request->size = size;
    UInt32 offset = 0;
    stringToBuffer(profileUID.value(), request->data + offset, true);
    offset += stringInBufSize(profileUID.value());
    stringToBuffer(piuid.value(), request->data + offset, true);
    offset += stringInBufSize(piuid.value());
    stringToBuffer(sid.value(), request->data + offset, true);
    PmpCommunicator::getInstance()->sendFrame(request);
}

void PmpPIM::sendProfileDiedRequest(iviLink::Profile::IUid const& piuid,
        iviLink::Service::Uid const& sid)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    UInt32 size = sizeof(PMPFrame);
    size += stringInBufSize(piuid.value());
    size += stringInBufSize(sid.value());
    PMPFrame * request = reinterpret_cast<PMPFrame*>(new UInt8[size]);
    request->client = PMP_CLIENT_PI_SRV;
    request->reqType = PMP_REQ_PIM_PROFILE_DIED;
    request->size = size;
    stringToBuffer(piuid.value(), request->data, true);
    UInt32 offset = stringInBufSize(piuid.value());
    stringToBuffer(sid.value(), request->data + offset, true);
    PmpCommunicator::getInstance()->sendFrame(request);
}
