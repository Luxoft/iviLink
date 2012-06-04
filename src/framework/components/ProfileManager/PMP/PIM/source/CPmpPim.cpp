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






#include <cassert>

#include "../include/CPmpPim.hpp"
#include "../include/CProfileData.hpp"
#include "../include/CProfileInstanceData.hpp"
#include "../include/helpers.hpp"

#include "framework/components/ProfileManager/PMP/interaction/include/IPmpPimProtocol.hpp"
#include "framework/components/ProfileManager/PMP/ipc_protocol/include/IPMPIpcToPIM.hpp"

#include "framework/components/ProfileManager/PMP/componentManager/include/CPmpComponentManager.hpp"

#include "framework/libraries/AppMan/Pmp/include/IPmpRequest.hpp"

#include "utils/threads/include/CThreadPool.hpp"

namespace AXIS {
namespace PMP {

Logger CPmpPim::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.PIM.CPmpPim"));

using PIM::CProfileData;
using PIM::CProfileInstanceData;

struct CIncomingProfileJobData : public IThreadPoolJobData
{
   CIncomingProfileJobData(CPmpPim* pPim,
         CUid const& appId,
         AXIS::Profile::Uid const& profileUid,
         AXIS::Profile::IUid const& piuid,
         AXIS::Service::Uid const& sid) :
      mpPim(pPim),
      mAppId(appId),
      mProfileUid(profileUid),
      mPiuid(piuid),
      mSid(sid)
   {
      Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.PIM.CPmpPim"));
      LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ );
   }

   virtual ~CIncomingProfileJobData()
   {
      Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.PIM.CPmpPim"));
      LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ );
   }

   CPmpPim* mpPim;
   CUid mAppId;
   AXIS::Profile::Uid mProfileUid;
   AXIS::Profile::IUid mPiuid;
   AXIS::Service::Uid mSid;

};

class CPIMJobStarter
{
public:
   static void incomingProfile(IThreadPoolJobData const* pJobData)
   {
      Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.PIM.CPmpPim"));
      LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ );
      assert(pJobData);
      CIncomingProfileJobData const* data = static_cast<CIncomingProfileJobData const*>(pJobData);
      CPmpPim* pim = data->mpPim;

      pim->processIncomingProfile(data->mAppId, data->mProfileUid, data->mPiuid, data->mSid);
   }
};



void CPmpPim::generatePIUID(AXIS::Profile::IUid& piuid,
   AXIS::Ipc::DirectionID const& dirId)
{
   piuid = AXIS::generateUid();
}

void CPmpPim::releasePIUID(AXIS::Profile::IUid const& piuid,
   AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_INFO(msLogger, "todo: Currently, it is not used. I planned, that this "
         "method will somehow return the uid to the pool of available id's, "
         "but this is not necessary now");
   /// @todo Currently, it is not used.
   /// I planned, that this method will somehow return the uid to the pool of
   /// available id's, but this is not necessary now.
}

void CPmpPim::createProfile(AXIS::CUid const& profileUid,
      AXIS::Profile::IUid const& piuid,
      AXIS::Service::Uid const& sid,
      AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   LOG4CPLUS_INFO(msLogger, "prfUid: " + profileUid.value() +
         ", piuid: " + piuid.value() + ", sid: " + sid.value() +
         + ", dirId: " + convertIntegerToString(dirId));

   CUid appId;
   CError err = mRegistry.findAppByDirId(dirId, appId);
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return;
   }

   bool remoteCreation = (mIncomingProfiles.erase(piuid) != 0);

   CProfileData data = { piuid, sid, profileUid };
   err = mRegistry.registerProfile(appId, data);

   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return;
   }

   if (remoteCreation)
   {
      // Creation of profiles is started on other side. No need to message.
      return;
   }

   if (mpCore)
   {
      Profile::Uid compUid;
      LOG4CPLUS_INFO(msLogger, "looking for complementary profile");
      bool res = mpCore->getComplementary(profileUid, compUid);
      if (res)
      {
         LOG4CPLUS_INFO(msLogger, "found complementary " + compUid.value() +
               ", sending to other side");
         err = mpProtocol->createProfile(compUid, piuid, sid);
         LOG4CPLUS_INFO(msLogger, static_cast<std::string>(err));
      }
      else
      {
         LOG4CPLUS_INFO(msLogger, "unable to find complementary for " +
               profileUid.value());
      }
   }
   else 
   {
      LOG4CPLUS_INFO(msLogger, "mpCore is NULL - unable to get complementary profile");
   }
   
}


void CPmpPim::profileDied(AXIS::Profile::IUid const& piuid,
   AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   AXIS::Service::Uid sid;
   CError err = mRegistry.findSidByPIUID(piuid, sid);
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return;
   }

   err = mRegistry.unregisterProfile(piuid);
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return;
   }

   err = mpProtocol->profileDied(piuid, sid);
   LOG4CPLUS_INFO(msLogger, static_cast<std::string>(err));
}

