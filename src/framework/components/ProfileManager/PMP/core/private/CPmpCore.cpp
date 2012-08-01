/* 
 * 
 * iviLINK SDK, version 1.0
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







#include <cassert>

#include "utils/threads/CThreadPool.hpp"
#include "utils/threads/CThreadPoolJob.hpp"

#include "framework/components/ProfileRepository/CProfileRepoClient.hpp"
#include "CPmpCore.hpp"

static const iviLink::Profile::ApiUid gsAuthProfilePapiUid("AuthenticationProfile_PAPI_UID");

namespace iviLink
{

   namespace PMP
   {

      Logger CPmpCore::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.CPmpCore"));

      CPmpCore::CPmpCore(ProfileRepository::IProfileRepoClient * pRepoClient, std::string &platform)
         : mpRepoClient(pRepoClient)
         , mPlatform(platform)
         , mActualRepoState(false)
         , mpPim(0)
         , mpCoreProtocol(0)
         , mpPool(0)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         assert(mpRepoClient);
         mpPool = new CThreadPool(1);
         reloadProfilesFromRepository();
         print();
      }

      CPmpCore::~CPmpCore()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         delete mpPool;
      }

      CError CPmpCore::addProfileApi(const std::string &manifestPath)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         return mpRepoClient->addProfileApi(manifestPath);
      }

      CError CPmpCore::removeProfileApi(iviLink::Profile::ApiUid profileApiUid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         return mpRepoClient->removeProfileApi(UID(profileApiUid.value()));
      }

      CError CPmpCore::addProfile(const std::string &xmlManifestPath)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         CError err = mpRepoClient->addProfile(xmlManifestPath);
         if (err.isNoError())
         {
            mActualRepoState = false;
         }
         return err;
      }

      CError CPmpCore::removeProfile(Profile::Uid profileUid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         CError err = mpRepoClient->removeProfile(UID(profileUid.value()));
         if (err.isNoError())
         {
            mActualRepoState = false;
         }
         return err;
      }

      CError CPmpCore::addProfileImplementation(Profile::Uid profileID, const LibDescriptor& library)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         CError err = mpRepoClient->addProfileImplementation(UID(profileID.value()),library);
         if (err.isNoError())
         {
            mActualRepoState = false;
         }
         return err;
      }

      CError CPmpCore::removeProfileImplementation(Profile::Uid profileUid, const std::string &platform)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         CError err = mpRepoClient->removeProfileImplementation(UID(profileUid.value()),platform);
         if (err.isNoError())
         {
            mActualRepoState = false;
         }
         return err;
      }

      void CPmpCore::findProfilesAsync(iviLink::CUid profileApiUid,
            const std::map<std::string, std::string> & profileParameters
            , std::list<Profile::Uid> & profiles, bool enabledProfiles)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::list<LibInfo> profileInfos;
         profiles.clear();
         findProfilesAsync(profileApiUid, profileParameters, mPlatform,profileInfos,enabledProfiles);
         for (std::list<LibInfo>::const_iterator it = profileInfos.begin(); profileInfos.end() != it; ++it)
         {
            profiles.push_back(Profile::Uid(it->uid.value));
         }
      }

      void CPmpCore::findProfilesAsync(iviLink::CUid id,
            const std::map<std::string, std::string> & profileArguments,
            const std::string &platform, std::list<LibInfo>& profiles, bool onlyAvailable)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         profiles.clear();
         std::list<LibInfo> list;
         mpRepoClient->findProfiles(UID(id.value()), profileArguments, list, platform);
         LOG4CPLUS_INFO(msLogger, "List size : " + convertIntegerToString(list.size()));
         for (std::list<LibInfo>::iterator it = list.begin(); list.end() != it; ++it)
         {
            if (mProfiles.find(Profile::Uid(it->uid.value)) == mProfiles.end())
            {
               continue;
            }
            if ( it->platform == platform && ( !onlyAvailable || mProfiles[Profile::Uid(it->uid.value)].available() ) )
            {
               profiles.push_back((*it));
            }
         }
         LOG4CPLUS_INFO(msLogger, "CPmpCore::findProfilesAsync() : res size : " + convertIntegerToString(profiles.size()));
      }

      void CPmpCore::findProfiles(iviLink::CUid profileApiUid,
            const std::map<std::string, std::string> & profileParameters
            , std::list<Profile::Uid> & profiles, bool enabledProfiles)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfileMutex.lock();
         findProfilesAsync(profileApiUid,profileParameters,profiles,enabledProfiles);
         mProfileMutex.unlock();
      }

      void CPmpCore::findProfiles(iviLink::CUid id, const std::map<std::string, std::string> & profileArguments, const std::string &platform, std::list<LibInfo>& profiles, bool onlyAvailable)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfileMutex.lock();
         findProfilesAsync(id,profileArguments,platform,profiles,onlyAvailable);
         mProfileMutex.unlock();
      }

      bool CPmpCore::getManifest(iviLink::CUid uid, std::string & manifest)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         manifest = mpRepoClient->getManifest(UID(uid.value()));
         if (manifest == "")
         {
            return false;
         }
         return true;
      }

      void CPmpCore::reloadProfilesFromRepository()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfileMutex.lock();
         mProfiles.clear();
         std::list <UID> tmp = mpRepoClient->getProfilesList();
         std::list<Profile::Uid> list;
         for(std::list<UID>::iterator it = tmp.begin(); tmp.end() != it; ++it)
         {
            list.push_back(Profile::Uid((*it).value));
         }
         LOG4CPLUS_INFO(msLogger, "List of Profiles has been returned");
         LOG4CPLUS_INFO(msLogger, "List size : " + convertIntegerToString(list.size()));
         for (std::list<Profile::Uid>::const_iterator it = list.begin(); list.end() != it; ++it)
         {
            LOG4CPLUS_INFO(msLogger, "UID : " + (*it).value());
            std::map<std::string, std::string> profileArguments;
            std::list<LibInfo> impls;
            mpRepoClient->findProfiles(UID((*it).value()),profileArguments,impls,mPlatform);
            LOG4CPLUS_INFO(msLogger, "findProfiles result : " + convertIntegerToString(impls.size()));
            for (std::list<LibInfo>::const_iterator lit = impls.begin(); impls.end() != lit; ++lit)
            {
               LOG4CPLUS_INFO(msLogger, "mPlatform: " + mPlatform +
                     ", lit->platform : " + lit->platform + ", lit->path : " + lit->path);
               if (lit->platform == mPlatform)
               {
                  LOG4CPLUS_INFO(msLogger, "Found implementation: " + lit->path);
                  std::string man;
                  getManifest((*it),man);
                  mProfiles[(*it)] = CPmpCoreProfileInfo(man);
                  mProfiles[(*it)].setLibrary(lit->path);
                  break;
               }
            }
         }

         updateComplements();

         mActualRepoState = true;
         mProfileMutex.unlock();
      }

      void CPmpCore::updateComplements()
      {
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
         {
            mComplementsList.push_back(it->second.uid());
         }
      }

      void CPmpCore::resetProfilesState()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         
         disableByComplementAll();
         lockAll();
         //unlockAll();
         
         if (!mpCoreProtocol)
         {
            mProfileMutex.lock();
            print();
            mProfileMutex.unlock();
            return;
         }

         std::list<Profile::Uid> compls;
         CError err = mpCoreProtocol->getAvailableProfileComplements(compls);
         if (!err.isNoError())
         {
            LOG4CPLUS_ERROR(msLogger, "Error in resetProfilesState()");
         }
         LOG4CPLUS_INFO(msLogger, "After getAvailableProfileComplements, count : " + convertIntegerToString(compls.size()));
         mProfileMutex.lock();
         for (std::list<Profile::Uid>::const_iterator it = compls.begin(); compls.end() != it; ++it)
         {
            LOG4CPLUS_INFO(msLogger, "Available complement: " + it->value());
            for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator pit = mProfiles.begin(); mProfiles.end() != pit; ++pit)
            {
               LOG4CPLUS_INFO(msLogger, "*it : " + (*it).value() + ", pit->complement: " + pit->second.complement().value());
               if ((*it) == pit->second.complement())
               {
                  LOG4CPLUS_INFO(msLogger, "true");
                  pit->second.enableByComplement();
               }
            }
         }
         print();
         mProfileMutex.unlock();
      }

      void CPmpCore::disableByComplementAll()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfileMutex.lock();
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
         {
            it->second.disableByComplement();
         }
         mProfileMutex.unlock();
      }

      bool CPmpCore::lock(iviLink::CUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (!mpCoreProtocol)
         {
            return false;
         }
         mProfileMutex.lock();
         std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.find(id);
         if (it == mProfiles.end())
         {
            mProfileMutex.unlock();
            bool res = lockByApiUid(id);
            return res;
         }
         if (!mpPim || mpPim->hasInstance(id))
         {
            mProfileMutex.unlock();
            return false;
         }
         else
         {
            it->second.lock();
            mProfileMutex.unlock();
            mpCoreProtocol->lock(it->second.complement());
            return true;
         }
      }

      bool CPmpCore::unlock(iviLink::CUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (!mpCoreProtocol)
         {
            return false;
         }
         mProfileMutex.lock();
         std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.find(id);
         if (it == mProfiles.end())
         {
            mProfileMutex.unlock();
            bool res = unlockByApiUid(id);
            return res;
         }
         it->second.unlock();
         mProfileMutex.unlock();
         mpCoreProtocol->unlock(it->second.complement());
         return true;
      }

      bool CPmpCore::lockByApiUid(Profile::ApiUid id)
      {
         bool res = false;
         std::list<Profile::Uid> lst;
         mProfileMutex.lock();
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.begin();
               mProfiles.end() != it; ++it)
         {
            if (it->second.api() == id && mpPim &&
                  !mpPim->hasInstance(it->second.uid()))
            {
               res = true;
               it->second.lock();
               lst.push_back(it->second.complement());
            }
         }
         mProfileMutex.unlock();
         for (std::list<Profile::ApiUid>::iterator it = lst.begin();
               lst.end() != it; ++it)
         {
            mpCoreProtocol->lock((*it));
         }
         return res;
      }

      bool CPmpCore::unlockByApiUid(Profile::ApiUid id)
      {
         bool res = false;
         mProfileMutex.lock();
         std::list<Profile::Uid> lst;
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.begin();
               mProfiles.end() != it; ++it)
         {
            if (it->second.api() == id)
            {
               res = true;
               it->second.unlock();
               lst.push_back(it->second.complement());
            }
         }
         mProfileMutex.unlock();

         for (std::list<Profile::Uid>::iterator it = lst.begin();
               lst.end() != it; ++it)
         {
            mpCoreProtocol->unlock((*it));
         }

         return res;
      }

      void CPmpCore::lockAll()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (!mpCoreProtocol)
         {
            return;
         }

         mProfileMutex.lock();
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
         {
            it->second.lock();
         }
         mProfileMutex.unlock();
         mpCoreProtocol->lockAll();
      }


      void CPmpCore::unlockAll()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (!mpCoreProtocol)
         {
            return;
         }

         mProfileMutex.lock();
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
         {
            it->second.unlock();
         }
         mProfileMutex.unlock();
         mpCoreProtocol->unlockAll();
      }

      bool CPmpCore::getProfileLibPath(Profile::Uid uid, std::string& path)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfileMutex.lock();
         std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.find(uid);
         if (it == mProfiles.end())
         {
            LOG4CPLUS_WARN(msLogger, "profile not found");
            mProfileMutex.unlock();
            return false;
         }
         else
         {
            path = it->second.library();
            LOG4CPLUS_INFO(msLogger, "path = " + path);
            mProfileMutex.unlock();
            return true;
         }
      }

      bool CPmpCore::disableByClient(iviLink::CUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_TRACE(msLogger, "id: " + id.value());
         if (!mpCoreProtocol)
         {
            return false;
         }
         mProfileMutex.lock();
         std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.find(id);
         if (it == mProfiles.end())
         {
            mProfileMutex.unlock();
            return disableApiUidByClient(id);
         }
         it->second.disableByClient();
         mProfileMutex.unlock();

         if (mpPim && mpPool)
         {
            struct Runner
            {
               struct Data : public IThreadPoolJobData
               {
                  IPmpPimToCore* pim;
                  iviLink::CUid id;
                  Data(IPmpPimToCore* pim, iviLink::CUid const& id):
                     pim(pim),
                     id(id)
                  {}
               };

               static void run(IThreadPoolJobData const* pJobData)
               {
                  LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
                  Data const* data = static_cast<Data const*>(pJobData);
                  data->pim->unloadInstances(data->id);
               }
            };

            LOG4CPLUS_INFO(msLogger, "CPmpCore::disableByClient creating job for pim");

            Runner::Data* data = new Runner::Data(mpPim, id);
            CThreadPoolJob job(Runner::run, data);
            mpPool->addJob(job);
         }

         mpCoreProtocol->disableByClient(it->second.complement());
         return true;
      }

      bool CPmpCore::enableByClient(iviLink::CUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (!mpCoreProtocol)
         {
            LOG4CPLUS_ERROR(msLogger, "no mpCoreProtocol");
            return false;
         }
         mProfileMutex.lock();
         std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.find(id);
         if (it == mProfiles.end())
         {
            mProfileMutex.unlock();
            return enableApiUidByClient(id);
         }
         it->second.enableByClient();
         mProfileMutex.unlock();
         mpCoreProtocol->enableByClient(it->second.complement());
         return true;
      }

      bool CPmpCore::disableApiUidByClient(Profile::ApiUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         bool res = false;
         mProfileMutex.lock();
         std::list<Profile::Uid> lstComp;
         std::list<Profile::Uid> lstProfs;
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.begin();
               mProfiles.end() != it; ++it)
         {
            if (it->second.api() == id)
            {
               lstProfs.push_back(it->second.uid());
               res = true;
               it->second.disableByClient();
               lstComp.push_back(it->second.complement());
            }
         }
         mProfileMutex.unlock();

         for(std::list<Profile::Uid>::iterator it = lstProfs.begin();
               lstProfs.end() != it; ++it)
         {
            mpPim->unloadInstances((*it));
         }

         for (std::list<Profile::Uid>::iterator it = lstComp.begin();
               lstComp.end() != it; ++it)
         {
            mpCoreProtocol->disableByClient((*it));
         }
         return res;
      }

      bool CPmpCore::enableApiUidByClient(Profile::ApiUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         bool res = false;
         mProfileMutex.lock();

         std::list<Profile::Uid> lst;
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.begin();
               mProfiles.end() != it; ++it)
         {
            if (it->second.api() == id)
            {
               res = true;
               it->second.enableByClient();
               lst.push_back(it->second.complement());
            }
         }
         mProfileMutex.unlock();

         for (std::list<Profile::Uid>::iterator it = lst.begin();
               lst.end() != it; ++it)
         {
            mpCoreProtocol->enableByClient((*it));
         }

         return res;
      }

      void CPmpCore::enableByClientAll()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (!mpCoreProtocol)
         {
            return;
         }
         mProfileMutex.lock();
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
         {
            it->second.enableByClient();
         }
         mProfileMutex.unlock();
         mpCoreProtocol->enableByClientAll();
      }

      void CPmpCore::onGetAvailableProfileComplements(std::list<Profile::Uid> & complements)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         complements.assign(mComplementsList.begin(), mComplementsList.end());
      }

      bool CPmpCore::onLock(iviLink::CUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfileMutex.lock();
         std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.find(id);
         if (it == mProfiles.end())
         {
            mProfileMutex.unlock();
            return false;
         }
         it->second.lock();
         mProfileMutex.unlock();
         return true;
      }

      bool CPmpCore::onUnlock(iviLink::CUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfileMutex.lock();
         std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.find(id);
         if (it == mProfiles.end())
         {
            mProfileMutex.unlock();
            return false;
         }
         it->second.unlock();
         mProfileMutex.unlock();
         return true;
      }

      void CPmpCore::onLockAll()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfileMutex.lock();
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
         {
            it->second.lock();
         }
         mProfileMutex.unlock();
      }

      void CPmpCore::onUnlockAll()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfileMutex.lock();
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
         {
            it->second.unlock();
         }
         mProfileMutex.unlock();
      }

      bool CPmpCore::onDisableByClient(iviLink::CUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfileMutex.lock();
         std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.find(id);
         if (it == mProfiles.end())
         {
            mProfileMutex.unlock();
            return false;
         }
         it->second.disableByClient();
         mProfileMutex.unlock();
         return true;
      }

      bool CPmpCore::onEnableByClient(iviLink::CUid id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfileMutex.lock();
         std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.find(id);
         if (it == mProfiles.end())
         {
            mProfileMutex.unlock();
            return false;
         }
         it->second.enableByClient();
         mProfileMutex.unlock();
         return true;
      }

      void CPmpCore::onEnableByClientAll()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfileMutex.lock();
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
         {
            it->second.enableByClient();
         }
         mProfileMutex.unlock();
      }

      void CPmpCore::onReloadProfilesFromRepository()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         reloadProfilesFromRepository();
      }

      void CPmpCore::onResetProfilesState()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         resetProfilesState();
      }

      void CPmpCore::onLostConnection()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         disableByComplementAll();
      }

      bool CPmpCore::actualCoreAndRepoState() const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         return mActualRepoState;
      }

      void CPmpCore::init(IPmpPimToCore * pPim, IPmpCoreProtocol * pCoreProtocol)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpPim = pPim;
         mpCoreProtocol = pCoreProtocol;
      }

      void CPmpCore::print() const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::string print = "*****\n";
         print += "Platform: ";
         print += mPlatform;
         std::string str = mActualRepoState ? "" : "not ";
         print += "\nCore state is " + str + "actual";
         for (std::map<Profile::Uid,CPmpCoreProfileInfo>::const_iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
         {
            print += "\n\t***";
            print += "\n\tUID :" + it->second.uid().value();
            print += "\n\tAPI : " + it->second.api().value();
            print += "\n\tComplement : " + it->second.complement().value();
            print += it->second.available() ? "\n\tAvailable" : "\n\tNot available";
            print += it->second.enabled() ? "\n\tEnabled" : "\n\tDisabled";
            print += it->second.locked() ? "\n\tLocked" : "\n\tUnlocked";
            print += "\n\tLibrary : " + it->second.library();
         }
         print += "\n*****";
         LOG4CPLUS_INFO(msLogger, print);
      }


      bool CPmpCore::getComplementary(Profile::Uid const& profUid, 
            Profile::Uid& complProfUid) const
      {
         bool res = true;

         mProfileMutex.lock();
         {
            std::map<Profile::Uid,CPmpCoreProfileInfo>::const_iterator it = mProfiles.find(profUid);
            if (it == mProfiles.end())
            {
               res = false;
            }
            else
            {
               complProfUid = it->second.complement();
            }
         }
         mProfileMutex.unlock();

         return res;
      }

      bool CPmpCore::getPapiUid(iviLink::Profile::Uid const& profUid,
         iviLink::Profile::ApiUid& papiUid) const
      {
         bool res = true;
         mProfileMutex.lock();
         {
            std::map<Profile::Uid,CPmpCoreProfileInfo>::const_iterator it = mProfiles.find(profUid);
            if (it == mProfiles.end())
            {
               res = false;
            }
            else
            {
               papiUid = it->second.api();
            }
         }
         mProfileMutex.unlock();

         return res;
      }


      bool CPmpCore::unlockAuthProfile()
      {
         return unlockByApiUid(gsAuthProfilePapiUid);
      }

      void CPmpCore::unlockBySysCtrlAll()
      {
         return unlockAll();
      }
   }

}
