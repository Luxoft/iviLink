#include <cassert>
#include <dispatch/dispatch.h>

#include "Database.hpp"

using namespace iviLink::PMP;

#define AUTHENTICATION_PROFILE_IMPL_UID "AuthenticationProfileImpl_UID"

static iviLink::ServiceInfo authenticationServiceInfo("AuthenticationService",
                                                      "AuthenticationProfile_PAPI_UID",
                                                      "AuthenticationProfileImpl_UID", 1);

Logger Database::mLogger = Logger::getInstance("iviLink::PMP::iOS::Database");

Database::Database()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    forgetRemoteComplements();
}

void Database::printDatabase() const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mServicesMutex.lockRead();
    for (tServiceInfoMap::const_iterator iter = mServicesList.begin(); iter != mServicesList.end();
            iter++)
    {
        LOG4CPLUS_INFO(mLogger,
                "=====================================================================");
        LOG4CPLUS_INFO(mLogger, "service:               " + (*iter).first);
        LOG4CPLUS_INFO(mLogger,
                "complementing service: " + (*iter).second.mServiceComplement.value());
        for (tApisList::const_iterator apisIter = (*iter).second.mApiInfos.begin();
                apisIter != (*iter).second.mApiInfos.end(); apisIter++)
        {
            LOG4CPLUS_INFO(mLogger, "api:               " + (*apisIter).mApiUid.value());
            LOG4CPLUS_INFO(mLogger, "implementation:    " + (*apisIter).mImplInfo.mUid.value());
            LOG4CPLUS_INFO(mLogger,
                    "complement:        " + (*apisIter).mImplInfo.mComplementUid.value());
        }
        LOG4CPLUS_INFO(mLogger,
                "=====================================================================");
    }
    mServicesMutex.unlockRead();
}

void Database::forgetRemoteComplements()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mServicesMutex.lockWrite();
    mServicesList = mInformant.getKnownServices();
    mServicesList[authenticationServiceInfo.mServiceUid.value()] = authenticationServiceInfo;
    mServicesMutex.unlockWrite();
    updateProfileLists();
    printDatabase();
}

void Database::enableAuthenticationProfileLocally()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mProfilesMutex.lockWrite();
    tEnabledProfilesList::iterator iter = mProfiles.begin();
    for (; iter != mProfiles.end(); iter++)
    {
        if (iter->mProfileImplUid == std::string(AUTHENTICATION_PROFILE_IMPL_UID))
        {
            iter->mIsEnabled = true;
            break;
        }
    }
    mProfilesMutex.unlockWrite();
}


UInt32 Database::getVersionForProfileInService(std::string const& profileUid, std::string const& serviceUid) const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    UInt32 result = 0;
    mServicesMutex.lockRead();
    tServiceInfoMap::const_iterator findIt = mServicesList.find(serviceUid);
    if (findIt != mServicesList.end())
    {
        for (tApisList::const_iterator apis = findIt->second.mApiInfos.begin(); apis != findIt->second.mApiInfos.end(); apis++)
        {
            if (apis->mImplInfo.mUid.value() == profileUid)
            {
                result = apis->mImplInfo.mVersion;
            }
        }
    }
    mServicesMutex.unlockRead();
    return result;
}

iviLink::Profile::Uid Database::getAuthenticationProfileUid() const
{
    static Profile::Uid authUid(AUTHENTICATION_PROFILE_IMPL_UID);
    return authUid;
}

void Database::enableAuthenticationProfileRemote()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mRemoteProfilesMutex.lockWrite();
    tEnabledProfilesList::iterator iter = mRemoteProfiles.begin();
    for (; iter != mRemoteProfiles.end(); iter++)
    {
        if (iter->mProfileImplUid == std::string(AUTHENTICATION_PROFILE_IMPL_UID))
        {
            iter->mIsEnabled = true;
            break;
        }
    }
    mRemoteProfilesMutex.unlockWrite();
}

bool Database::isKnownService(const std::string & uid) const
{
    bool result = false;
    mServicesMutex.lockRead();
    result = (mServicesList.find(uid) != mServicesList.end());
    mServicesMutex.unlockRead();
    return result;
}

std::string Database::getRemoteComplementToLocalService(std::string const& service) const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Service::Uid result(std::string(""));
    LOG4CPLUS_INFO(mLogger, "looking for complement for service: " + service);
    mServicesMutex.lockRead();
    tServiceInfoMap::const_iterator iter = mServicesList.find(service);
    if (iter != mServicesList.end())
    {
        result = iter->second.mServiceComplement;
        LOG4CPLUS_INFO(mLogger,
                "found complement for service: " + service + " is:" + result.value());
    }
    mServicesMutex.unlockRead();
    return result.value();
}

std::string Database::getLocalComplementToRemoteService(std::string const& service) const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "looking for local complement for service: " + service);
    std::string result("");
    mServicesMutex.lockRead();
    tServiceInfoMap::const_iterator iter = mServicesList.begin();
    for (; iter != mServicesList.end(); iter++)
    {
        if (iter->second.mServiceComplement.value() == service)
        {
            result = iter->first;
            break;
        }
    }
    mServicesMutex.unlockRead();
    return result;
}

