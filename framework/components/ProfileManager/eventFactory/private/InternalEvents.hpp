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


#ifndef INTERNALEVENTS_HPP_
#define INTERNALEVENTS_HPP_

#include "EventHandler.hpp"
#include "UID.hpp"
#include "IPmpPimToCore.hpp"
#include "Logger.hpp"

class CSignalSemaphore;

namespace iviLink
{
namespace PMP
{

namespace Ipc
{
class CIpcProtocol;
}

class PmpPim;
class PmpCore;
class PmpComponentManager;

class InternalCoreUnloadInstances : public Utils::Event
{
public:
    InternalCoreUnloadInstances(IPmpPimToCore * core, BaseUid id);

private:
    virtual void handleEvent();

    IPmpPimToCore * mCore;
    iviLink::BaseUid mId;
    Logger mLogger;
};

class InternalPimProcessIncomingProfile : public Utils::Event
{
public:
    InternalPimProcessIncomingProfile(PmpPim * pim, const BaseUid & appId, const Profile::Uid & profileUid,
            const Profile::IUid & profileIUid, const Service::SessionUid & sessionUid);

private:
    virtual void handleEvent();

    PmpPim * mPim;
    BaseUid mAppId;
    iviLink::Profile::Uid mProfileUid;
    iviLink::Profile::IUid mProfileIUid;
    iviLink::Service::Uid mServiceUid;
    Logger mLogger;
};

class InternalCoreUnlockAuthenticationProfile : public Utils::Event
{
public:
    InternalCoreUnlockAuthenticationProfile(PmpCore * core, CSignalSemaphore * sem);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    CSignalSemaphore * mSem;
    Logger mLogger;
};

class InternalCoreUnlockProfiles : public Utils::Event
{
public:
    InternalCoreUnlockProfiles(PmpCore * core);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    Logger mLogger;
};

class InternalCoreResetProfilesState : public Utils::Event
{
public:
    InternalCoreResetProfilesState(PmpCore * core);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    Logger mLogger;
};

class InternalCoreIsReady : public Utils::Event
{
public:
    InternalCoreIsReady(PmpComponentManager * manager);

private:
    virtual void handleEvent();

    PmpComponentManager * mManager;
    Logger mLogger;
};
} /* namespace PMP */
} /* namespace iviLink */
#endif /* INTERNALEVENTS_HPP_ */
