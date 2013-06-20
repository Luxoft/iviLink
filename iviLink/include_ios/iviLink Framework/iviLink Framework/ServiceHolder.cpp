/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 26.03.2013
 */

#include <cassert>

#include "ServiceHolder.hpp"
#include "Database.hpp"
#include "CServiceManager.hpp"
#include "PmpPIM.hpp"

using namespace iviLink::App;
using namespace iviLink::PMP;

class iviLink::PMAL::PIM::CProfileEnabler
{
    CProfileEnabler();
public:
    static void enable(iviLink::Profile::ProfileInternal* p)
    {
        assert(p);
        p->setEnabled(true);
    }

    static void disable(iviLink::Profile::ProfileInternal* p)
    {
        assert(p);
        p->setEnabled(false);
    }
};

class iviLink::PMAL::PIM::CProfileDestroyer
{
    CProfileDestroyer();
public:
    static void destroy(iviLink::Profile::ProfileInternal* p)
    {
        assert(p);
        delete p;
    }
};

ServiceHolder::ServiceHolder(std::string const& uid) :
        mLogger(Logger::getInstance("iviLink::App::Service::iOS::ServiceHolder::" + uid))
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mUid = uid;
    mComplementUid = Database::getInstance()->getRemoteComplementToLocalService(uid);
}

ServiceHolder::~ServiceHolder()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    unload();
}

iviLink::Error ServiceHolder::load()
{
    iviLink::generateUid(); // app uid
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Database * database = Database::getInstance();
    if (!database->isKnownService(mUid))
    {
        return iviLink::Error(SERVICE_NOT_FOUND, "Service is not known or cannot be loaded", false);
    }
    if (mComplementUid.empty())
    {
        return iviLink::Error(SERVICE_NOT_FOUND, "Complementing service not known", false);
    }
    LOG4CPLUS_INFO(mLogger, "getting profiles for service: " + mUid);
    iviLink::tApisList apisAndImpls = database->getProfilesAndApis(mUid);
    for (tApisList::iterator iter = apisAndImpls.begin(); iter != apisAndImpls.end(); iter++)
    {
        LOG4CPLUS_INFO(mLogger, "getting profiles for api: " + (*iter).mApiUid.value());
        Profile::Uid profile = (*iter).mImplInfo.mUid;
        Profile::Uid complement = (*iter).mImplInfo.mComplementUid;
        if (!(database->isLocalProfileEnabled(profile.value())
                && database->isRemoteProfileEnabled(complement.value())))
        {
            LOG4CPLUS_INFO(mLogger,
                    "Either the profile or its complement is disabled: " + profile.value());
            cleanUpAfterError();
            return iviLink::Error(NO_PROFILES_FOUND,
                    "Either profile or its complement is disabled: " + profile.value(), false);
        }
        Profile::IProfileCallbackProxy * callback =
                Service::CServiceManager::getInstance()->getProfileCallbacks((*iter).mApiUid);
        if (callback == NULL)
        {
            cleanUpAfterError();
            return iviLink::Error(CALLBACKS_NOT_REGISTERED, "No callback for: " + profile.value(),
                    false);
        }
        Profile::ProfileHolder * profileHolder = PmpPIM::getInstance()->createProfile(callback,
                mUid, profile.value(), iviLink::generateUid().value());
        if (profileHolder == NULL)
        {
            cleanUpAfterError();
            return iviLink::Error(PIM_ERROR, "Error creating profile", false);
        }
        mProfilesMap[profile] = profileHolder;
        PmpPIM::getInstance()->sendCreateProfileRequest(complement, profileHolder->getProfileID(),
                mComplementUid);
        App::Service::CServiceManager::getInstance()->setProfileForServiceApi(profileHolder, mUid,
                (*iter).mApiUid);
        PMAL::PIM::CProfileEnabler::enable(profileHolder);
    }
    return iviLink::Error::NoError();
}

void ServiceHolder::silentUnload()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (allProfilesDead())
    {
        return;
    }
    tLoadedProfilesMap::iterator iter = mProfilesMap.begin();
    for (; iter != mProfilesMap.end(); iter++)
    {
        Profile::ProfileHolder * profileHolder = iter->second;
        Profile::ApiUid api;
        tApisList allApis = Database::getInstance()->getProfilesAndApis(mUid);
        for (tApisList::iterator apiIter = allApis.begin(); apiIter != allApis.end(); iter++)
        {
            if (apiIter->mImplInfo.mUid == iter->first)
            {
                api = apiIter->mApiUid;
                break;
            }
        }
        Service::CServiceManager::getInstance()->eraseProfileRecordForServiceApi(mUid, api);
        PMAL::PIM::CProfileDestroyer::destroy(profileHolder);
    }
    mProfilesMap.clear();
}

bool ServiceHolder::allProfilesLoaded()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (allProfilesDead())
    {
        return false;
    }
    Database * database = Database::getInstance();
    if (!database->isKnownService(mUid))
    {
        return false;
    }
    iviLink::tApisList apisAndImpls = database->getProfilesAndApis(mUid);
    // check that there is a non-empty record for each profile implementation for every PAPI in the service in the map
    for (tApisList::iterator iter = apisAndImpls.begin(); iter != apisAndImpls.end(); iter++)
    {
        Profile::Uid profile = (*iter).mImplInfo.mUid;
        tLoadedProfilesMap::iterator findProfileIt = mProfilesMap.find(profile);
        if (findProfileIt == mProfilesMap.end())
        {
            return false;
        }
        if (findProfileIt->second == NULL)
        {
            return false;
        }
    }
    return true;
}

