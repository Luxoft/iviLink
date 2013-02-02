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


#include <cassert>
#include <utility>
#include <unistd.h>

#include "CThreadPool.hpp"

#include "ProfileHolder.hpp"

#include "CComponentMgr.hpp"
#include "IProfileManagersCallbacks.hpp"

#include "IPMALIpcToPIM.hpp"
#include "IPMALCoreToPIM.hpp"

#include "CPIM.hpp"
#include "ProfileInternal.hpp"

#include "PIUIDGenerator.hpp"
#include "profileFactory.hpp"


static const char gModuleName[] = "CPIM";

namespace iviLink {
namespace PMAL {
namespace PIM {

class CProfileEnabler
{
   static Logger logger;

   CProfileEnabler();
public:
   static void enable(Profile::ProfileInternal* p)
   {
      LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + (" " + convertIntegerToString((intptr_t)p)));
      assert(p);
      p->setEnabled(true);
   }

   static void disable(Profile::ProfileInternal* p)
   {
      LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + (" " + convertIntegerToString((intptr_t)p)));
      assert(p);
      p->setEnabled(false);
   }
};

class CProfileDestroyer
{
   static Logger logger;

   CProfileDestroyer();
public:
   static void destroy(Profile::ProfileInternal* p)
   {
      LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + (" " + convertIntegerToString((intptr_t)p)));
      assert(p);
      delete p;
   }
};


}  // namespace PIM

struct CIncomingProfileJobData : public IThreadPoolJobData
{
   CIncomingProfileJobData(CPIM* pPim,
         iviLink::Profile::Uid const& profileUid,
         iviLink::Profile::ApiUid const papiUid,
         iviLink::Profile::IUid const& piuid,
         iviLink::Service::Uid const& serviceUID) :
      mpPim(pPim),
      mProfileUid(profileUid),
      mPapiUid(papiUid),
      mPiuid(piuid),
      mServiceUID(serviceUID)
   {
   }

   virtual ~CIncomingProfileJobData()
   {
   }

   CPIM* mpPim;
   iviLink::Profile::Uid mProfileUid;
   iviLink::Profile::ApiUid mPapiUid;
   iviLink::Profile::IUid mPiuid;
   iviLink::Service::Uid mServiceUID;
};

struct CProfileDiedJobData : public IThreadPoolJobData
{
   CProfileDiedJobData(CPIM* pPim, iviLink::Profile::IUid const& piuid) :
      mpPim(pPim),
      mPiuid(piuid)
   {}

   CPIM* mpPim;
   iviLink::Profile::IUid mPiuid;
};

class CPIMJobStarter
{
   static Logger logger;

public:
   static void incomingProfile(IThreadPoolJobData const* pJobData)
   {
      LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
      assert(pJobData);
      CIncomingProfileJobData const* data = static_cast<CIncomingProfileJobData const*>(pJobData);
      CPIM* pim = data->mpPim;

      pim->incomingProfileInternal(data->mProfileUid, data->mPapiUid, data->mPiuid, data->mServiceUID);
   }

   static void profileDied(IThreadPoolJobData const* pJobData)
   {
      LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
      assert(pJobData);
      CProfileDiedJobData const* data = static_cast<CProfileDiedJobData const*>(pJobData);
      CPIM* pim = data->mpPim;

      pim->profileDiedInternal(data->mPiuid);
   }
};

