/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */






#ifndef CPMPPIM_HPP_
#define CPMPPIM_HPP_

#include <tr1/tuple>

#include "utils/misc/include/Logger.hpp"
#include "CClientAppRegistry.hpp"
#include "IPMPPIMToIpc.hpp"
#include "framework/components/ProfileManager/PMP/interaction/include/IPmpPimClb.hpp"
#include "framework/components/ProfileManager/PMP/PIM/include/IPmpPimToCore.hpp"
#include "framework/components/ProfileManager/PMP/include/IPmpCoreToPim.hpp"
#include "framework/libraries/AppMan/Pmp/include/IPmpHandler.hpp"


class CThreadPool;

class CPMP_PIM_Test;

namespace AXIS {
namespace PMP {

class IPMPIpcToPIM;
class IPmpPimProtocol;

class CPmpPim : public IPMPPIMToIpc, public IPmpPimClb, public IPmpPimToCore,
   public AXIS::AppMan::IPmpHandler
{
public:
   // from IPMPPIMToIpc

   virtual void generatePIUID(AXIS::Profile::IUid& piuid,
      AXIS::Ipc::DirectionID const& dirId);

   virtual void releasePIUID(AXIS::Profile::IUid const& piuid,
      AXIS::Ipc::DirectionID const& dirId);

   virtual void createProfile(AXIS::CUid const& profileUid,
      AXIS::Profile::IUid const& piuid,
      AXIS::Service::Uid const& sid,
      AXIS::Ipc::DirectionID const& dirId);

   virtual void profileDied(AXIS::Profile::IUid const& piuid,
      AXIS::Ipc::DirectionID const& dirId);

   virtual void readyToServe(AXIS::Service::SessionUid const& sesUid,
      AXIS::Ipc::DirectionID const& dirId);

   virtual void onConnection(AXIS::Ipc::DirectionID const& dirId);

   virtual void onConnectionLost(AXIS::Ipc::DirectionID const& dirId);

public:
   // from IPmpPimClb

   virtual void onCreateProfile(AXIS::Profile::Uid const& profileUid,
         AXIS::Profile::IUid const& piuid,
         AXIS::Service::Uid const& sid);

   virtual void onProfileDied(AXIS::Profile::IUid const& piuid,
            AXIS::Service::Uid const& sid);

public:
   // from IPmpPimToCore

   virtual bool hasInstance(Profile::Uid const& id);

   virtual void unloadInstances(Profile::Uid const& id);

public:
   // from AppMan::IPmpHandler

   virtual void appRequestError(AXIS::Service::SessionUid session);

   virtual void appManConnectionLost();

public:

   CPmpPim(IPmpPimProtocol* pProtocol);
   void setIpc(IPMPIpcToPIM* pIpc);
   void setCore(IPmpCoreToPim* pCore);

   virtual ~CPmpPim();

private:

   CPmpPim(CPmpPim const&);
   CPmpPim& operator=(CPmpPim const&);

   void saveIncomingProfileForProcessing(AXIS::CUid const& appId,
      AXIS::Profile::Uid const& profileUid,
      AXIS::Profile::IUid const& piuid,
      AXIS::Service::Uid const& sid);

   void processIncomingProfile(AXIS::CUid const& appId,
      AXIS::Profile::Uid const& profileUid,
      AXIS::Profile::IUid const& piuid,
      AXIS::Service::Uid const& sid);

private:

   IPmpPimProtocol* mpProtocol;
   IPMPIpcToPIM* mpIpc;
   IPmpCoreToPim* mpCore;

   typedef std::set<Profile::IUid> tIncomingProfiles;
   tIncomingProfiles mIncomingProfiles;

   PIM::CClientAppRegistry mRegistry;

   typedef std::tr1::tuple<Profile::Uid, Profile::IUid, Service::Uid> tPendingService;
   typedef std::map<AXIS::Service::SessionUid, tPendingService> tPendingServiceMap;
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
