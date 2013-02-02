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


#ifndef CPIM_HPP_
#define CPIM_HPP_

#include <map>
#include <set>

#include "Logger.hpp"

#include "CMutex.hpp"

#include "IPMALPIM.hpp"
#include "IAppManHandler.hpp"

#include "IPMALPIMToIpc.hpp"
#include "CProfileInstanceMap.hpp"

#include "AppInfo.hpp"

class CThreadPool;
class IThreadPoolJobData;

namespace iviLink {
namespace PMAL {

class CPIMJobStarter;

class CPIM : public IPMALPIM, public IPMALPIMToIpc, public AppMan::IAppManHandler
{
   static Logger logger;
   
public:
   // from IPMALPIM
   virtual CPMALError loadProfile(iviLink::Profile::Uid const& profileUid,
         iviLink::Service::Uid const& sid,
         iviLink::Profile::IProfileCallbackProxy* const pProxy,
         Profile::ProfileHolder*& pProfile,
         Android::AppInfo appInfo = Android::AppInfo());  
         /*
          * we copy profiles' *.so to the AppInfo's internalPath and load them from there,
          * otherwise on Android 2.2 devices (and maybe on others) fail is imminent
          */

   virtual CPMALError unloadProfile(Profile::ProfileHolder*& profile);

public:
   // from IPMALPIMToIpc

   virtual void incomingProfile(iviLink::BaseUid const& profileUid,
      iviLink::Profile::ApiUid const& papiUid,
      iviLink::Profile::IUid const& piuid,
      iviLink::Service::Uid const& serviceUID);

   virtual void profileDied(iviLink::Profile::IUid const& piuid);

public:
   // from IAppManHandler

   virtual BaseError sessionRequest(iviLink::Service::SessionUid session,
      iviLink::Service::Uid service);

public:

   CPIM();
   ~CPIM();

private:

   friend class CPIMJobStarter;

   CPIM(CPIM const &);
   CPIM& operator=(CPIM const &);
   void unloadAll();
   void unloadProfileThroughClient(IProfileManagerCallbacks* clbs, Profile::ProfileHolder* pp, Profile::IUid const& piuid);
   void unloadProfile(Profile::IUid const& piuid, Profile::ProfileHolder *& profile);

   void incomingProfileInternal(iviLink::Profile::Uid const& profileUid,
      iviLink::Profile::ApiUid const& papiUid,
      iviLink::Profile::IUid const& piuid,
      iviLink::Service::Uid const& serviceUID);

   void profileDiedInternal(Profile::IUid const& piuid);

   /**
    * Generates the piuid for profile on its loading.
    * Method can get the piuid from PMP or return the piuid passed for incoming
    * profile.
    *
    * @param profileUid needed to find possible incoming profile
    * @param serviceUID needed to find possible incoming profile
    * @param[out] piuid generated uid for profile instance. Undefined in case
    *    of any error
    *
    * @retval IVILINK_NO_ERROR piuid generated successfully
    * @retval ERROR_PIM_INTERNAL unable to obtain piuid using ipc
    */
   CPMALError generatePIUID(iviLink::BaseUid const& profileUid,
      iviLink::Service::Uid const& serviceUID,
      iviLink::Profile::IUid & piuid);

private:
   // members section

   PIM::CProfileInstanceMap mInstanceMap;

   CThreadPool* mpThreadPool;

   typedef std::pair<Service::Uid, Profile::Uid> tIncomingPrfUID;
   typedef std::multimap<tIncomingPrfUID, Profile::IUid> tIncomingPrfMap;
   tIncomingPrfMap mIncomingPrfMap;
   CMutex mIncomingPrfMapMutex;

   typedef std::set<Profile::IUid> tRemoteDiedProfiles;
   tRemoteDiedProfiles mRemoteDiedProfiles;
};


}  // namespace ProfileManager
}  // namespace AXIS


#endif /* CPIM_HPP_ */
