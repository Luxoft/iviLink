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


#ifndef IVILINK_PMP_EVENTFACTORY_HPP_
#define IVILINK_PMP_EVENTFACTORY_HPP_

#include <list>

#include "UID.hpp"
#include "Logger.hpp"

class CSignalSemaphore;

namespace iviLink
{

namespace Utils
{
class EventHandler;
}

namespace AppMan
{
class IAmpHandler;
}

namespace PMP
{

class MessageHandler;
class IPmpCoreForIpc;
class IPmpPimForIpc;

class PmpComponentManager;
class PmpCore;
class PmpPim;
class ProfileRepository;
class IPmpPimToCore;
class PmpInteractionProtocol;
class WaitingServiceManager;

namespace Ipc
{
class PmpIpcProtocol;
}

class EventFactory
{
public:
    EventFactory(Utils::EventHandler * handler, PmpComponentManager * manager,
            ProfileRepository * repository, PmpCore * core, PmpPim * pim,
            Ipc::PmpIpcProtocol * ipcProtocol, PmpInteractionProtocol * interProtocol,
            WaitingServiceManager * sessionManager);
    virtual ~EventFactory();

    AppMan::IAmpHandler * getAmpHandler();
    IPmpCoreForIpc * getIpcCoreHandler();
    IPmpPimForIpc * getIpcPimHandler();

    /// Exceptions from Interaction protocol
    void exceptionProtocolChannelDeleted();
    void exceptionProtocolConnectionLost();

    /// Events from Interaction Protocol for core
    void coreProtocolGetAvailableProfileComplementsRequest();
    void coreProtocolGetAvailableProfileComplementsResponse(
            const std::list<Profile::Uid> & complements);
    void coreProtocolReloadProfilesFromRepository();
    void coreProtocolResetProfileState();
    void coreProtocolLock(const BaseUid & id);
    void coreProtocolUnlock(const BaseUid & id);
    void coreProtocolLockAll();
    void coreProtocolUnlockAll();
    void coreProtocolDisableByClient(const BaseUid & id);
    void coreProtocolEnableByClient(const BaseUid & id);
    void coreProtocolEnableByClientAll();

    /// Events from Interaction Protocol for PIM
    void pimProtocolProfileDied(const Profile::IUid & piuid, const Service::Uid & sid);
    void pimProtocolCreateProfile(const Profile::Uid & profileUid,
            const Profile::IUid & piuid, const Service::Uid & sid);

    /// Event for Interaction Protocol
    void pmpReady();

    ///Events for internal entities
    void coreInternalUnloadInstances(IPmpPimToCore * core, const BaseUid & id);
    void pimProcessIncomingProfile(const BaseUid & appId, const Profile::Uid & profileUid,
            const Profile::IUid & profileIUid, const Service::SessionUid & sessionUid);

    /// Events from system controller
    void scShutDown();
    void scUnlockAuthenticationProfile();
    void scUnlockProfiles();

    /// Events for component manager
    void resetProfilesState();

    ///Events from PMP Core
    void coreIsReady();

    void negotiateRole(UInt32 number);
    void startWaitingService(bool master);
    void receiveRemoteWaitingServiceList(const std::list<Service::Uid> & services);
    void authenticationDone();

private:
    Utils::EventHandler * mEventHandler;
    MessageHandler * mHandler;
    PmpComponentManager * mManager;
    ProfileRepository * mRepository;
    PmpCore * mCore;
    PmpPim * mPim;
    Ipc::PmpIpcProtocol * mIpcProtocol;
    PmpInteractionProtocol * mInterProtocol;
    WaitingServiceManager * mSessionManager;

    static CSignalSemaphore * mSCSemaphore;
    static Logger msLogger; ///< object of logger
};

} /* namespace PMP */
} /* namespace iviLink */
#endif /* IVILINK_PMP_EVENTFACTORY_HPP_ */
