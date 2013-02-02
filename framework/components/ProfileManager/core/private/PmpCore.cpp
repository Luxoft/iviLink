/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 


#include <cassert>

#include "PmpCore.hpp"
#include "ProfileRepository.hpp"
#include "PmpCoreProtocol.hpp"
#include "EventFactory.hpp"
#include "PmpCoreProfileInfo.hpp"

static const iviLink::Profile::ApiUid gsAuthProfilePapiUid("AuthenticationProfile_PAPI_UID");

namespace iviLink
{

namespace PMP
{

Logger PmpCore::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.PmpCore"));

PmpCore::PmpCore(ProfileRepository * repository, std::string &platform)
    : mRepository(repository)
    , mPlatform(platform)
    , mActualRepoState(false)
    , mpPim(NULL)
    , mpCoreProtocol(NULL)
    , mEventFactory(NULL)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mRepository);
}

PmpCore::~PmpCore()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

BaseError PmpCore::addProfile(const std::string &xmlManifestPath)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    BaseError err = mRepository->addProfile(xmlManifestPath);
    if (err.isNoError())
    {
        mActualRepoState = false;
    }
    return err;
}

BaseError PmpCore::removeProfile(Profile::Uid profileUid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    BaseError err = mRepository->removeProfile(profileUid);
    if (err.isNoError())
    {
        mActualRepoState = false;
    }
    return err;
}

BaseError PmpCore::addProfileImplementation(Profile::Uid profileID, const LibDescriptor& library)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    BaseError err = mRepository->addProfileImplementation(profileID,library);
    if (err.isNoError())
    {
        mActualRepoState = false;
    }
    return err;
}

BaseError PmpCore::removeProfileImplementation(Profile::Uid profileUid, const std::string &platform)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    BaseError err = mRepository->removeProfileImplementation(profileUid,platform);
    if (err.isNoError())
    {
        mActualRepoState = false;
    }
    return err;
}

void PmpCore::findProfiles(Profile::ApiUid profileApiUid,
        const std::map<std::string, std::string> & profileParameters
        , std::list<Profile::Uid> & profiles, bool enabledProfiles)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    std::list<LibInfo> profileInfos;
    profiles.clear();
    findProfiles(profileApiUid, profileParameters, mPlatform,profileInfos,enabledProfiles);
    for (std::list<LibInfo>::const_iterator it = profileInfos.begin(); profileInfos.end() != it; ++it)
    {
        profiles.push_back(it->uid);
    }
}

void PmpCore::findProfiles(Profile::ApiUid id, const std::map<std::string, std::string> & profileArguments, const std::string &platform, std::list<LibInfo>& profiles, bool onlyAvailable)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    profiles.clear();
    std::list<LibInfo> list;
    list = mRepository->findProfiles(id, profileArguments, platform);
    LOG4CPLUS_INFO(msLogger, "PmpCore::findProfilesAsync : List size : " + convertIntegerToString(list.size()));
    for (std::list<LibInfo>::iterator it = list.begin(); list.end() != it; ++it)
    {
        LOG4CPLUS_INFO(msLogger, "PmpCore::findProfilesAsync : in cycle");
        if (mProfiles.find(it->uid) == mProfiles.end())
        {
            LOG4CPLUS_INFO(msLogger, "continue");
            continue;
        }
        if ( it->platform == platform && ( !onlyAvailable || mProfiles[it->uid].available() ) )
        {
            LOG4CPLUS_INFO(msLogger, "push_back");
            profiles.push_back((*it));
        }
    }
    LOG4CPLUS_INFO(msLogger, "PmpCore::findProfilesAsync() : res size : " + convertIntegerToString(profiles.size()));
}

bool PmpCore::getManifest(iviLink::BaseUid uid, std::string & manifest)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    manifest = mRepository->getManifest(uid);
    if (manifest == "")
    {
        return false;
    }
    return true;
}

