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


#include "MessageHandler.hpp"
#include "AmpRequests.hpp"
#include "IpcEvents.hpp"
#include "CIpc.hpp"
#include "EventHandler.hpp"
#include "PmpComponentManager.hpp"
#include "WaitingServiceManager.hpp"

namespace iviLink
{
namespace PMP
{

Logger MessageHandler::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.EventFactory"));

MessageHandler::MessageHandler(Utils::EventHandler * handler, PmpComponentManager * manager,
        ProfileRepository * repository, PmpCore * core, PmpPim * pim,
        Ipc::PmpIpcProtocol * ipcProtocol, WaitingServiceManager * sessionManager)
    : mHandler(handler)
    , mManager(manager)
    , mRepository(repository)
    , mCore(core)
    , mPim(pim)
    , mIpcProtocol(ipcProtocol)
    , mSessionManager(sessionManager)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

MessageHandler::~MessageHandler()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void MessageHandler::onAmpAppRequestError(iviLink::Service::SessionUid session)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new AmpAppRequestError(mPim, session));
}

void MessageHandler::onAmpConnectionLost()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushException(new AmpConnectionLost(mManager));
}

void MessageHandler::onAmpConnection()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mManager->onAmpConnection();
}

void MessageHandler::disableByClient(const iviLink::BaseUid & id,
         const iviLink::Ipc::DirectionID & dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new CoreIpcDisableByClient(mCore, id, dirId));
}

void MessageHandler::enableByClient(const iviLink::BaseUid & id,
         const iviLink::Ipc::DirectionID & dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new CoreIpcEnableByClient(mCore, id, dirId));
}

void MessageHandler::enableByClientAll(const iviLink::Ipc::DirectionID & dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new CoreIpcEnableByClientAll(mCore, dirId));
}

void MessageHandler::getManifest(const iviLink::BaseUid & uid,
         const iviLink::Ipc::DirectionID & dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new CoreIpcGetManifest(mCore, uid, dirId, mIpcProtocol));
}

void MessageHandler::getProfileLibPath(const iviLink::Profile::Uid & uid,
         const iviLink::Ipc::DirectionID & dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new CoreIpcGetProfileLibPath(mCore, uid, dirId, mIpcProtocol));
}

void MessageHandler::findProfiles(const iviLink::BaseUid & id,
       const std::map<std::string, std::string> & profileParameters,
       bool enabledProfiles, const iviLink::Ipc::DirectionID & dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new CoreIpcFindProfiles(mCore, id, profileParameters, enabledProfiles,
            dirId, mIpcProtocol));
}

void MessageHandler::generatePIUID(iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new PimIpcGeneratePIUID(mPim,dirId, mIpcProtocol));
}

void MessageHandler::releasePIUID(iviLink::Profile::IUid const& piuid,
    iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new PimIpcReleasePIUID(mPim, piuid, dirId));
}

void MessageHandler::createProfile(iviLink::BaseUid const& profileUid,
    iviLink::Profile::IUid const& piuid,
    iviLink::Service::Uid const& sid,
    iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new PimIpcCreateProfile(mPim, profileUid, piuid, sid, dirId));
}

void MessageHandler::profileDied(iviLink::Profile::IUid const& piuid,
    iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new PimIpcProfileDied(mPim, piuid, dirId));
}

void MessageHandler::readyToServe(iviLink::Service::SessionUid const& sesUid,
    iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new PimIpcReadyToServe(mPim, sesUid, dirId));
}

void MessageHandler::onConnection(iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new PimIpcOnConnection(mPim, dirId));
}

void MessageHandler::onConnectionLost(iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mHandler->pushEvent(new PimIpcOnConnectionLost(mPim, dirId));
}

void MessageHandler::onWaitingServicesRequest(const std::list<Service::Uid> & services)
{
    assert(mSessionManager);
    assert(mManager);
    class OnWaitingServicesRequest : public Utils::Event
    {
        std::list<Service::Uid> mServices;
        PmpComponentManager * mComponentManager;
        WaitingServiceManager * mSessionManager;

    public:
        OnWaitingServicesRequest(const std::list<Service::Uid> & services,
                PmpComponentManager * componentManager, WaitingServiceManager * sessionManager)
            : mServices (services)
            , mSessionManager(sessionManager)
            , mComponentManager(componentManager)
        {
        }

        virtual void handleEvent()
        {
            mSessionManager->setLocalWaitingServices(mServices);
            mComponentManager->checkWaitingServices();
        }
    };
    mHandler->pushEvent(new OnWaitingServicesRequest(services, mManager, mSessionManager));
}

} /* namespace PMP */
} /* namespace iviLink */
