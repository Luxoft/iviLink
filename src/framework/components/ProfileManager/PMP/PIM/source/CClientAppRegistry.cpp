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





#include <algorithm>
#include <cassert>

#include "CClientAppRegistry.hpp"
#include "helpers.hpp"

const char gModuleName[] = "CClientAppRegistry";


namespace AXIS {
namespace PMP {
namespace PIM {

Logger CClientAppRegistry::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.PIM.CClientAppRegistry"));

struct CClientAppRegistry::IsPiuidInProfDataPred : 
   public std::unary_function<CProfileData const&, bool>
{
   AXIS::Profile::IUid const& piuid;

   IsPiuidInProfDataPred(AXIS::Profile::IUid const& piuid) :
      piuid(piuid)
   {
   }

   bool operator()(CProfileData const& pdata) const
   {
      return pdata.piuid == piuid;
   }
};

struct CClientAppRegistry::IsProfileUidInProfDataPred :
   public std::unary_function<CProfileData const&, bool>
{
   AXIS::Profile::Uid const& uid;

   IsProfileUidInProfDataPred(AXIS::Profile::Uid const& uid) :
      uid(uid)
   {
   }

   bool operator()(CProfileData const& pdata) const
   {
      return pdata.profileUid == uid;
   }
};

CError CClientAppRegistry::registerApp(CUid const& appId, AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   LOG4CPLUS_INFO(msLogger, "appId: " + appId.value());
   CError err = CError::NoError(gModuleName);

   mAppDatasMutex.lock();
   {
      tAppDatas::iterator it = mAppDatas.find(appId);
      if (it != mAppDatas.end())
      {
         err = CError(1, gModuleName, CError::ERROR, "appId already known");
      }
      else
      {
         mAppDatas.insert(std::make_pair(appId, CAppData()));
      }
   }
   mAppDatasMutex.unlock();

   if (err.isNoError())
   {
      mAppProfilesMutex.lock();
      {
         mAppProfiles.insert(std::make_pair(appId, tProfileDataSet()));
         assert(mDirIdAppMap.find(dirId) == mDirIdAppMap.end());
         mDirIdAppMap.insert(std::make_pair(dirId, appId));
      }
      mAppProfilesMutex.unlock();
   }

   return err;
}

CError CClientAppRegistry::unregisterApp(CUid const& appId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   CError err = CError::NoError(gModuleName);

   mAppDatasMutex.lock();
   {
      tAppDatas::iterator it = mAppDatas.find(appId);
      if (it == mAppDatas.end())
      {
         err = CError(1, gModuleName, CError::ERROR, "appId is unknown");
      }
      else
      {
         mAppDatas.erase(it);
      }
   }
   mAppDatasMutex.unlock();

   if (err.isNoError())
   {
      mAppProfilesMutex.lock();
      {
         mAppProfiles.erase(appId);

         PairValueEquealPred<tDirIdAppMap::value_type> pred(appId);
         tDirIdAppMap::iterator it = std::find_if(mDirIdAppMap.begin(), mDirIdAppMap.end(), pred);
         assert(it != mDirIdAppMap.end());
         mDirIdAppMap.erase(it);
      }
      mAppProfilesMutex.unlock();
   }

   return err;

}

CError CClientAppRegistry::registerProfile(CUid const& appId, CProfileData const& data)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   LOG4CPLUS_INFO(msLogger, "appId '" + appId.value() + "'");

   CError err = CError::NoError(gModuleName);

   mProfileToAppMutex.lock();
   {
      tProfileToApp::iterator it = mProfileToApp.find(data.piuid);
      if (it != mProfileToApp.end())
      {
         err = CError(1, gModuleName, CError::ERROR, "piuid already known");
      }
      else
      {
         mProfileToApp.insert(std::make_pair(data.piuid, appId));
      }
   }
   mProfileToAppMutex.unlock();