void PmpCore::reloadProfilesFromRepository()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mProfiles.clear();
    std::list <Profile::Uid> tmp = mRepository->getProfilesList();
    std::list<Profile::Uid> list;
    for(std::list<Profile::Uid>::iterator it = tmp.begin(); tmp.end() != it; ++it)
    {
        list.push_back((*it));
    }

    LOG4CPLUS_INFO(msLogger, "List of Profiles has been returned");
    LOG4CPLUS_INFO(msLogger, "List size : " + convertIntegerToString(list.size()));
    for (std::list<Profile::Uid>::const_iterator it = list.begin(); list.end() != it; ++it)
    {
        LOG4CPLUS_INFO(msLogger, "UID : " + (*it).value());
        std::map<std::string, std::string> profileArguments;
        std::list<LibInfo> impls;
        impls = mRepository->findProfiles((*it),profileArguments,mPlatform);
        LOG4CPLUS_INFO(msLogger, "findProfiles result : " + convertIntegerToString(impls.size()));

        for (std::list<LibInfo>::const_iterator lit = impls.begin(); impls.end() != lit; ++lit)
        {
            LOG4CPLUS_INFO(msLogger, "mPlatform: " + mPlatform +
                    ", lit->platform : " + lit->platform + ", lit->path : " + lit->path);
            if (lit->platform == mPlatform)
            {
                LOG4CPLUS_INFO(msLogger, "Found implementation: " + lit->path);
                std::string man;
                getManifest((*it),man);
                mProfiles[(*it)] = PmpCoreProfileInfo(man);
                mProfiles[(*it)].setLibrary(lit->path);
                break;
            }
        }
    }

    mActualRepoState = true;
}

void PmpCore::resetProfilesState()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    disableByComplementAll();
    onLockAll();
    mpCoreProtocol->getAvailableProfileComplementsRequest();
}

void PmpCore::getAvailbleProfileComplementsResponse(const std::list<Profile::Uid> & complements)
{
    for (std::list<Profile::Uid>::const_iterator it = complements.begin(); complements.end() != it; ++it)
    {
        LOG4CPLUS_INFO(msLogger, "Available complement: " + it->value());
        for (CoreProfileInfo::iterator pit = mProfiles.begin(); mProfiles.end() != pit; ++pit)
        {
            LOG4CPLUS_INFO(msLogger, "*it : " + (*it).value() + ", pit->complement: " + pit->second.complement().value());
            if ((*it) == pit->second.complement())
            {
                LOG4CPLUS_INFO(msLogger, "true");
                pit->second.enableByComplement();
            }
        }
    }
    print();
    mEventFactory->coreIsReady();
}

void PmpCore::disableByComplementAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    for (CoreProfileInfo::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->second.disableByComplement();
    }
}

bool PmpCore::lock(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpCoreProtocol)
    {
        return false;
    }
    CoreProfileInfo::iterator it = mProfiles.find(id);
    if (it == mProfiles.end())
    {
        bool res = lockByApiUid(id);
        return res;
    }
    if (!mpPim || mpPim->hasInstance(id))
    {
        return false;
    }
    else
    {
        it->second.lock();
        return mpCoreProtocol->lock(it->second.complement());
    }
}

bool PmpCore::unlock(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpCoreProtocol)
    {
        return false;
    }
    CoreProfileInfo::iterator it = mProfiles.find(id);

    if (it == mProfiles.end())
    {
        bool res = unlockByApiUid(id);
        return res;
    }
    it->second.unlock();
    return mpCoreProtocol->unlock(it->second.complement());
}

bool PmpCore::lockByApiUid(Profile::ApiUid id)
{
    bool res = false;
    std::list<Profile::Uid> lst;
    for (CoreProfileInfo::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        if (it->second.api() == id && mpPim &&
                !mpPim->hasInstance(it->second.uid()))
        {
            res = true;
            it->second.lock();
            lst.push_back(it->second.complement());
        }
    }

    for (std::list<Profile::ApiUid>::iterator it = lst.begin(); lst.end() != it; ++it)
    {
        mpCoreProtocol->lock((*it));
    }
    return res;
}

