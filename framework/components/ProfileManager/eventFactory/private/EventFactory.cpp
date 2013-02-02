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


#include "EventFactory.hpp"
#include "MessageHandler.hpp"
#include "ProtocolEvents.hpp"
#include "InternalEvents.hpp"

#include "EventHandler.hpp"
#include "CSignalSemaphore.hpp"
#include "PmpComponentManager.hpp"
#include "PmpIpcProtocol.hpp"
#include "WaitingServiceManager.hpp"

namespace iviLink
{
namespace PMP
{

Logger EventFactory::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.EventFactory"));

::CSignalSemaphore * EventFactory::mSCSemaphore = 0;

EventFactory::EventFactory(Utils::EventHandler * handler, PmpComponentManager * manager,
        ProfileRepository * repository, PmpCore * core, PmpPim * pim,
        Ipc::PmpIpcProtocol * ipcProtocol, PmpInteractionProtocol * interProtocol
        , WaitingServiceManager * sessionManager )
    : mEventHandler(handler)
    , mHandler(new MessageHandler(handler,manager,repository,core,pim,ipcProtocol,
            sessionManager))
    , mManager(manager)
    , mRepository(repository)
    , mCore(core)
    , mPim(pim)
    , mIpcProtocol(ipcProtocol)
    , mInterProtocol(interProtocol)
    , mSessionManager(sessionManager)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

EventFactory::~EventFactory()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    MessageHandler * pHan = mHandler;
    delete pHan;
}

AppMan::IAmpHandler * EventFactory::getAmpHandler()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    return mHandler;
}

IPmpCoreForIpc * EventFactory::getIpcCoreHandler()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    return mHandler;
}
IPmpPimForIpc * EventFactory::getIpcPimHandler()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    return mHandler;
}
void EventFactory::exceptionProtocolChannelDeleted()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushException(new ExceptionProtocolChannelDeleted(mManager));
}
void EventFactory::exceptionProtocolConnectionLost()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushException(new ExceptionProtocolConnectionLost(mManager));
}

void EventFactory::coreProtocolGetAvailableProfileComplementsRequest()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new CoreProtocolGetAvailableProfileComplementsRequest(mCore));
}

void EventFactory::coreProtocolGetAvailableProfileComplementsResponse(
        const std::list<Profile::Uid> & complements)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new CoreProtocolGetAvailableProfileComplementsResponse(mCore, complements));
}

void EventFactory::coreProtocolReloadProfilesFromRepository()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new CoreProtocolReloadProfilesFromRepository(mCore));
}

void EventFactory::coreProtocolResetProfileState()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new CoreProtocolResetProfileState(mCore));
}

void EventFactory::coreProtocolLock(const BaseUid & id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new CoreProtocolLock(mCore, id));
}

void EventFactory::coreProtocolUnlock(const BaseUid & id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new CoreProtocolUnlock(mCore, id));
}

void EventFactory::coreProtocolLockAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new CoreProtocolLockAll(mCore));
}

void EventFactory::coreProtocolUnlockAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new CoreProtocolUnlockAll(mCore));
}

void EventFactory::coreProtocolDisableByClient(const BaseUid & id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new CoreProtocolDisableByClient(mCore, id));
}

void EventFactory::coreProtocolEnableByClient(const BaseUid & id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new CoreProtocolEnableByClient(mCore, id));
}

void EventFactory::coreProtocolEnableByClientAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new CoreProtocolEnableByClientAll(mCore));
}

void EventFactory::pimProtocolProfileDied(const Profile::IUid & piuid, const Service::Uid & sid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new PimProtocolProfileDied(mPim,piuid,sid));
}

void EventFactory::pimProtocolCreateProfile(const Profile::Uid & profileUid,
        const Profile::IUid & piuid, const Service::Uid & sid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new PimProtocolCreateProfile(mPim,profileUid, piuid, sid));
}

void EventFactory::pmpReady()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mManager->onReady();
}

void EventFactory::coreInternalUnloadInstances(IPmpPimToCore * core, const BaseUid & id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new InternalCoreUnloadInstances(core, id));
}

void EventFactory::pimProcessIncomingProfile(const BaseUid & appId, const Profile::Uid & profileUid,
        const Profile::IUid & profileIUid, const Service::SessionUid & sessionUid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new InternalPimProcessIncomingProfile(mPim,appId,profileUid,
            profileIUid, sessionUid));
}

void EventFactory::scShutDown()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mManager->signal();
}

void EventFactory::scUnlockAuthenticationProfile()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    if (!mSCSemaphore)
    {
        mSCSemaphore = new ::CSignalSemaphore();
    }
    mEventHandler->pushEvent(new InternalCoreUnlockAuthenticationProfile(mCore, mSCSemaphore));
//    mSCSemaphore->wait();
}

void EventFactory::scUnlockProfiles()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new InternalCoreUnlockProfiles(mCore));
}

void EventFactory::resetProfilesState()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mEventHandler->pushEvent(new InternalCoreResetProfilesState(mCore));
}

void EventFactory::coreIsReady()
{
    mEventHandler->pushEvent(new InternalCoreIsReady(mManager));
}

void EventFactory::negotiateRole(UInt32 number)
{
    mEventHandler->pushEvent(new ProtocolEventNegotiateRole(mManager, number));
}

void EventFactory::startWaitingService(bool master)
{
    // if master - start service; if slave - send to master list of services
    class StartWaitingService : public Utils::Event
    {
        WaitingServiceManager * mSessionManager;
        bool mMaster;
    public:
        StartWaitingService(WaitingServiceManager * sessionManager, bool master)
            : mSessionManager(sessionManager)
            , mMaster(master)
        {
        }

        virtual void handleEvent()
        {
            mSessionManager->startWaitingServiceRequest(mMaster);
        }
    };

    mEventHandler->pushEvent(new StartWaitingService(mSessionManager, master));
}

void EventFactory::receiveRemoteWaitingServiceList(const std::list<Service::Uid> & services)
{
    class ReceiveRemoteWaitingServiceList : public Utils::Event
    {
        WaitingServiceManager * mSessionManager;
        PmpComponentManager * mManager;
        std::list<Service::Uid> mServices;

    public:
        ReceiveRemoteWaitingServiceList(WaitingServiceManager * sessionManager,
                PmpComponentManager * manager, const std::list<Service::Uid> services)
            : mSessionManager(sessionManager)
            , mManager(manager)
            , mServices(services)
        {
        }

        virtual void handleEvent()
        {
            mSessionManager->setRemoteWaitingServices(mServices);
            mManager->checkWaitingServices();
        }
    };

    mEventHandler->pushEvent(new ReceiveRemoteWaitingServiceList(mSessionManager,
            mManager, services));
}

void EventFactory::authenticationDone()
{
    class AuthenticationDoneEvent : public Utils::Event
    {
        WaitingServiceManager * mWaitingServiceManager;

    public:
        AuthenticationDoneEvent(WaitingServiceManager * waitingServiceManager)
            : mWaitingServiceManager(waitingServiceManager)
        {
        }

        virtual void handleEvent()
        {
            mWaitingServiceManager->authenticationDone();
        }
    };

    mEventHandler->pushEvent(new AuthenticationDoneEvent(mSessionManager));
}

} /* namespace PMP */
} /* namespace iviLink */
