/* 
 * 
 * iviLINK SDK, version 1.1.2
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
 * 
 */


/**
 * @file                PmpCore.cpp
 * @ingroup             Profile Manager
 * @author              Plachkov Vyacheslav <vplachkov@luxoft.com>
 * @date                10.01.2013
 *
 * Implements PmpCore class
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

void PmpCore::findProfiles(const Profile::ApiUid & id,
        const std::map<std::string, std::string> & profileParameters
        , std::list<Profile::Uid> & profiles, bool enabledProfiles)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mRepository);
    std::map<int, Profile::Uid> profMap;
    for (CoreProfileInfoList::const_iterator it = mProfiles.begin(); it != mProfiles.end(); ++it)
    {
        if ((id == it->api() || id == it->uid()) &&
                (!enabledProfiles || it->available()))
        {
            profMap.insert(std::make_pair(mRepository->getRelevance(it->uid(), it->version(),profileParameters), it->uid()));
        }
    }

    profiles.clear();

    for (std::map<int, Profile::Uid>::const_reverse_iterator it = profMap.rbegin();
            profMap.rend() != it; ++it)
    {
        profiles.push_back(it->second);
    }
    LOG4CPLUS_INFO(msLogger, "PmpCore::findProfiles : List size : " + convertIntegerToString(profiles.size()));
}

void PmpCore::reloadProfilesFromRepository()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mProfiles.clear();
    std::list<LibInfo> libs;
    mRepository->getAllLibs(libs);

    for (std::list<LibInfo>::const_iterator it = libs.begin();
            libs.end() != it; ++it)
    {
        if (mPlatform == it->platform)
        {
            mProfiles.push_back(PmpCoreProfileInfo(*it));
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

void PmpCore::getAvailbleProfileComplementsResponse(const std::list<std::pair<Profile::Uid, UInt32> > & complements)
{
    for (std::list<std::pair<Profile::Uid, UInt32> >::const_iterator it = complements.begin(); complements.end() != it; ++it)
    {
        LOG4CPLUS_INFO(msLogger, "Available complement: " + it->first.value());
        for (CoreProfileInfoList::iterator pit = mProfiles.begin(); mProfiles.end() != pit; ++pit)
        {
            LOG4CPLUS_INFO(msLogger, "*it : " + it->first.value() + ", pit->complement: " + pit->complement().value());
            if (it->first == pit->complement() && it->second == pit->version())
            {
                LOG4CPLUS_INFO(msLogger, "true");
                pit->enableByComplement();
            }
        }
    }

    print();
    mEventFactory->coreIsReady();
}

void PmpCore::disableByComplementAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->disableByComplement();
    }
}

bool PmpCore::lock(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpCoreProtocol || !mpPim || mpPim->hasInstance(id))
    {
        return false;
    }

    bool res = false;
    Profile::Uid complement;
    for (CoreProfileInfoList::iterator it = mProfiles.begin();
            mProfiles.end() != it; ++it)
    {
        if (id == it->uid())
        {
            res = true;
            it->lock();
            complement = it->complement();
        }
    }
    if (!res)
    {
        return lockByApiUid(id);
    }
    else
    {
        return mpCoreProtocol->lock(complement);
    }
}

bool PmpCore::unlock(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpCoreProtocol)
    {
        return false;
    }

    bool res = false;
    Profile::Uid complement;
    for (CoreProfileInfoList::iterator it = mProfiles.begin();
            mProfiles.end() != it; ++it)
    {
        if (id == it->uid())
        {
            res = true;
            it->unlock();
            complement = it->complement();
        }
    }
    if (!res)
    {
        return unlockByApiUid(id);
    }
    else
    {
        return mpCoreProtocol->unlock(complement);
    }
}

bool PmpCore::lockByApiUid(Profile::ApiUid id)
{
    bool res = false;
    std::list<Profile::Uid> lst;
    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        if (it->api() == id && mpPim &&
                !mpPim->hasInstance(it->uid()))
        {
            res = true;
            it->lock();
            lst.push_back(it->complement());
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
    for (CoreProfileInfoList::iterator it = mProfiles.begin();
    mProfiles.end() != it; ++it)
    {
        if (it->api() == id)
        {
            res = true;
            it->unlock();
            lst.push_back(it->complement());
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

    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->lock();
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

    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->unlock();
    }
    mpCoreProtocol->unlockAll();
}

bool PmpCore::getProfileLibPath(Profile::Uid uid, std::string& path) const
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    UInt32 version = 0;
    for (CoreProfileInfoList::const_iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        if (it->uid() == uid && it->version() > version)
        {
            path = it->library();
            version = it->version();
        }
    }
    LOG4CPLUS_INFO(msLogger, "Found lib path: " + path + " for UID: " + uid.value() +
             ", version: " + convertIntegerToString(version));
    return static_cast<bool> (version);
}

bool PmpCore::disableByClient(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_TRACE(msLogger, "id: " + id.value());
    if (!mpCoreProtocol)
    {
        return false;
    }

    bool res = false;
    Profile::Uid complement;
    for (CoreProfileInfoList::iterator it = mProfiles.begin();
            mProfiles.end() != it; ++it)
    {
        if (id == it->uid())
        {
            res = true;
            it->disableByClient();
            complement = it->complement();
        }
    }
    if (!res)
    {
        return disableApiUidByClient(id);
    }
    else
    {
        if (mpPim && mEventFactory)
        {
            mEventFactory->coreInternalUnloadInstances(mpPim,id);
        }
        mpCoreProtocol->disableByClient(complement);
    }
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

    bool res = false;
    Profile::Uid complement;
    for (CoreProfileInfoList::iterator it = mProfiles.begin();
            mProfiles.end() != it; ++it)
    {
        if (id == it->uid())
        {
            res = true;
            it->enableByClient();
            complement = it->complement();
        }
    }
    if (!res)
    {
        return enableApiUidByClient(id);
    }
    else
    {
        mpCoreProtocol->enableByClient(complement);
    }
    return true;
}

bool PmpCore::disableApiUidByClient(Profile::ApiUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    bool res = false;
    std::list<Profile::Uid> lst;
    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        if (it->api() == id)
        {
            res = true;
            it->disableByClient();
            if (mpPim)
            {
                mpPim->unloadInstances(it->uid());
            }
            lst.push_back(it->complement());
        }
    }

    for (std::list<Profile::ApiUid>::iterator it = lst.begin(); lst.end() != it; ++it)
    {
        mpCoreProtocol->disableByClient(*it);
    }
    return res;
}

bool PmpCore::enableApiUidByClient(Profile::ApiUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    bool res = false;
    std::list<Profile::Uid> lst;
    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        if (it->api() == id)
        {
            res = true;
            it->enableByClient();
            lst.push_back(it->complement());
        }
    }

    for (std::list<Profile::ApiUid>::iterator it = lst.begin(); lst.end() != it; ++it)
    {
        mpCoreProtocol->enableByClient(*it);
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

    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->enableByClient();
    }
    mpCoreProtocol->enableByClientAll();
}

void PmpCore::getAvailableProfileComplements()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    tComplementsList complementsList;

    for (CoreProfileInfoList::iterator it = mProfiles.begin();
            mProfiles.end() != it; ++it)
    {
        complementsList.push_back(std::make_pair(it->uid(), it->version()));
    }
    mpCoreProtocol->getAvailableProfileComplementsResponse(complementsList);
}

bool PmpCore::onLock(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    bool res = false;
    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        if (it->uid() == id)
        {
            res = true;
            it->lock();
        }
    }
    return res;
}

bool PmpCore::onUnlock(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    bool res = false;
    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        if (it->uid() == id)
        {
            res = true;
            it->unlock();
        }
    }
    return res;
}

void PmpCore::onLockAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    for (CoreProfileInfoList::iterator it = mProfiles.begin();  mProfiles.end() != it; ++it)
    {
        it->lock();
    }
}

void PmpCore::onUnlockAll()
    {
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->unlock();
    }
}

bool PmpCore::onDisableByClient(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    bool res = false;
    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        if (it->uid() == id)
        {
            res = true;
            it->disableByClient();
        }
    }
    return res;
}

bool PmpCore::onEnableByClient(iviLink::BaseUid id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    bool res = false;
    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        if (it->uid() == id)
        {
            res = true;
            it->enableByClient();
        }
    }
    return res;
}

void PmpCore::onEnableByClientAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    for (CoreProfileInfoList::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->enableByClient();
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

    for (CoreProfileInfoList::const_iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        print += "\n\t***";
        print += "\n\tUID :" + it->uid().value();
        print += "\n\tAPI : " + it->api().value();
        print += "\n\tComplement : " + it->complement().value();
        print += it->available() ? "\n\tAvailable" : "\n\tNot available";
        print += it->enabled() ? "\n\tEnabled" : "\n\tDisabled";
        print += it->locked() ? "\n\tLocked" : "\n\tUnlocked";
        print += "\n\tLibrary : " + it->library();
    }
    print += "\n*****";
    LOG4CPLUS_INFO(msLogger, print);
}


bool PmpCore::getComplementary(Profile::Uid const& profUid,
        Profile::Uid& complProfUid) const
{

    for (CoreProfileInfoList::const_iterator it = mProfiles.begin();
            mProfiles.end() != it; ++it)
    {
        if (profUid == it->uid())
        {
            complProfUid = it->complement();
            return true;
        }
    }
    return false;
}

bool PmpCore::getPapiUid(iviLink::Profile::Uid const& profUid,
        iviLink::Profile::ApiUid& papiUid) const
{

    for (CoreProfileInfoList::const_iterator it = mProfiles.begin();
            mProfiles.end() != it; ++it)
    {
        if (profUid == it->uid())
        {
            papiUid = it->api();
            return true;
        }
    }
    return false;
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
