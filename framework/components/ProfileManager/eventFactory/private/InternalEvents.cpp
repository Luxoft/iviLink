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


#include "InternalEvents.hpp"
#include "PmpPim.hpp"
#include "PmpCore.hpp"
#include "PmpComponentManager.hpp"
#include "CSignalSemaphore.hpp"

#include <cassert>

namespace iviLink
{
namespace PMP
{

InternalCoreUnloadInstances::InternalCoreUnloadInstances(IPmpPimToCore * core, BaseUid id)
    : mCore(core)
    , mId(id)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
    assert(mCore);
}

void InternalCoreUnloadInstances::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->unloadInstances(mId);
}

InternalPimProcessIncomingProfile::InternalPimProcessIncomingProfile(
        PmpPim * pim, const BaseUid & appId, const Profile::Uid & profileUid,
            const Profile::IUid & profileIUid, const Service::SessionUid & sessionUid)
    : mPim(pim)
    , mAppId(appId)
    , mProfileUid(profileUid)
    , mProfileIUid(profileIUid)
    , mServiceUid(sessionUid)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}


void InternalPimProcessIncomingProfile::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mPim->processIncomingProfile(mAppId, mProfileUid, mProfileIUid, mServiceUid);
}

InternalCoreUnlockAuthenticationProfile::InternalCoreUnlockAuthenticationProfile(PmpCore * core,
        CSignalSemaphore * sem)
    : mCore(core)
    , mSem(sem)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void InternalCoreUnlockAuthenticationProfile::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->unlockAuthProfile();
    if (mSem)
    {
        mSem->signal();
    }
}

InternalCoreUnlockProfiles::InternalCoreUnlockProfiles(PmpCore * core)
    : mCore(core)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void InternalCoreUnlockProfiles::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->unlockBySysCtrlAll();
}

InternalCoreResetProfilesState::InternalCoreResetProfilesState(PmpCore * core)
    : mCore(core)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void InternalCoreResetProfilesState::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->resetProfilesState();
}

InternalCoreIsReady::InternalCoreIsReady(PmpComponentManager * manager)
    : mManager(manager)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void InternalCoreIsReady::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mManager->endInitialization();
}

} /* namespace PMP */
} /* namespace iviLink */
