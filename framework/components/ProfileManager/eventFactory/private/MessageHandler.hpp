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


#ifndef MESSAGEHANDLER_HPP_
#define MESSAGEHANDLER_HPP_

#include "IAmpHandler.hpp"
#include "IPmpCoreForIpc.hpp"
#include "IPmpPimForIpc.hpp"
#include "Logger.hpp"


namespace iviLink
{

namespace Utils
{

class EventHandler;

}

namespace PMP
{

class PmpComponentManager;
class PmpCore;
class PmpPim;
class ProfileRepository;
class WaitingServiceManager;

namespace Ipc
{
class PmpIpcProtocol;
}

class MessageHandler : public AppMan::IAmpHandler
                      , public IPmpCoreForIpc
                      , public IPmpPimForIpc
{
public:
    MessageHandler(Utils::EventHandler * handler, PmpComponentManager * manager,
            ProfileRepository * repository, PmpCore * core, PmpPim * pim,
            Ipc::PmpIpcProtocol * ipcProtocol, WaitingServiceManager * sessionManager);
    virtual ~MessageHandler();

private:
    /// AMP requests
    virtual void onAmpAppRequestError(iviLink::Service::SessionUid session);
    virtual void onAmpConnectionLost();
    virtual void onAmpConnection();
    virtual void onWaitingServicesRequest(const std::list<Service::Uid> & services);

    ////////// IPC
    ///Core IPC
     virtual void disableByClient(const iviLink::BaseUid & id,
             const iviLink::Ipc::DirectionID & dirId);

     virtual void enableByClient(const iviLink::BaseUid & id,
             const iviLink::Ipc::DirectionID & dirId);

     virtual void enableByClientAll(const iviLink::Ipc::DirectionID & dirId);

     virtual void getManifest(const iviLink::BaseUid & uid,
             const iviLink::Ipc::DirectionID & dirId);

     virtual void getProfileLibPath(const iviLink::Profile::Uid & uid,
             const iviLink::Ipc::DirectionID & dirId);

     virtual void findProfiles(const iviLink::BaseUid & id,
           const std::map<std::string, std::string> & profileParameters,
           bool enabledProfiles, const iviLink::Ipc::DirectionID & dirId);

    ///PIM IPC
     virtual void generatePIUID(iviLink::Ipc::DirectionID const& dirId);

     virtual void releasePIUID(iviLink::Profile::IUid const& piuid,
        iviLink::Ipc::DirectionID const& dirId);

     virtual void createProfile(iviLink::BaseUid const& profileUid,
        iviLink::Profile::IUid const& piuid,
        iviLink::Service::Uid const& sid,
        iviLink::Ipc::DirectionID const& dirId);

     virtual void profileDied(iviLink::Profile::IUid const& piuid,
        iviLink::Ipc::DirectionID const& dirId);

     virtual void readyToServe(iviLink::Service::SessionUid const& sesUid,
        iviLink::Ipc::DirectionID const& dirId);

     virtual void onConnection(iviLink::Ipc::DirectionID const& dirId);

     virtual void onConnectionLost(iviLink::Ipc::DirectionID const& dirId);

     Utils::EventHandler * mHandler;
     PmpComponentManager * mManager;
     ProfileRepository * mRepository;
     PmpCore * mCore;
     PmpPim * mPim;
     Ipc::PmpIpcProtocol * mIpcProtocol;
     WaitingServiceManager * mSessionManager;

     static Logger msLogger; ///< object of logger
};

} /* namespace PMP */
} /* namespace iviLink */
#endif /* MESSAGEHANDLER_HPP_ */
