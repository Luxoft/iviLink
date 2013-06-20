/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 26.03.2013
 *
 * Stores ProfileHolders to be accessed by profile proxies, and redirects
 * incoming service requests to Applications that support them.
 */

#include <cassert>

#include "CServiceManager.hpp"
#include "PmpCore.hpp"
#include "PmpPIM.hpp"

#include "Database.hpp"
#include "ApplicationRegistry.hpp"

using namespace iviLink::PMP;
using namespace iviLink::App::Service;

Logger CServiceManager::mLogger = Logger::getInstance("iviLink::AppLib::CServiceManager");

CServiceManager::CServiceManager()
{
}

CServiceManager::~CServiceManager()
{
}

CServiceManager * CServiceManager::getInstance(iviLink::Android::AppInfo)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    static CServiceManager * instance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^
            {
                LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
                instance = new CServiceManager();
            });
    return instance;
}

iviLink::BaseProfileApi * CServiceManager::getProfile(const iviLink::Service::Uid& service,
        const iviLink::Profile::ApiUid& api)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mProfilesDataMutex);
    std::map<std::string, Profile::ProfileHolder*>::iterator iter = mProfilesMap.find(
            getProfilesMapKey(service, api));
    if (iter != mProfilesMap.end())
    {
        BaseProfileApi * result = (*iter).second->profileInstance();
        return result;
    }
    return NULL;
}

void CServiceManager::releaseProfile(const iviLink::Service::Uid& service,
        const iviLink::Profile::ApiUid& api)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mProfilesDataMutex);
    std::map<std::string, Profile::ProfileHolder*>::iterator iter = mProfilesMap.find(
            getProfilesMapKey(service, api));
    if (iter != mProfilesMap.end())
    {
        (*iter).second->release();
    }
}

std::string CServiceManager::getProfilesMapKey(const iviLink::Service::Uid& service,
        const Profile::ApiUid& api)
{
    std::string result = service.value() + "|" + api.value();
    LOG4CPLUS_INFO(mLogger, "generated map key: " + result);
    return result;
}

void CServiceManager::registerProfileCallbacks(const Profile::ApiUid& profileApi,
        Profile::IProfileCallbackProxy* callbacks)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mCallbacksMutex);
    mCallbacksMap[profileApi] = callbacks;
}

iviLink::Profile::IProfileCallbackProxy * CServiceManager::getProfileCallbacks(
        const Profile::ApiUid &profileApi)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mCallbacksMutex);
    if (mCallbacksMap.find(profileApi) != mCallbacksMap.end())
    {
        return mCallbacksMap[profileApi];
    }
    return NULL;
}

void CServiceManager::setProfileForServiceApi(Profile::ProfileHolder* holder,
        const iviLink::Service::Uid& service, const Profile::ApiUid& api)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mProfilesDataMutex);
    mProfilesMap[getProfilesMapKey(service, api)] = holder;
}

void CServiceManager::eraseProfileRecordForServiceApi(const iviLink::Service::Uid& service,
        const Profile::ApiUid& api)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mProfilesDataMutex);
    std::map<std::string, Profile::ProfileHolder*>::iterator findIter = mProfilesMap.find(
            getProfilesMapKey(service, api));
    if (findIter != mProfilesMap.end())
    {
        mProfilesMap.erase(findIter);
    }
}

void CServiceManager::incomingProfileRequest(std::string const& service, std::string const& profile,
        std::string const& piuid)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Application * app = ApplicationRegistry::getInstance()->getApplicationWithEnabledService(
            service);
    if (app == NULL)
    {
        return;
    }
    app->processIncomingCreateRequest(service, profile, piuid);
}

void CServiceManager::incomingProfileDiedRequest(std::string const& service,
        std::string const& piuid)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Service::Uid localService = Database::getInstance()->getLocalComplementToRemoteService(
            service);
    if (localService.value().empty())
    {
        LOG4CPLUS_ERROR(mLogger, "Could not find local complementing service for: " + service);
        return;
    }
    Application * app = ApplicationRegistry::getInstance()->getApplicationWithActiveService(
            localService);
    if (app == NULL)
    {
        LOG4CPLUS_ERROR(mLogger, "No living app with service: " + localService.value());
        return;
    }
    app->processIncomingDiedRequest(localService.value(), piuid);
}
