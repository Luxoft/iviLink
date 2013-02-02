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


#ifndef PMPCOMPONENTMANAGER_HPP_
#define PMPCOMPONENTMANAGER_HPP_

#include <sys/types.h>

#include "Logger.hpp"
#include "Types.hpp"

class CSignalSemaphore;

namespace iviLink
{

namespace conf
{
class Configurator;
}  // namespace conf

namespace AppMan
{
namespace Ipc
{
class AmpClient;
}
}  //namespace AppMan

namespace Utils
{
class EventHandler;
}

namespace PMP
{
namespace Ipc
{
class PmpIpcProtocol;
}  // namespace Ipc

class PmpCore;
class IPmpCoreToSysCtrl;
class PmpPim;
class EventFactory;
class ProfileRepository;
class ServiceRepository;
class PmpInteractionProtocol;
class SystemControllerMsg;
class WaitingServiceManager;

enum Role
{
    PMP_ROLE_NONE,
    PMP_ROLE_MASTER,
    PMP_ROLE_SLAVE
};

/**
* Class is used for managing PMP components. It's responsible for initializing and de-initializing
* of PMP and connecting submodules with each other. It's singleton class.
*/
class PmpComponentManager
{
public:

    void onChannelDeleted();
    void onConnectionLost();
    void onReady();
    void onAmpConnection();
    void onAmpConnectionLost();

    /**
    * Inits PMP components
    */
    void initPmp();

    void endInitialization();

    /**
    * Uninits PMP components
    */
    void uninitPmp();

    void wait();
    void signal();
    /**
    * Constructor
    */
    PmpComponentManager(conf::Configurator * pConfig = 0);

    /**
    * Destructor
    */
    ~PmpComponentManager();

    Role getRole() const;
    void negotiateRole();
    void onNegotiateRole(UInt32 number);
    void checkWaitingServices();

private:
    void genRandomNumber();

    iviLink::conf::Configurator * mConfig;      ///< Instance of configurator
    ProfileRepository * mProfileRepository;     ///< Pointer to instance of P Repository
    ServiceRepository * mServiceRepository;     ///< Pointer to instance of Service Repository
    PmpCore * mCore;                           ///< Pointer to Core instance
    PmpPim * mPim;                             ///< Pointer to PIM instance
    PmpInteractionProtocol * mInteractionProtocol;  ///< Instance of interaction protocol
    iviLink::PMP::Ipc::PmpIpcProtocol * mIpcProtocol; ///< Pointer to IPC Protocol instance
    AppMan::Ipc::AmpClient * mAmpClient;        ///< Instance of AMP IPC client
    Utils::EventHandler * mEventHandler;        ///< Instance of PMP Event Handler
    EventFactory * mEventFactory;   ///< Pointer to instance of Event factory for Event Handler
    SystemControllerMsg * mSCMsg;   ///< Instance of SC IPC protocol
    CSignalSemaphore * mSemaphore;  ///< Instance of signal semaphore
    bool mInitializationDone;       ///< Flag true if initialization of PMP's been finished
    Logger mLogger;                 ///< object of logger
    Role mRole;                     ///< role of PMP in interaction
    UInt32 mRoleNubmer;
    WaitingServiceManager * mSessionManager;
    UInt32 mOtherRoleNumber;		///< role number of another side PMP if we received it before negotiateRole
};

}  // namespace PMP
}  // namespace iviLink

#endif /* PMPCOMPONENTMANAGER_HPP_ */
