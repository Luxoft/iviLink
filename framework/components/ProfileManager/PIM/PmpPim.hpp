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


#ifndef CPMPPIM_HPP_
#define CPMPPIM_HPP_

#include <map>
#include <set>
#include <tr1/tuple>

#include "Logger.hpp"
#include "IPmpPimToCore.hpp"
#include "IPmpCoreToPim.hpp"
#include "EventFactory.hpp"
#include "ipc_common.hpp"

class PMP_PIM_Test;

namespace iviLink
{

namespace AppMan
{
class IAmpRequest;
}

namespace PMP
{

namespace Ipc
{
class PmpIpcProtocol;
}

class EventFactory;
class PmpPimProtocol;

namespace PIM
{
class ClientAppRegistry;
}  // namespace PIM

class IPmIpcToPim;

class PmpPim : public IPmpPimToCore
{
public:
   // from IPMPPIMToIpc

    virtual void generatePIUID(iviLink::Profile::IUid& piuid,
            iviLink::Ipc::DirectionID const& dirId);

    virtual void releasePIUID(iviLink::Profile::IUid const& piuid,
            iviLink::Ipc::DirectionID const& dirId);

    virtual void createProfile(iviLink::BaseUid const& profileUid,
            iviLink::Profile::IUid const& piuid,
            iviLink::Service::Uid const& sid,
            iviLink::Ipc::DirectionID const& dirId);

    virtual void profileDied(iviLink::Profile::IUid const& piuid,
            iviLink::Ipc::DirectionID const&/* dirId*/);

    virtual void readyToServe(iviLink::Service::SessionUid const& sesUid,
            iviLink::Ipc::DirectionID const& dirId);

    virtual void onConnection(iviLink::Ipc::DirectionID const& dirId);

    virtual void onConnectionLost(iviLink::Ipc::DirectionID const& dirId);

    // from IPmpPimClb

    virtual bool onCreateProfile(iviLink::Profile::Uid const& profileUid,
            iviLink::Profile::IUid const& piuid,
            iviLink::Service::Uid const& sid);

    virtual void onProfileDied(iviLink::Profile::IUid const& piuid,
            iviLink::Service::Uid const& sid);

    // from IPmpPimToCore

    virtual bool hasInstance(Profile::Uid const& id);

    virtual void unloadInstances(Profile::Uid const& id);

    // from AppMan::IPmpHandler

    virtual void appRequestError(iviLink::Service::SessionUid session);

    virtual void appManConnectionLost();

    PmpPim();
    void setPimProtocol(PmpPimProtocol* pProtocol);
    void setEventFactory(EventFactory * factory);
    void setIpc(Ipc::PmpIpcProtocol* pIpc);
    void setCore(IPmpCoreToPim* pCore);
    void setAmp(AppMan::IAmpRequest *amp);

    virtual ~PmpPim();

    PmpPim(PmpPim const&);
    PmpPim& operator=(PmpPim const&);

    void saveIncomingProfileForProcessing(iviLink::BaseUid const& appId,
            iviLink::Profile::Uid const& profileUid,
            iviLink::Profile::IUid const& piuid,
            iviLink::Service::Uid const& sid);

    void processIncomingProfile(iviLink::BaseUid const& appId,
            iviLink::Profile::Uid const& profileUid,
            iviLink::Profile::IUid const& piuid,
            iviLink::Service::Uid const& sid);

    private:

    PmpPimProtocol * mProtocol;
    Ipc::PmpIpcProtocol * mIpc;
    IPmpCoreToPim * mCore;

    typedef std::set<Profile::IUid> IncomingProfiles;
    IncomingProfiles mIncomingProfiles;

    PIM::ClientAppRegistry * mRegistry;

    typedef std::tr1::tuple<Profile::Uid, Profile::IUid, Service::Uid> PendingService;
    typedef std::map<iviLink::Service::SessionUid, PendingService> PendingServiceMap;
    PendingServiceMap mPendingServiceMap;

    friend class ::PMP_PIM_Test;

    friend class PimJobStarter;

    EventFactory * mEventFactory;
    AppMan::IAmpRequest * mAmpRequest;

    static Logger msLogger;
};


}  // namespace ProfileManager
}  // namespace AXIS


#endif /* CPMPPIM_HPP_ */
