/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 22.03.2013
 * Communicates with PMP Core on the other side, supervises locking
 * and unlocking profiles.
 */

#include <cassert>
#include <set>

#include "PmpCore.hpp"
#include "PmpCommunicator.hpp"
#include "Serialize.hpp"
#include "ByteOrder.hpp"
#include "Database.hpp"
#include "SystemControllerStub.hpp"

namespace iviLink
{
namespace PMP
{
Logger PmpCore::mLogger = Logger::getInstance("iviLink::PMP::iOS::PmpCore");

PmpCore::PmpCore()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

PmpCore::~PmpCore()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

PmpCore * PmpCore::getInstance()
{
    static PmpCore * instance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^
            {
                instance = new PmpCore;
            });
    return instance;
}

void PmpCore::waitForAllUnlocked()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mAllUnlockSema.wait();
}

void PmpCore::waitForAuthUnlocked()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mAuthWaitSema.wait();
}

void PmpCore::unlockAuthenticationProfile()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Database * database = Database::getInstance();
    database->enableAuthenticationProfileLocally();
    UInt32 size = sizeof(PMPFrame);
    size += stringInBufSize(database->getAuthenticationProfileUid().value());
    PMPFrame * request = reinterpret_cast<PMPFrame*>(new UInt8[size]);
    request->size = size;
    request->client = PMP_CLIENT_CORE;
    request->reqType = PMP_REQ_CORE_UNLOCK;
    stringToBuffer(database->getAuthenticationProfileUid().value(), request->data, true);
    PmpCommunicator::getInstance()->sendFrame(request);
}

void PmpCore::unlockAll()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Database::getInstance()->unlockAll();
    UInt32 size = sizeof(PMPFrame);
    PMPFrame * toSend = reinterpret_cast<PMPFrame*>(new UInt8[size]);
    toSend->size = size;
    toSend->reqType = PMP_REQ_CORE_UNLOCK_ALL;
    toSend->client = PMP_CLIENT_CORE;
    PmpCommunicator::getInstance()->sendFrame(toSend);
}

void PmpCore::onUnlockProfile(iviLink::PMP::PMPFrame *frame)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Profile::Uid profile = bufferToString(frame->data, true);
    if (profile.value() == Database::getInstance()->getAuthenticationProfileUid().value())
    {
        Database::getInstance()->enableAuthenticationProfileRemote();
        mAuthWaitSema.signal();
    }
    else
    {
        assert(false); // not supported yet
    }
}

void PmpCore::onGetAvailableComplementsRequest()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    int size = sizeof(PMPFrame) + 4;
    tProfilesAndVersionsList profileImpls = Database::getInstance()->getAvailableProfiles();
    tProfilesAndVersionsList::const_iterator iter = profileImpls.begin();
    int count = 0;
    for (; iter != profileImpls.end(); iter++)
    {
        size += stringInBufSize(iter->first) + sizeof(UInt32);
        count++;
    }
    PMPFrame * complements = reinterpret_cast<PMPFrame*>(new UInt8[size]);
    complements->size = size;
    complements->reqType = PMP_REQ_CORE_GET_COMPLEMENTS_RESPONSE;
    complements->client = PMP_CLIENT_CORE;
    count = ByteOrder::hton32(count);
    memcpy(complements->data, &count, 4);
    int offset = 4;
    tProfilesAndVersionsList::const_iterator setIter = profileImpls.begin();
    for (; setIter != profileImpls.end(); setIter++)
    {
        stringToBuffer(setIter->first, complements->data + offset, true);
        offset += stringInBufSize(setIter->first);
        UInt32 version = ByteOrder::hton32(setIter->second);
        memcpy(complements->data + offset, &version, sizeof(version));
        offset += sizeof(version);
    }
    PmpCommunicator::getInstance()->sendFrame(complements);
}

void PmpCore::processFrame(PMPFrame* frame)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    assert(frame->client == PMP_CLIENT_CORE);
    switch (frame->reqType)
    {
    case PMP_REQ_CORE_GET_COMPLEMENTS_REQUEST:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_CORE_GET_COMPLEMENTS_REQUEST");
        onGetAvailableComplementsRequest();
        break;
    case PMP_REQ_CORE_GET_COMPLEMENTS_RESPONSE:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_CORE_GET_COMPLEMENTS_RESPONSE");
        onGetAvailableComplementsResponse(frame);
        break;
    case PMP_REQ_CORE_LOCK:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_CORE_LOCK");
        break;
    case PMP_REQ_CORE_UNLOCK:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_CORE_UNLOCK");
        onUnlockProfile(frame);
        break;
    case PMP_REQ_CORE_LOCK_ALL:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_CORE_LOCK_ALL");
        Database::getInstance()->lockAllRemote();
        break;
    case PMP_REQ_CORE_UNLOCK_ALL:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_CORE_UNLOCK_ALL");
        Database::getInstance()->unlockAllRemote();
        mAllUnlockSema.signal();
        break;
    case PMP_REQ_CORE_DIS_BY_CLIENT:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_CORE_DIS_BY_CLIENT");
    case PMP_REQ_CORE_ENABLE_BY_CLNT:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_CORE_ENABLE_BY_CLNT");
    case PMP_REQ_CORE_ENABLE_BY_CLNT_ALL:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_CORE_ENABLE_BY_CLNT_ALL");
    case PMP_REQ_CORE_RELOAD_PROFS_FROM_REPO:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_CORE_RELOAD_PROFS_FROM_REPO");
    case PMP_REQ_CORE_RESET_PROF_STATE:
        LOG4CPLUS_INFO(mLogger, "PMP_REQ_CORE_RESET_PROF_STATE");
    default:
        assert(false);
    }
}

void PmpCore::lockAll()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Database::getInstance()->forgetRemoteComplements();
}

void PmpCore::onGetAvailableComplementsResponse(PMPFrame * frame)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    UInt32 count;
    memcpy(&count, frame->data, 4);
    count = ByteOrder::ntoh32(count);
    tProfilesAndVersionsList remoteProfiles;
    LOG4CPLUS_INFO(mLogger, "complements count: " + convertIntegerToString(count));
    int pos = 4;
    for (int i = 0; i < count; i++)
    {
        std::string str = bufferToString(frame->data + pos, true);
        LOG4CPLUS_INFO(mLogger, "complement: " + str);
        pos += stringInBufSize(str);
        UInt32 version;
        memcpy(&version, frame->data + pos, sizeof(version));
        pos += sizeof(version);
        version = ByteOrder::ntoh32(version);
        LOG4CPLUS_DEBUG(mLogger, "complement version: " + convertIntegerToString(version));
        remoteProfiles.push_back(std::make_pair(str, version));
    }
    Database::getInstance()->setAvailableRemoteComplements(remoteProfiles);
    iviLink::SystemController::SystemControllerStub::getInstance()->onProfileManagerHandshakeDone();
}
}
}