void Database::setAvailableRemoteComplements(tProfilesAndVersionsList complements)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mServicesMutex.lockWrite();
    tServiceInfoMap::iterator servicesIter = mServicesList.begin();
    while (servicesIter != mServicesList.end())
    {
        LOG4CPLUS_INFO(mLogger, "validating service " + servicesIter->first);
        bool needErase = false;
        for (tApisList::const_iterator apisIter = servicesIter->second.mApiInfos.begin();
                apisIter != servicesIter->second.mApiInfos.end(); apisIter++)
        {
            LOG4CPLUS_INFO(mLogger, "checking api: " + apisIter->mApiUid.value());
            std::string complement = (*apisIter).mImplInfo.mComplementUid.value();
            UInt32 version = apisIter->mImplInfo.mVersion;
            bool found = false;
            for (tProfilesAndVersionsList::const_iterator remoteComplementsIter = complements.begin();
                 remoteComplementsIter != complements.end(); remoteComplementsIter ++)
            {
                if (remoteComplementsIter->first == complement && remoteComplementsIter->second == version)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                LOG4CPLUS_FATAL(mLogger,
                                "No remote complement for : " + (*apisIter).mImplInfo.mUid.value()
                                + " expected: " + complement);
                needErase = true;
                break;
            }
        }
        if (needErase)
        {
            LOG4CPLUS_INFO(mLogger, "service " + servicesIter->first + " is incomplete");
            mServicesList.erase(servicesIter++);
        }
        else
        {
            LOG4CPLUS_INFO(mLogger, "service " + servicesIter->first + " is complete");
            servicesIter++;
        }
    }
    mServicesMutex.unlockWrite();
    updateProfileLists();
}

void Database::updateProfileLists()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mProfilesMutex.lockWrite();
    mRemoteProfilesMutex.lockWrite();
    mServicesMutex.lockRead();
    mProfiles.clear();
    mRemoteProfiles.clear();
    for (tServiceInfoMap::iterator iter = mServicesList.begin(); iter != mServicesList.end();
            iter++)
    {
        for (tApisList::iterator apisIter = (*iter).second.mApiInfos.begin();
                apisIter != (*iter).second.mApiInfos.end(); apisIter++)
        {
            mProfiles.push_back(EnabledProfileInfo(apisIter->mImplInfo.mUid.value()));
            mRemoteProfiles.push_back(EnabledProfileInfo(apisIter->mImplInfo.mComplementUid.value()));
        }
    }
    mServicesMutex.unlockRead();
    mProfilesMutex.unlockWrite();
    mRemoteProfilesMutex.unlockWrite();
}

bool Database::isLocalProfileEnabled(std::string const& uid) const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    bool result = false;
    mProfilesMutex.lockRead();
    tEnabledProfilesList::const_iterator iter = mProfiles.begin();
    for (; iter != mProfiles.end(); iter++)
    {
        if ((*iter).mProfileImplUid == uid)
        {
            LOG4CPLUS_INFO(mLogger,
                    uid + " is " + (iter->mIsEnabled ? "enabled" : "disabled"));
            result = iter->mIsEnabled;
        }
    }
    mProfilesMutex.unlockRead();
    return result;
}

bool Database::isRemoteProfileEnabled(std::string const& uid) const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    bool result = false;
    mRemoteProfilesMutex.lockRead();
    tEnabledProfilesList::const_iterator iter = mRemoteProfiles.begin();
    for (; iter != mRemoteProfiles.end(); iter++)
    {
        if (iter->mProfileImplUid == uid)
        {
            LOG4CPLUS_INFO(mLogger,
                    uid + " is " + (iter->mIsEnabled ? "enabled" : "disabled"));
            result = iter->mIsEnabled;
            break;
        }
    }
    mRemoteProfilesMutex.unlockRead();
    return result;
}

iviLink::tApisList Database::getProfilesAndApis(std::string const& service) const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mServicesMutex.lockRead();
    iviLink::tApisList result;
    tServiceInfoMap::const_iterator iter = mServicesList.find(service);
    assert(iter != mServicesList.end());
    result = iter->second.mApiInfos;
    mServicesMutex.unlockRead();
    return result;
}

tProfilesAndVersionsList Database::getAvailableProfiles() const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mServicesMutex.lockRead();
    tProfilesAndVersionsList result;
    for (tServiceInfoMap::const_iterator iter = mServicesList.begin(); iter != mServicesList.end();
            iter++)
    {
        for (tApisList::const_iterator apisIter = (*iter).second.mApiInfos.begin();
                apisIter != (*iter).second.mApiInfos.end(); apisIter++)
        {
            result.push_back(std::make_pair(apisIter->mImplInfo.mUid.value(), apisIter->mImplInfo.mVersion));
        }
    }
    mServicesMutex.unlockRead();
    return result;
}

void Database::lockAll()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mProfilesMutex.lockWrite();
    tEnabledProfilesList::iterator iter = mProfiles.begin();
    for (; iter != mProfiles.end(); iter++)
    {
        (*iter).mIsEnabled = false;
    }
    mProfilesMutex.unlockWrite();
}

void Database::unlockAll()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mProfilesMutex.lockWrite();
    tEnabledProfilesList::iterator iter = mProfiles.begin();
    for (; iter != mProfiles.end(); iter++)
    {
        (*iter).mIsEnabled = true;
    }
    mProfilesMutex.unlockWrite();
}
void Database::lockAllRemote()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mRemoteProfilesMutex.lockWrite();
    tEnabledProfilesList::iterator iter = mRemoteProfiles.begin();
    for (; iter != mRemoteProfiles.end(); iter++)
    {
        (*iter).mIsEnabled = false;
    }
    mRemoteProfilesMutex.unlockWrite();
}

void Database::unlockAllRemote()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mRemoteProfilesMutex.lockWrite();
    tEnabledProfilesList::iterator iter = mRemoteProfiles.begin();
    for (; iter != mRemoteProfiles.end(); iter++)
    {
        (*iter).mIsEnabled = true;
    }
    mRemoteProfilesMutex.unlockWrite();
}

Database::~Database()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

Database * Database::getInstance()
{
    static Database * instance = NULL;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = new Database();
    });
    return instance;
}