bool PmpCore::unlockByApiUid(Profile::ApiUid id)
{
    bool res = false;
    std::list<Profile::Uid> lst;
    for (CoreProfileInfo::iterator it = mProfiles.begin();
    mProfiles.end() != it; ++it)
    {
        if (it->second.api() == id)
        {
            res = true;
            it->second.unlock();
            lst.push_back(it->second.complement());
        }
    }

    for (std::list<Profile::Uid>::iterator it = lst.begin();
    lst.end() != it; ++it)
    {
        mpCoreProtocol->unlock((*it));
    }

    return res;
}

void PmpCore::lockAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpCoreProtocol)
    {
        return;
    }

    for (CoreProfileInfo::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->second.lock();
    }
    mpCoreProtocol->lockAll();
}


void PmpCore::unlockAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpCoreProtocol)
    {
        return;
    }

    for (CoreProfileInfo::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->second.unlock();
    }
    mpCoreProtocol->unlockAll();
}

bool PmpCore::getProfileLibPath(Profile::Uid uid, std::string& path)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    CoreProfileInfo::iterator it = mProfiles.find(uid);
    if (it == mProfiles.end())
    {
        LOG4CPLUS_WARN(msLogger, "profile not found");
        return false;
    }
    else
    {
        path = it->second.library();
        LOG4CPLUS_INFO(msLogger, "path = " + path);
        return true;
    }
}

bool PmpCore::disableByClient(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_TRACE(msLogger, "id: " + id.value());
    if (!mpCoreProtocol)
    {
        return false;
    }
    CoreProfileInfo::iterator it = mProfiles.find(id);
    if (it == mProfiles.end())
    {
        return disableApiUidByClient(id);
    }
    it->second.disableByClient();

    if (mpPim && mEventFactory)
    {
        mEventFactory->coreInternalUnloadInstances(mpPim,id);
    }
    mpCoreProtocol->disableByClient(it->second.complement());
    return true;
}

bool PmpCore::enableByClient(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpCoreProtocol)
    {
        LOG4CPLUS_ERROR(msLogger, "no mpCoreProtocol");
        return false;
    }
    CoreProfileInfo::iterator it = mProfiles.find(id);

    if (it == mProfiles.end())
    {
        return enableApiUidByClient(id);
    }
    it->second.enableByClient();
    mpCoreProtocol->enableByClient(it->second.complement());
    return true;
}

bool PmpCore::disableApiUidByClient(Profile::ApiUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    bool res = false;
    std::list<Profile::Uid> lstComp;
    std::list<Profile::Uid> lstProfs;
    for (CoreProfileInfo::iterator it = mProfiles.begin();
            mProfiles.end() != it; ++it)
    {
        if (it->second.api() == id)
        {
            lstProfs.push_back(it->second.uid());
            res = true;
            it->second.disableByClient();
            lstComp.push_back(it->second.complement());
        }
    }

    for(std::list<Profile::Uid>::iterator it = lstProfs.begin();
            lstProfs.end() != it; ++it)
    {
        mpPim->unloadInstances((*it));
    }

    for (std::list<Profile::Uid>::iterator it = lstComp.begin();
            lstComp.end() != it; ++it)
    {
        mpCoreProtocol->disableByClient((*it));
    }
    return res;
}

bool PmpCore::enableApiUidByClient(Profile::ApiUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    bool res = false;

    std::list<Profile::Uid> lst;
    for (CoreProfileInfo::iterator it = mProfiles.begin();
            mProfiles.end() != it; ++it)
    {
        if (it->second.api() == id)
        {
            res = true;
            it->second.enableByClient();
            lst.push_back(it->second.complement());
        }
    }

    for (std::list<Profile::Uid>::iterator it = lst.begin();
            lst.end() != it; ++it)
    {
        mpCoreProtocol->enableByClient((*it));
    }

    return res;
}

void PmpCore::enableByClientAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpCoreProtocol)
    {
        return;
    }

    for (CoreProfileInfo::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->second.enableByClient();
    }
    mpCoreProtocol->enableByClientAll();
}