iviLink::Error ServiceHolder::unload()
{
    if (allProfilesDead())
    {
        return iviLink::Error(SERVICE_NOT_FOUND, "Service is not loaded: " + mUid, true);
    }
    tLoadedProfilesMap::iterator iter = mProfilesMap.begin();
    Database * database = Database::getInstance();
    for (; iter != mProfilesMap.end(); iter++)
    {
        Profile::ProfileHolder * profileHolder = iter->second;
        assert(profileHolder);
        PMP::PmpPIM::getInstance()->sendProfileDiedRequest(profileHolder->getProfileID(), mUid);
        Profile::ApiUid api;
        tApisList allApis = database->getProfilesAndApis(mUid);
        for (tApisList::iterator apiIter = allApis.begin(); apiIter != allApis.end(); iter++)
        {
            if (apiIter->mImplInfo.mUid == iter->first)
            {
                api = apiIter->mApiUid;
                break;
            }
        }
        assert(!api.value().empty());
        Service::CServiceManager::getInstance()->eraseProfileRecordForServiceApi(mUid, api);
        PMAL::PIM::CProfileEnabler::disable(profileHolder);
        PMAL::PIM::CProfileDestroyer::destroy(profileHolder);
    }
    mProfilesMap.clear();
    return iviLink::Error::NoError();
}

iviLink::Error ServiceHolder::incomingProfile(tStringPtr requestedProfile, tStringPtr piuid)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Database * database = Database::getInstance();
    if (!database->isKnownService(mUid))
    {
        return iviLink::Error(SERVICE_NOT_FOUND, "Service not known or cannot be loaded", false);
    }
    LOG4CPLUS_INFO(mLogger, "getting profiles for service: " + mUid);
    iviLink::tApisList apisAndImpls = database->getProfilesAndApis(mUid);
    bool didStuff = false;
    for (tApisList::iterator iter = apisAndImpls.begin(); iter != apisAndImpls.end(); iter++)
    {
        LOG4CPLUS_INFO(mLogger, "getting profiles for api: " + (*iter).mApiUid.value());
        Profile::Uid profile = (*iter).mImplInfo.mUid;
        if (profile.value() != (*requestedProfile.get()))
        {
            continue;
        }
        didStuff = true;
        Profile::IProfileCallbackProxy * callback =
                Service::CServiceManager::getInstance()->getProfileCallbacks((*iter).mApiUid);
        if (callback == NULL)
        {
            cleanUpAfterError();
            return iviLink::Error(CALLBACKS_NOT_REGISTERED, "No callback for: " + profile.value(),
                    false);
        }
        Profile::ProfileHolder * profileHolder = PmpPIM::getInstance()->createProfile(callback,
                mUid, profile.value(), (*piuid.get()));
        if (profileHolder == NULL)
        {
            cleanUpAfterError();
            return iviLink::Error(PIM_ERROR, "Error creating profile", false);
        }
        mProfilesMap[profile] = profileHolder;
        App::Service::CServiceManager::getInstance()->setProfileForServiceApi(profileHolder, mUid,
                (*iter).mApiUid);
        PMAL::PIM::CProfileEnabler::enable(profileHolder);
        break;
    }
    if (!didStuff)
    {
        cleanUpAfterError();
        return iviLink::Error(NO_PROFILES_FOUND, "no profiles per request found", false);
    }
    return iviLink::Error::NoError();
}

iviLink::Error ServiceHolder::incomingProfileDied(tStringPtr piuid)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    Database * database = Database::getInstance();
    iviLink::tApisList apisAndImpls = database->getProfilesAndApis(mUid);
    for (tApisList::iterator iter = apisAndImpls.begin(); iter != apisAndImpls.end(); iter++)
    {
        Profile::Uid profile = (*iter).mImplInfo.mUid;
        tLoadedProfilesMap::iterator findProfileIt = mProfilesMap.find(profile);
        if (findProfileIt == mProfilesMap.end())
        {
            continue;
        }
        Profile::ProfileHolder * profileHolder = findProfileIt->second;
        if (profileHolder == NULL)
        {
            continue;
        }
        if (profileHolder->getProfileID().value() == (*piuid.get()))
        {
            LOG4CPLUS_INFO(mLogger,
                    "profile died: IUid: " + (*piuid.get()) + " impl uid:" + profile.value());
            if (profileHolder->isLocked())
            {
                LOG4CPLUS_ERROR(mLogger, "TODO implement delayed unloading");
                assert(false);
            }
            mProfilesMap.erase(findProfileIt);
            Service::CServiceManager::getInstance()->eraseProfileRecordForServiceApi(mUid,
                    (*iter).mApiUid);
            PMAL::PIM::CProfileEnabler::disable(profileHolder);
            PMAL::PIM::CProfileDestroyer::destroy(profileHolder);
            return iviLink::Error::NoError();
        }
    }
    return iviLink::Error(NO_PROFILES_FOUND, "profile not found", false);
}

bool ServiceHolder::allProfilesDead()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    return mProfilesMap.empty();
}

void ServiceHolder::cleanUpAfterError()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    // cleans up all already loaded profiles
    unload();
}
