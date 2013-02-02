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

#include "ProtocolEvents.hpp"

#include "PmpCore.hpp"
#include "PmpPim.hpp"
#include "PmpInteractionProtocol.hpp"
#include "PmpComponentManager.hpp"

namespace iviLink
{
namespace PMP
{

CoreProtocolGetAvailableProfileComplementsRequest::
    CoreProtocolGetAvailableProfileComplementsRequest(PmpCore * core)
    : mCore(core)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreProtocolGetAvailableProfileComplementsRequest::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->getAvailableProfileComplements();
}

CoreProtocolGetAvailableProfileComplementsResponse::
    CoreProtocolGetAvailableProfileComplementsResponse(PmpCore * core,
        const std::list<Profile::Uid> & complements)
    : mCore(core)
    , mComplements(complements)
,    mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreProtocolGetAvailableProfileComplementsResponse::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->getAvailbleProfileComplementsResponse(mComplements);
}

CoreProtocolReloadProfilesFromRepository::CoreProtocolReloadProfilesFromRepository(PmpCore * core)
    : mCore(core)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreProtocolReloadProfilesFromRepository::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->onReloadProfilesFromRepository();
}

CoreProtocolResetProfileState::CoreProtocolResetProfileState(PmpCore * core)
    : mCore(core)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreProtocolResetProfileState::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->onResetProfilesState();
}

CoreProtocolLock::CoreProtocolLock(PmpCore * core, const BaseUid & id)
    : mCore(core)
    , mId(id)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreProtocolLock::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->onLock(mId);
}

CoreProtocolUnlock::CoreProtocolUnlock(PmpCore * core, const BaseUid & id)
    : mCore(core)
    , mId(id)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreProtocolUnlock::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->onUnlock(mId);
}

CoreProtocolLockAll::CoreProtocolLockAll(PmpCore * core)
    : mCore(core)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreProtocolLockAll::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->onLockAll();
}

CoreProtocolUnlockAll::CoreProtocolUnlockAll(PmpCore * core)
    : mCore(core)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreProtocolUnlockAll::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->onUnlockAll();
}

CoreProtocolDisableByClient::CoreProtocolDisableByClient(PmpCore * core, const BaseUid & id)
    : mCore(core)
    , mId(id)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreProtocolDisableByClient::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->onDisableByClient(mId);
}

CoreProtocolEnableByClient::CoreProtocolEnableByClient(PmpCore * core, const BaseUid & id)
    : mCore(core)
    , mId(id)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreProtocolEnableByClient::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->onEnableByClient(mId);
}

CoreProtocolEnableByClientAll::CoreProtocolEnableByClientAll(PmpCore * core)
    : mCore(core)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreProtocolEnableByClientAll::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->onEnableByClientAll();
}

PimProtocolProfileDied::PimProtocolProfileDied(PmpPim * pim,
        const Profile::IUid & piuid, const Service::Uid & sid)
    : mPim(pim)
    , mPIUid(piuid)
    , mSUid(sid)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void PimProtocolProfileDied::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mPim->onProfileDied(mPIUid, mSUid);
}

PimProtocolCreateProfile::PimProtocolCreateProfile(PmpPim * pim,
        const Profile::Uid & profileUid, const Profile::IUid & piuid, const Service::Uid & sid)
    : mPim(pim)
    , mProfileUid(profileUid)
    , mProfileIUid(piuid)
    , mSUid(sid)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

int PimProtocolCreateProfile::getTimeout()
{
	return 300;
}

void PimProtocolCreateProfile::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
     if (!mPim->onCreateProfile(mProfileUid, mProfileIUid, mSUid))
     {
         pushTimeoutEvent(new PimProtocolCreateProfile(mPim, mProfileUid, mProfileIUid, mSUid));
     }
}

ExceptionProtocolChannelDeleted::ExceptionProtocolChannelDeleted(PmpComponentManager * manager)
    : mManager(manager)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void ExceptionProtocolChannelDeleted::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mManager->onChannelDeleted();
}

ExceptionProtocolConnectionLost::ExceptionProtocolConnectionLost(PmpComponentManager * manager)
    : mManager(manager)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void ExceptionProtocolConnectionLost::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mManager->onConnectionLost();
}

ProtocolEventNegotiateRole::ProtocolEventNegotiateRole(PmpComponentManager * manager, UInt32 number)
    : mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
    , mManager(manager)
    , mNumber(number)
{
}

void ProtocolEventNegotiateRole::handleEvent()
{
    mManager->onNegotiateRole(mNumber);
}

} /* namespace PMP */
} /* namespace iviLink */
