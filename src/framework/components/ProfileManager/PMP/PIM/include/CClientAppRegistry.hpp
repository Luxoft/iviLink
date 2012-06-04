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






#ifndef CCLIENTAPPREGISTRY_HPP_
#define CCLIENTAPPREGISTRY_HPP_

#include <map>
#include <set>
#include <vector>

#include "utils/misc/include/Logger.hpp"
#include "utils/ipc/include/common.hpp"
#include "utils/misc/include/CError.hpp"
#include "utils/threads/include/CMutex.hpp"

#include "utils/misc/include/UID.hpp"

#include "CAppData.hpp"
#include "CProfileData.hpp"
#include "CProfileInstanceData.hpp"

namespace AXIS {

class CUid;

namespace PMP {
namespace PIM {


class CAppData;
class CProfileData;

class CClientAppRegistry
{
public:

   CError registerApp(CUid const& appId, AXIS::Ipc::DirectionID const& dirId);

   CError unregisterApp(CUid const& appId);

   CError registerProfile(CUid const& appId, CProfileData const& data);

   CError unregisterProfile(AXIS::Profile::IUid const& piuid);

   CError findAppByDirId(AXIS::Ipc::DirectionID const& dirId, CUid& appId) const;
   CError findDirIdByApp(CUid const& appId, AXIS::Ipc::DirectionID& dirId) const;

   CError findAppByPIUID(AXIS::Profile::IUid const& piuid, CUid& appId) const;

   CError findSidByPIUID(AXIS::Profile::IUid const& piuid, AXIS::Service::Uid & sid) const;

   bool hasInstancesOf(AXIS::Profile::Uid const& uid) const;

   void findInstancesOf(AXIS::Profile::Uid const& uid, std::vector<CProfileInstanceData> & result) const;

   bool isAppHaveService(AXIS::CUid const& appId, AXIS::Service::Uid const& sid) const;

   void addServiceToApp(AXIS::CUid const& appId, AXIS::Service::Uid const& sid);

   void removeServiceFromApp(AXIS::CUid const& appId, AXIS::Service::Uid const& sid);

   CError findAppBySid(AXIS::Service::Uid const& sid, AXIS::CUid & appId) const;

   /// @todo for testing. Remove it.
   CError getFirstApp(AXIS::CUid & appId);

public:

   CClientAppRegistry();
   ~CClientAppRegistry();

private:

   CClientAppRegistry(CClientAppRegistry const&);
   CClientAppRegistry& operator=(CClientAppRegistry const&);

private:

   typedef std::map<AXIS::Profile::IUid, AXIS::CUid> tProfileToApp;
   tProfileToApp mProfileToApp;
   mutable CMutex mProfileToAppMutex;

   struct IsPiuidInProfDataPred;
   struct IsProfileUidInProfDataPred;
   struct IsDirIdInAppDataPred;

   typedef std::set<CProfileData> tProfileDataSet;
   typedef std::map<AXIS::CUid, tProfileDataSet > tAppProfiles;
   tAppProfiles mAppProfiles;
   mutable CMutex mAppProfilesMutex;

   typedef std::map<AXIS::CUid, CAppData> tAppDatas;
   tAppDatas mAppDatas;
   mutable CMutex mAppDatasMutex;

   typedef std::map<AXIS::Ipc::DirectionID, AXIS::CUid> tDirIdAppMap;
   tDirIdAppMap mDirIdAppMap;

   static Logger msLogger;

};


}  // namespace PIM
}  // namespace ProfileManager
}  // namespace AXIS


#endif /* CCLIENTAPPREGISTRY_HPP_ */