   if (err.isNoError())
   {
      mAppProfilesMutex.lock();
      {
         tAppProfiles::iterator it = mAppProfiles.find(appId);
         assert(it != mAppProfiles.end());

         tProfileDataSet& ds = it->second;
         ds.insert(data);
      }
      mAppProfilesMutex.unlock();
   }

   return err;
}

CError CClientAppRegistry::unregisterProfile(AXIS::Profile::IUid const& piuid)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   CError err = CError::NoError(gModuleName);

   CUid appId;

   mProfileToAppMutex.lock();
   {
      tProfileToApp::iterator it = mProfileToApp.find(piuid);
      if (it == mProfileToApp.end())
      {
         err = CError(1, gModuleName, CError::ERROR, "piuid is unknown");
      }
      else
      {
         appId = it->second;
         mProfileToApp.erase(piuid);
      }
   }
   mProfileToAppMutex.unlock();

   if (err.isNoError())
   {
      mAppProfilesMutex.lock();
      {
         tAppProfiles::iterator it = mAppProfiles.find(appId);
         assert(it != mAppProfiles.end());
         tProfileDataSet& ds = it->second;
         IsPiuidInProfDataPred pred(piuid);
         tProfileDataSet::const_iterator dsit = std::find_if(ds.begin(), ds.end(), pred);
         if (dsit != ds.end())
         {
            ds.erase(dsit);
         }
      }
      mAppProfilesMutex.unlock();
   }

   return err;
}

CError CClientAppRegistry::findAppByDirId(AXIS::Ipc::DirectionID const& dirId, CUid& appId) const
{
   CError err = CError::NoError(gModuleName);

   mAppDatasMutex.lock();
   {
      tDirIdAppMap::const_iterator it = mDirIdAppMap.find(dirId);
      if (it == mDirIdAppMap.end())
      {
         err = CError(1, gModuleName, CError::ERROR, "given dirId is unknown");
      }
      else
      {
         appId = it->second;
      }
   }
   mAppDatasMutex.unlock();

   return err;
}

CError CClientAppRegistry::findDirIdByApp(CUid const& appId, AXIS::Ipc::DirectionID& dirId) const
{
   CError err = CError::NoError(gModuleName);

   mAppDatasMutex.lock();
   {
      PairValueEquealPred<tDirIdAppMap::value_type> pred(appId);
      tDirIdAppMap::const_iterator it = std::find_if(mDirIdAppMap.begin(), mDirIdAppMap.end(), pred);
      if (it == mDirIdAppMap.end())
      {
         err = CError(1, gModuleName, CError::ERROR, "given appId is unknown");
      }
      else
      {
         dirId = it->first;
      }
   }
   mAppDatasMutex.unlock();

   return err;
}

CError CClientAppRegistry::findAppByPIUID(AXIS::Profile::IUid const& piuid, CUid& appId) const
{
   CError err = CError::NoError(gModuleName);

   mProfileToAppMutex.lock();
   {
      tProfileToApp::const_iterator it = mProfileToApp.find(piuid);
      if (it == mProfileToApp.end())
      {
         err = CError(1, gModuleName, CError::ERROR, "piuid is unknown");
      }
      else
      {
         appId = it->second;
      }
   }
   mProfileToAppMutex.unlock();

   return err;
}


CError CClientAppRegistry::findSidByPIUID(AXIS::Profile::IUid const& piuid, AXIS::Service::Uid & sid) const
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   AXIS::CUid appId;
   CError err = findAppByPIUID(piuid, appId);

   if (!err.isNoError())
      return err;

   mAppProfilesMutex.lock();
   {
      tAppProfiles::const_iterator it = mAppProfiles.find(appId);
      if (it != mAppProfiles.end())
      {
         IsPiuidInProfDataPred pred(piuid);
         tProfileDataSet const& ds = it->second;
         tProfileDataSet::const_iterator dsit = std::find_if(ds.begin(), ds.end(), pred);
         if (dsit != ds.end())
         {
            sid = dsit->sid;
         }
         else
         {
            err = CError(1, gModuleName, CError::ERROR, "piuid is unknown");
         }
      }
      else
      {
         err = CError(1, gModuleName, CError::ERROR, "app for this piuid is unknown");
      }
   }
   mAppProfilesMutex.unlock();