////
CPMALError CPIM::loadProfile(iviLink::Profile::Uid const& profileUid,
            iviLink::Service::Uid const& sid,
            iviLink::Profile::IProfileCallbackProxy* const pProxy,
            Profile::ProfileHolder*& pProfile,
            Android::AppInfo appInfo)
{

   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + (" uid" + profileUid.value()));

   Profile::ProfileHolder* pProf = NULL;

   std::string path;

   // preparing

   CPMALComponentMgr* pMgr = CPMALComponentMgr::getInstance();
   if (!pMgr)
   {
      return CPMALError(CPMALError::ERROR_PIM_INTERNAL, gModuleName, "no instance of CPMALComponentMgr. Something is very wrong");
   }

   IPMALCoreToPIM* pCore = pMgr->getCoreToPIM();
   if (!pCore)
   {
      return CPMALError(CPMALError::ERROR_PIM_INTERNAL, gModuleName, "no instance of PMAL Core");
   }

   bool profileEnabled = false;
   BaseError err = pCore->isEnabledProfile(profileUid, profileEnabled);
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(logger, static_cast<std::string>(err));
      return CPMALError(CPMALError::ERROR_PIM_INTERNAL, gModuleName, "unable to get state of profile");
   }

   if (!profileEnabled)
   {
      LOG4CPLUS_ERROR(logger, "profile is disabled");
      return CPMALError(CPMALError::ERROR_UNKNOWN_PROFILE_UID, gModuleName, "profile is disabled or unknown");
   }

   err = pCore->getProfileLibPath(profileUid, path);
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(logger, static_cast<std::string>(err));
      return CPMALError(CPMALError::ERROR_UNKNOWN_PROFILE_UID, gModuleName, "no profile library path");
   }

   Profile::IUid piuid;
   {
      CPMALError err = generatePIUID(profileUid, sid, piuid);
      if (!err.isNoError())
         return err;
   }

   // creating
   LOG4CPLUS_INFO(logger, "creating profile '" + path + "'with piuid '" + piuid.value() + "'");
   Profile::ProfileInternal* pProfInt = NULL;
   PIM::ProfileInitData initData(piuid, sid, pProxy);
   err = PIM::createProfileImpl(path.c_str(), initData, pProfInt, appInfo);

   if (!err.isNoError())
   {
      PIM::releasePIUID(piuid);

      LOG4CPLUS_ERROR(logger, static_cast<std::string>(err));
      return CPMALError(CPMALError::ERROR_PIM_INTERNAL, gModuleName, "unable to create profile object");
   }

   assert(pProfInt);

   err = pProfInt->getError();
   if (!err.isNoError())
   {
      PIM::releasePIUID(piuid);
      err = pProfInt->getError();
      LOG4CPLUS_ERROR(logger, static_cast<std::string>(err));

      PIM::CProfileDestroyer::destroy(pProfInt);
      return CPMALError(CPMALError::ERROR_PROFILE_INIT, gModuleName, "profile implementation init error");
   }

   // registering

   pProf = static_cast<Profile::ProfileHolder*>(pProfInt);
   err = mInstanceMap.registerProfile(piuid, pProf);
   if (!err.isNoError())
   {
      PIM::CProfileDestroyer::destroy(pProf);

      LOG4CPLUS_ERROR(logger, static_cast<std::string>(err));

      return CPMALError(CPMALError::ERROR_PIM_INTERNAL, gModuleName, "unable to register profile object");
   }

   // messaging
   IPMALIpcToPIM* ipc = pMgr->getIpcToPIM();
   assert(ipc);
   bool res = ipc->createProfile(profileUid, piuid, sid);
   if (!res)
   {
       return CPMALError(CPMALError::ERROR_OTHER);
   }

   // enabling

   PIM::CProfileEnabler::enable(pProf);

   pProfile = pProf;

   return CPMALError::NoPMALError(gModuleName);
}

CPMALError CPIM::unloadProfile(Profile::ProfileHolder *& profile)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + (" " + convertIntegerToString((intptr_t)profile)));

   Profile::IUid piuid;
   BaseError err = mInstanceMap.find(profile, piuid);
   if (!err.isNoError())
   {
      /// @todo what error handling can be here?
      LOG4CPLUS_ERROR(logger, static_cast<std::string>(err));
      return CPMALError(CPMALError::ERROR_WRONG_PARAM, gModuleName);
   }

   unloadProfile(piuid, profile);

   return CPMALError::NoPMALError(gModuleName);
}