void CPmpPim::readyToServe(AXIS::Service::SessionUid const& sesUid, 
   AXIS::Ipc::DirectionID const &dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   tPendingService pendingServ;
   CUid appId;

   LOG4CPLUS_INFO(msLogger, "before lock");
   mPendingServiceMapMutex.lock();
   LOG4CPLUS_INFO(msLogger, "after lock");
   {
      LOG4CPLUS_INFO(msLogger, "this = " + convertIntegerToString((intptr_t)this) +
            " mPendingServiceMap = " + convertIntegerToString((intptr_t)&mPendingServiceMap));
      LOG4CPLUS_INFO(msLogger, "mPendingServiceMap.size() " +
            convertIntegerToString(static_cast<int>(mPendingServiceMap.size())));
      for (tPendingServiceMap::iterator it = mPendingServiceMap.begin(); it != mPendingServiceMap.end(); ++it)
      {
         LOG4CPLUS_INFO(msLogger, "service ses " + it->first.value());
      }

      tPendingServiceMap::iterator it = mPendingServiceMap.find(sesUid);
      if (it == mPendingServiceMap.end())
      {
         LOG4CPLUS_WARN(msLogger, "unknown sesUid '" + sesUid.value() +
               "' from dirId " + convertIntegerToString(dirId));
      }
      else
      {
         pendingServ = it->second;
         AXIS::Service::Uid sid = std::tr1::get<2>(pendingServ);

         CError err = mRegistry.findAppByDirId(dirId, appId);
         if (!err.isNoError())
         {
            LOG4CPLUS_ERROR(msLogger, "unknown dirId " + convertIntegerToString(dirId));
         }
         else
         {
            mRegistry.addServiceToApp(appId, sid);
         }

         mPendingServiceMap.erase(it);
      }
   }
   mPendingServiceMapMutex.unlock();

   if (appId.empty())
   {
      LOG4CPLUS_ERROR(msLogger, "CPmpPim::readyToServe appId not found");
      return;
   }

   LOG4CPLUS_INFO(msLogger, "CPmpPim::readyToServe saveIncomingProfileForProcessing");
   saveIncomingProfileForProcessing(appId,
      std::tr1::get<0>(pendingServ),
      std::tr1::get<1>(pendingServ),
      std::tr1::get<2>(pendingServ));
}

void CPmpPim::onConnection(AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   CUid appId = AXIS::generateUid();

   CError err = mRegistry.registerApp(appId, dirId);
   assert(err.isNoError());
}

void CPmpPim::onConnectionLost(AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   CUid appId;
   CError err = mRegistry.findAppByDirId(dirId, appId);
   assert(err.isNoError());

   err = mRegistry.unregisterApp(appId);
}

bool CPmpPim::hasInstance(Profile::Uid const& profileId)
{
   return mRegistry.hasInstancesOf(profileId);
}

void CPmpPim::unloadInstances(Profile::Uid const& profileId)
{
   std::vector<CProfileInstanceData> profiles;

   mRegistry.findInstancesOf(profileId, profiles);

   /// @todo implement direction id search
   LOG4CPLUS_INFO(msLogger, "todo implement direction id search");

   AXIS::Ipc::DirectionID dirId = -1;

   for (std::vector<CProfileInstanceData>::iterator it = profiles.begin(); it != profiles.end(); ++it)
   {
      CProfileInstanceData const& data = *it;

      CError err = mpProtocol->profileDied(data.piuid, data.sid);
      if (!err.isNoError())
      {
         /// @todo error check
         LOG4CPLUS_INFO(msLogger, "todo: error check");
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      }

      err = mpIpc->profileDied(data.piuid, dirId);
      if (!err.isNoError())
      {
         /// @todo error check
         LOG4CPLUS_INFO(msLogger, "todo: error check");
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      }

      err = mRegistry.unregisterProfile(data.piuid);
      if (!err.isNoError())
      {
         /// @todo error check
         LOG4CPLUS_INFO(msLogger, "todo: error check");
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      }
   }
}