void PmpCore::getAvailableProfileComplements()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    tComplementsList complementsList;

    for (std::map<Profile::Uid, PmpCoreProfileInfo>::iterator it = mProfiles.begin();
            mProfiles.end() != it; ++it)
    {
        complementsList.push_back(it->second.uid());
    }
    mpCoreProtocol->getAvailableProfileComplementsResponse(complementsList);
}

bool PmpCore::onLock(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    CoreProfileInfo::iterator it = mProfiles.find(id);
    if (it == mProfiles.end())
    {
        return false;
    }
    it->second.lock();
    return true;
}

bool PmpCore::onUnlock(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    CoreProfileInfo::iterator it = mProfiles.find(id);
    if (it == mProfiles.end())
    {
        return false;
    }
    it->second.unlock();
    return true;
}

void PmpCore::onLockAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    for (CoreProfileInfo::iterator it = mProfiles.begin();  mProfiles.end() != it; ++it)
    {
        it->second.lock();
    }
}

void PmpCore::onUnlockAll()
    {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    for (CoreProfileInfo::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->second.unlock();
    }
}

bool PmpCore::onDisableByClient(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    CoreProfileInfo::iterator it = mProfiles.find(id);
    if (it == mProfiles.end())
    {
        return false;
    }
    it->second.disableByClient();
    return true;
}

bool PmpCore::onEnableByClient(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    CoreProfileInfo::iterator it = mProfiles.find(id);
    if (it == mProfiles.end())
    {
        return false;
    }
    it->second.enableByClient();
    return true;
}

void PmpCore::onEnableByClientAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    for (CoreProfileInfo::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->second.enableByClient();
    }
}

void PmpCore::onReloadProfilesFromRepository()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    reloadProfilesFromRepository();
}

void PmpCore::onResetProfilesState()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    resetProfilesState();
}

void PmpCore::onLostConnection()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    disableByComplementAll();
}

bool PmpCore::actualCoreAndRepoState() const
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    return mActualRepoState;
}

void PmpCore::init(IPmpPimToCore * pPim, PmpCoreProtocol * pCoreProtocol,
        EventFactory * factory)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mpPim = pPim;
    mpCoreProtocol = pCoreProtocol;
    mEventFactory = factory;
}

void PmpCore::print() const
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    std::string print = "*****\n";
    print += "Platform: ";
    print += mPlatform;
    std::string str = mActualRepoState ? "" : "not ";
    print += "\nCore state is " + str + "actual";

    for (CoreProfileInfo::const_iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        print += "\n\t***";
        print += "\n\tUID :" + it->second.uid().value();
        print += "\n\tAPI : " + it->second.api().value();
        print += "\n\tComplement : " + it->second.complement().value();
        print += it->second.available() ? "\n\tAvailable" : "\n\tNot available";
        print += it->second.enabled() ? "\n\tEnabled" : "\n\tDisabled";
        print += it->second.locked() ? "\n\tLocked" : "\n\tUnlocked";
        print += "\n\tLibrary : " + it->second.library();
    }
    print += "\n*****";
    LOG4CPLUS_INFO(msLogger, print);
}


bool PmpCore::getComplementary(Profile::Uid const& profUid,
        Profile::Uid& complProfUid) const
{
    bool res = true;

    {
        CoreProfileInfo::const_iterator it = mProfiles.find(profUid);
        if (it == mProfiles.end())
        {
            res = false;
        }
        else
        {
            complProfUid = it->second.complement();
        }
    }

    return res;
}

bool PmpCore::getPapiUid(iviLink::Profile::Uid const& profUid,
        iviLink::Profile::ApiUid& papiUid) const
{
    bool res = true;
    {
        CoreProfileInfo::const_iterator it = mProfiles.find(profUid);
        if (it == mProfiles.end())
        {
            res = false;
        }
        else
        {
            papiUid = it->second.api();
        }
    }

    return res;
}

bool PmpCore::unlockAuthProfile()
{
    return unlockByApiUid(gsAuthProfilePapiUid);
}

void PmpCore::unlockBySysCtrlAll()
{
    return unlockAll();
}

}

}