void CPIM::incomingProfile(iviLink::BaseUid const& profileUid,
         iviLink::Profile::ApiUid const& papiUid,
         iviLink::Profile::IUid const& piuid,
         iviLink::Service::Uid const& serviceUID)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + (" uid " + profileUid.value() + " piuid" + piuid.value() + " sid " + serviceUID.value()));
   // Here is the thread border - to return the code flow to the ipc as
   // fast as possible
   CIncomingProfileJobData* data = new CIncomingProfileJobData(this, profileUid, papiUid, piuid, serviceUID);
   CThreadPoolJob job(CPIMJobStarter::incomingProfile, data);
   mpThreadPool->addJob(job);
}

void CPIM::profileDied(iviLink::Profile::IUid const& piuid)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + (" piuid " + piuid.value()));
   // Here is the thread border - to return the code flow to the ipc as
   // fast as possible

   CProfileDiedJobData* data = new CProfileDiedJobData(this, piuid);
   CThreadPoolJob job(CPIMJobStarter::profileDied, data);
   mpThreadPool->addJob(job);
}

BaseError CPIM::sessionRequest(iviLink::Service::SessionUid session,
   iviLink::Service::Uid service)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + (" session " + session.value() + " service " + service.value()));

   CPMALComponentMgr *pMgr = CPMALComponentMgr::getInstance();
   if (pMgr)
   {
      IPMALIpcToPIM* ipc = pMgr->getIpcToPIM();
      if (ipc)
      {
         bool res = ipc->readyToServe(session);
         return res ? CPMALError::NoPMALError() : CPMALError(CPMALError::ERROR_OTHER);
      }
      return BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "sessionRequest() no ipc");
   }
   return BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "sessionRequest() no manager");
}

CPIM::CPIM() :
      mpThreadPool(new CThreadPool(1))
{
}

CPIM::~CPIM()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   unloadAll();
   delete mpThreadPool;
}

void CPIM::unloadAll()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   typedef PIM::CProfileInstanceMap::tProfilesList PL;
   PL profiles;
   BaseError err = mInstanceMap.getAllRegisterd(profiles);
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(logger, static_cast<std::string>(err));
      return;
   }

   IProfileManagerCallbacks* clbs = NULL;
   CPMALComponentMgr* mgr = CPMALComponentMgr::getInstance();
   if (mgr)
   {
      clbs = mgr->getProfileManagerCallbacks();
   }

   for (PL::iterator it = profiles.begin(); it != profiles.end(); ++it)
   {
      Profile::IUid const& piuid = it->first;
      Profile::ProfileHolder* pp = static_cast<Profile::ProfileHolder*>(it->second);

      unloadProfileThroughClient(clbs, pp, piuid);
   }
}

void CPIM::unloadProfileThroughClient(IProfileManagerCallbacks* clbs, Profile::ProfileHolder* pp, Profile::IUid const& piuid)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + ("clbs " + convertIntegerToString((intptr_t)clbs) + " pp " + convertIntegerToString((intptr_t)pp) + " piuid " + piuid.value()));

   bool reallyUnloaded = false;

   if (clbs)
   {
      // If we have callback to client, we messaging him about died profile
      clbs->profileDied(pp->getServiceUID(), piuid);

      reallyUnloaded = !mInstanceMap.isKnown(piuid).isNoError();
   }

   if (!reallyUnloaded)
   {
      // We have no callback to client or client not called our
      // unloadProfile(), so we have to unload this profile manually
      unloadProfile(piuid, pp);
   }
}