void CPmpPim::appRequestError(AXIS::Service::SessionUid session)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CPmpPim::appManConnectionLost()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CPmpPim::onCreateProfile(AXIS::Profile::Uid const& profileUid,
   AXIS::Profile::IUid const& piuid,
   AXIS::Service::Uid const& sid)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   CUid appId;

   /// @todo implement proper solution
   LOG4CPLUS_INFO(msLogger, "todo implement proper solution");

   CSignalSemaphore semTimer;
   bool havePending = false;

   do {
      mPendingServiceMapMutex.lock();
      {
         havePending = !mPendingServiceMap.empty();
      }
      mPendingServiceMapMutex.unlock();
      semTimer.waitTimeout(300);
   } while (havePending);

   mPendingServiceMapMutex.lock();
   {
      CError err = mRegistry.findAppBySid(sid, appId);
      if (err.isNoError())
      {
         // service was pending, but now it is know
         LOG4CPLUS_INFO(msLogger, "service known");
         saveIncomingProfileForProcessing(appId, profileUid, piuid, sid);
      }
      else
      {
         // service is not pending and unknown. need to create request
         LOG4CPLUS_INFO(msLogger, "service unknown");

         Service::SessionUid sesUid = generateUid();
         LOG4CPLUS_INFO(msLogger, "pending sesUid '" + sesUid.value() + "'");
         mPendingServiceMap.insert(std::make_pair(sesUid,
            tPendingService(profileUid, piuid, sid)));

         LOG4CPLUS_INFO(msLogger, "this = " + convertIntegerToString((intptr_t)this) +
               " mPendingServiceMap = " + convertIntegerToString((intptr_t)&mPendingServiceMap));
         LOG4CPLUS_INFO(msLogger, "mPendingServiceMap.size() " +
               convertIntegerToString(static_cast<int>(mPendingServiceMap.size())));
         for (tPendingServiceMap::iterator it = mPendingServiceMap.begin(); it != mPendingServiceMap.end(); ++it)
         {
            LOG4CPLUS_INFO(msLogger, "service ses '" + it->first.value() + "'");
         }


         AppMan::IPmpRequest* pReq = CPmpComponentManager::instance()->applicationManager();
         if (pReq)
         {
            err = pReq->applicationRequest(sesUid, sid);
            if (!err.isNoError())
            {
               LOG4CPLUS_ERROR(msLogger, "applicationRequest failed: " + static_cast<std::string>(err));
            }
         }
         else
         {
            LOG4CPLUS_ERROR(msLogger, "no app man");
         }
      } // if no err
   }
   mPendingServiceMapMutex.unlock();

}

void CPmpPim::onProfileDied(AXIS::Profile::IUid const& piuid,
      AXIS::Service::Uid const& sid)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   LOG4CPLUS_INFO(msLogger, "todo find app");
   CUid appId;

   LOG4CPLUS_INFO(msLogger, "todo implement");
   AXIS::Ipc::DirectionID dirId = -1;

   CError err = mRegistry.unregisterProfile(piuid);

   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return;
   }

   err = mpIpc->profileDied(piuid, dirId);
   LOG4CPLUS_INFO(msLogger, static_cast<std::string>(err));
}


CPmpPim::CPmpPim(IPmpPimProtocol* pProtocol)
   : mpProtocol(pProtocol)
   , mpIpc(0)
   , mpCore(0)
   , mpThreadPool(new CThreadPool(1))
{
   assert(pProtocol);
}


void CPmpPim::setIpc(IPMPIpcToPIM* pIpc)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   
   mpIpc = pIpc;
   assert(mpIpc);
}

void CPmpPim::setCore(AXIS::PMP::IPmpCoreToPim *pCore)
{

   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   mpCore = pCore;
}

CPmpPim::~CPmpPim()
{

   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   delete mpThreadPool;
   mpThreadPool = NULL;
}

void CPmpPim::saveIncomingProfileForProcessing(AXIS::CUid const& appId,
   AXIS::Profile::Uid const& profileUid,
   AXIS::Profile::IUid const& piuid,
   AXIS::Service::Uid const& sid)
{

   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   // Here is the thread border
   CIncomingProfileJobData* data = new CIncomingProfileJobData(this, appId, profileUid, piuid, sid);
   CThreadPoolJob job(CPIMJobStarter::incomingProfile, data);
   mpThreadPool->addJob(job);
}

void CPmpPim::processIncomingProfile(AXIS::CUid const& appId,
   AXIS::Profile::Uid const& profileUid,
   AXIS::Profile::IUid const& piuid,
   AXIS::Service::Uid const& sid)
{

   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   Profile::ApiUid papiUid;
   if (mpCore)
   {
      LOG4CPLUS_INFO(msLogger, "looking for papiUid of profile");
      bool res = mpCore->getPapiUid(profileUid, papiUid);
      if (res)
      {
         LOG4CPLUS_INFO(msLogger, "found papiUid " + papiUid.value());
      }
      else
      {
         LOG4CPLUS_INFO(msLogger, "unable to find papiUid for " + profileUid.value());
         return;
      }
   }
   else 
   {
      LOG4CPLUS_ERROR(msLogger, "mpCore is NULL - unable to get papiUid of profile");
      return;
   }

   AXIS::Ipc::DirectionID dirId = -1;
   CError err = mRegistry.findDirIdByApp(appId, dirId);
   if (!err.isNoError())
   {
      LOG4CPLUS_INFO(msLogger, static_cast<std::string>(err));
      return;
   }

   CUid tmpAppId;
   err = mRegistry.findAppByPIUID(piuid, tmpAppId);
   if (err.isNoError())
   {
      LOG4CPLUS_INFO(msLogger, "known piuid " + piuid.value());
      return;
   }

   mIncomingProfiles.insert(piuid);

   err = mpIpc->incomingProfile(profileUid, papiUid, piuid, sid, dirId);
   LOG4CPLUS_INFO(msLogger, static_cast<std::string>(err));
}

}  // namespace ProfileManager
}  // namespace AXIS

