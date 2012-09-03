/* 
 * 
 * iviLINK SDK, version 1.1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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












#ifndef CPMPPIM_HPP_
#define CPMPPIM_HPP_

#include <map>
#include <set>
#include <tr1/tuple>

#include "utils/misc/Logger.hpp"
#include "utils/threads/CMutex.hpp"
#include "framework/components/ProfileManager/PMP/interaction/IPmpPimClb.hpp"
#include "framework/components/ProfileManager/PMP/PIM/IPmpPimToCore.hpp"
#include "framework/components/ProfileManager/PMP/core/IPmpCoreToPim.hpp"
#include "framework/libraries/AppMan/Pmp/IPmpHandler.hpp"
#include "IPMPPIMToIpc.hpp"

class CThreadPool;

class CPMP_PIM_Test;

namespace iviLink {
namespace PMP {

namespace PIM {
class CClientAppRegistry;
}  // namespace PIM

class IPMPIpcToPIM;
class IPmpPimProtocol;

class CPmpPim : public IPMPPIMToIpc, public IPmpPimClb, public IPmpPimToCore,
   public iviLink::AppMan::IPmpHandler
{
public:
   // from IPMPPIMToIpc

   virtual void generatePIUID(iviLink::Profile::IUid& piuid,
      iviLink::Ipc::DirectionID const& dirId);

   virtual void releasePIUID(iviLink::Profile::IUid const& piuid,
      iviLink::Ipc::DirectionID const& dirId);

   virtual void createProfile(iviLink::CUid const& profileUid,
      iviLink::Profile::IUid const& piuid,
      iviLink::Service::Uid const& sid,
      iviLink::Ipc::DirectionID const& dirId);

   virtual void profileDied(iviLink::Profile::IUid const& piuid,
      iviLink::Ipc::DirectionID const&/* dirId*/);

   virtual void readyToServe(iviLink::Service::SessionUid const& sesUid,
      iviLink::Ipc::DirectionID const& dirId);

   virtual void onConnection(iviLink::Ipc::DirectionID const& dirId);

   virtual void onConnectionLost(iviLink::Ipc::DirectionID const& dirId);

public:
   // from IPmpPimClb

   virtual void onCreateProfile(iviLink::Profile::Uid const& profileUid,
         iviLink::Profile::IUid const& piuid,
         iviLink::Service::Uid const& sid);

   virtual void onProfileDied(iviLink::Profile::IUid const& piuid,
            iviLink::Service::Uid const& sid);

public:
   // from IPmpPimToCore

   virtual bool hasInstance(Profile::Uid const& id);

   virtual void unloadInstances(Profile::Uid const& id);

public:
   // from AppMan::IPmpHandler

   virtual void appRequestError(iviLink::Service::SessionUid session);

   virtual void appManConnectionLost();

public:

   CPmpPim(IPmpPimProtocol* pProtocol);
   void setIpc(IPMPIpcToPIM* pIpc);
   void setCore(IPmpCoreToPim* pCore);

   virtual ~CPmpPim();

private:

   CPmpPim(CPmpPim const&);
   CPmpPim& operator=(CPmpPim const&);

   void saveIncomingProfileForProcessing(iviLink::CUid const& appId,
      iviLink::Profile::Uid const& profileUid,
      iviLink::Profile::IUid const& piuid,
      iviLink::Service::Uid const& sid);

   void processIncomingProfile(iviLink::CUid const& appId,
      iviLink::Profile::Uid const& profileUid,
      iviLink::Profile::IUid const& piuid,
      iviLink::Service::Uid const& sid);

private:

   IPmpPimProtocol* mpProtocol;
   IPMPIpcToPIM* mpIpc;
   IPmpCoreToPim* mpCore;

   typedef std::set<Profile::IUid> tIncomingProfiles;
   tIncomingProfiles mIncomingProfiles;

   PIM::CClientAppRegistry* mpRegistry;

   typedef std::tr1::tuple<Profile::Uid, Profile::IUid, Service::Uid> tPendingService;
   typedef std::map<iviLink::Service::SessionUid, tPendingService> tPendingServiceMap;
   tPendingServiceMap mPendingServiceMap;
   mutable CMutex mPendingServiceMapMutex;

   friend class ::CPMP_PIM_Test;

   friend class CPIMJobStarter;

   CThreadPool* mpThreadPool;

   static Logger msLogger;
};


}  // namespace ProfileManager
}  // namespace AXIS



#endif /* CPMPPIM_HPP_ */