   return err;
}

bool CClientAppRegistry::hasInstancesOf(AXIS::Profile::Uid const& uid) const
{
   bool res = false;

   IsProfileUidInProfDataPred pred(uid);

   mAppProfilesMutex.lock();
   {
      for (tAppProfiles::const_iterator it = mAppProfiles.begin(); it != mAppProfiles.end(); ++it)
      {
         tProfileDataSet const& ds = it->second;
         tProfileDataSet::const_iterator dsit = std::find_if(ds.begin(), ds.end(), pred);
         if (dsit != ds.end())
         {
            res = true;
            break;
         }
      }
   }
   mAppProfilesMutex.unlock();

   return res;
}

void CClientAppRegistry::findInstancesOf(AXIS::Profile::Uid const& uid, std::vector<CProfileInstanceData> & result) const
{
   IsProfileUidInProfDataPred pred(uid);

   mAppProfilesMutex.lock();
   {
      for (tAppProfiles::const_iterator it = mAppProfiles.begin(); it != mAppProfiles.end(); ++it)
      {
         tProfileDataSet const& ds = it->second;

         for (tProfileDataSet::const_iterator dsit = ds.begin(); dsit != ds.end(); ++dsit)
         {
            CProfileData const& pd = *dsit;

            if (pred(pd))
            {
               CProfileInstanceData data = {pd.piuid, pd.sid, it->first};
               result.push_back(data);
            }
         } // for dsit
      } // for it
   }
   mAppProfilesMutex.unlock();
}

bool CClientAppRegistry::isAppHaveService(AXIS::CUid const& appId, AXIS::Service::Uid const& sid) const
{
   bool res = false;
   mAppDatasMutex.lock();
   {
      tAppDatas::const_iterator it = mAppDatas.find(appId);
      if (it == mAppDatas.end())
      {
         LOG4CPLUS_WARN(msLogger, "unknown app " + appId.value());
         res = false;
      }
      else
      {
         res = it->second.hasService(sid);
      }
   }
   mAppDatasMutex.unlock();
   return res;
}

void CClientAppRegistry::addServiceToApp(AXIS::CUid const& appId, AXIS::Service::Uid const& sid)
{
   mAppDatasMutex.lock();
   {
      tAppDatas::iterator it = mAppDatas.find(appId);
      if (it == mAppDatas.end())
      {
         LOG4CPLUS_WARN(msLogger, "unknown app: " + appId.value());
      }
      else
      {
         it->second.addService(sid);
      }
   }
   mAppDatasMutex.unlock();
}

void CClientAppRegistry::removeServiceFromApp(AXIS::CUid const& appId, AXIS::Service::Uid const& sid)
{
   mAppDatasMutex.lock();
   {
      tAppDatas::iterator it = mAppDatas.find(appId);
      if (it == mAppDatas.end())
      {
         LOG4CPLUS_WARN(msLogger, "unknown app: " + appId.value());
      }
      else
      {
         it->second.removeServie(sid);
      }
   }
   mAppDatasMutex.unlock();
}

CError CClientAppRegistry::findAppBySid(AXIS::Service::Uid const& sid, AXIS::CUid & appId) const
{
   CError err = CError::NoError(gModuleName);

   mAppDatasMutex.lock();
   {
      tAppDatas::const_iterator it = mAppDatas.begin();
      for (; it != mAppDatas.end(); ++it)
      {
         if (it->second.hasService(sid))
         {
            appId = it->first;
            break;
         }
      }

      if (it == mAppDatas.end())
      {
         err = CError(1, gModuleName, CError::ERROR, "app for this sid is unknown");
      }
   }
   mAppDatasMutex.unlock();

   return err;
}


CClientAppRegistry::CClientAppRegistry()
{

}

CClientAppRegistry::~CClientAppRegistry()
{

}


}  // namespace PIM
}  // namespace ProfileManager
}  // namespace AXIS