void CPIM::unloadProfile(Profile::IUid const& piuid, Profile::ProfileHolder *& profile)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + (" piuid " + piuid.value() + " profile " + convertIntegerToString((intptr_t)profile)));

   BaseError err = mInstanceMap.unregisterProfile(profile);
   if (!err.isNoError())
   {
      // Someone was faster and already removed this profile
      LOG4CPLUS_INFO(logger, "unloadProfile: profile already unloaded, err = " + static_cast<std::string>(err));
      return;
   }

   PIM::CProfileEnabler::disable(profile);
   
   if (mRemoteDiedProfiles.erase(piuid) == 0)
   {
      CPMALComponentMgr *pMgr = CPMALComponentMgr::getInstance();
      if (pMgr)
      {
         IPMALIpcToPIM* ipc = pMgr->getIpcToPIM();
         if (ipc)
         {
            bool res = ipc->profileDied(piuid);
            if (!res)
            {
               LOG4CPLUS_ERROR(logger, "IPC error");
            }
         }
      }
   }

   PIM::releasePIUID(piuid);
   PIM::CProfileDestroyer::destroy(profile);
   
   profile = NULL;
   PIM::unloadProfileLib(piuid);
}

void CPIM::incomingProfileInternal(iviLink::Profile::Uid const& profileUid,
      iviLink::Profile::ApiUid const& papiUid,
      iviLink::Profile::IUid const& piuid,
      iviLink::Service::Uid const& serviceUID)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + (" uid " + profileUid.value() + " piuid" + piuid.value() + " sid " + serviceUID.value()));

   CPMALComponentMgr* mgr = CPMALComponentMgr::getInstance();
   if (!mgr)
   {
      LOG4CPLUS_ERROR(logger, "!mgr");
      return;
   }

   IProfileManagerCallbacks* clb = mgr->getProfileManagerCallbacks();
   if (!clb)
   {
      LOG4CPLUS_ERROR(logger, "!clb");
      return;
   }

   BaseError err = clb->serviceRequestCallback(serviceUID);
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(logger, static_cast<std::string>(err));
      return;
   }

   mIncomingPrfMapMutex.lock();
   {
      mIncomingPrfMap.insert(std::make_pair(std::make_pair(serviceUID, profileUid), piuid));
   }
   mIncomingPrfMapMutex.unlock();

   clb->incomingProfile(serviceUID, profileUid, papiUid);
}

void CPIM::profileDiedInternal(iviLink::Profile::IUid const& piuid)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   Profile::ProfileInternal* pi = NULL;

   BaseError err = mInstanceMap.find(piuid, pi);
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(logger, static_cast<std::string>(err));
      return;
   }

   CPMALComponentMgr* mgr = CPMALComponentMgr::getInstance();
   IProfileManagerCallbacks* clb = NULL;
   if (mgr)
   {
      clb = mgr->getProfileManagerCallbacks();
   }

   mRemoteDiedProfiles.insert(piuid);

   unloadProfileThroughClient(clb, static_cast<Profile::ProfileHolder*>(pi), piuid);
}

CPMALError CPIM::generatePIUID(iviLink::BaseUid const& profileUid,
      iviLink::Service::Uid const& serviceUID,
      iviLink::Profile::IUid & piuid)
{
   bool remote = false;

   mIncomingPrfMapMutex.lock();
   {
      if (!mIncomingPrfMap.empty())
      {
         tIncomingPrfMap::iterator it = mIncomingPrfMap.find(std::make_pair(serviceUID, profileUid));
         if (it != mIncomingPrfMap.end())
         {
            piuid = it->second;
            mIncomingPrfMap.erase(it);
            remote = true;
         }
      }
   }
   mIncomingPrfMapMutex.unlock();

   if (remote)
      return CPMALError::NoPMALError(gModuleName);

   BaseError err = PIM::getPIUID(piuid);
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(logger, static_cast<std::string>(err));
      return CPMALError(CPMALError::ERROR_PIM_INTERNAL, gModuleName, "no piuid");
   }

   return CPMALError::NoPMALError(gModuleName);
}


Logger CPIM::logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Pmal.Pim"));
Logger PIM::CProfileEnabler::logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Pmal.Pim.CProfileEnabler"));
Logger PIM::CProfileDestroyer::logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Pmal.Pim.CProfileDestroyer"));
Logger CPIMJobStarter::logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Pmal.Pim.CPIMJobStarter"));


}  // namespace PMAL
}  // namespace AXIS


