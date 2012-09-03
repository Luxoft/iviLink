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










/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <assert.h>
#include <cstring>
#include <list>
#include "stdio.h"
/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/


/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CService.hpp"
/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/

#include "utils/xml/pugixml.hpp"
#include "utils/threads/CMutex.hpp"
#include "framework/libraries/ServiceManager/CServiceManager.hpp"
#include "framework/components/ProfileManager/PMAL/CComponentMgr.hpp"
#include "framework/components/ProfileManager/PMAL/IPMALCore.hpp"
#include "framework/components/ProfileManager/PMAL/IPMALPIM.hpp"

namespace iviLink
{
   namespace Service
   {

      Logger CService::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ServicesLib.GenericService"));
      
      #ifndef ANDROID
      CService::CService(const std::string &xmlPath, const Service::Uid & service)
         : mXmlPath(xmlPath)
         , mUid(service)
         , mpProfilesMapMutex(new CMutex())
         , mpCbMapMutex(new CMutex())
         , mCorrect(true)
      #else
      CService::CService(iviLink::Android::AppInfo appInfo, const Service::Uid & service)
         : mAppInfo(appInfo)
         , mUid(service)
         , mpProfilesMapMutex(new CMutex())
         , mpCbMapMutex(new CMutex())
         , mCorrect(true)
      #endif //ANDROID
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
      }

      CService::~CService()
      {
         unload();
         delete mpProfilesMapMutex;
         delete mpCbMapMutex;
      }

      const Service::Uid & CService::getUid() const
      {
         return mUid;
      }

      bool CService::load(tCallbacksMap & cbMap)
      {
         #ifndef ANDROID
         mManifestPath = mXmlPath + mUid.value() + ".xml";
         #else
         mManifestPath = mAppInfo.serviceRepoPath + mUid.value() + ".xml";
         #endif //ANDROID
         LOG4CPLUS_TRACE(msLogger, "CService::parseServiceXML filename = '" +
            mManifestPath + "'");

         pugi::xml_document doc;
         pugi::xml_parse_result result = doc.load_file(mManifestPath.c_str());
         if (result)
         {
            std::string service_name(doc.child("service").attribute("name").value());
            LOG4CPLUS_TRACE(msLogger, "CService::parseServiceXML()" + service_name);
            mPairUid = doc.child("service").child("pair").attribute("name").value();

            for (pugi::xml_node profile_node = doc.child("service").child("profile"); profile_node;
                  profile_node = profile_node.next_sibling("profile"))
            {
               Profile::ApiUid profile_api_UID(profile_node.attribute("api").value());
               if (profile_api_UID.value() == "")
               {
                  LOG4CPLUS_ERROR(msLogger, "CService::load() manifest error");
                  return false;
               }

               std::map<std::string,std::string> params;
               for (pugi::xml_node param_node = profile_node.child("param"); param_node; param_node =
                     param_node.next_sibling("param"))
               {
                  std::string param_name(param_node.attribute("name").value());
                  std::string param_val(param_node.attribute("lowbound").value());
                  param_val+=";" + (std::string)param_node.attribute("upbound").value() + ";"
                        + (std::string)param_node.attribute("optimal").value();
                  params.insert(std::pair<std::string ,std::string >(param_name, param_val));
               }

               PMAL::IPMALCore* pCore = PMAL::CPMALComponentMgr::getInstance()->getCore();
               std::list<Profile::Uid> profiles;
               PMAL::CPMALError err = pCore->findProfiles(profile_api_UID, params, profiles, true);
               if (!err.isNoError())
               {
                  LOG4CPLUS_ERROR(msLogger, "CService::parseServiceXML() error getting profile for api='"
                     + profile_api_UID.value()
                     + "' error: " + static_cast<std::string>(err));
                  return false;
               }
               if (profiles.empty())
               {
                  LOG4CPLUS_ERROR(msLogger, "CService::parseServiceXML() no profiles for api='"
                     + profile_api_UID.value() + "'");
                  return false;
               }

               PMAL::IPMALPIM* pPim = PMAL::CPMALComponentMgr::getInstance()->getPIM();
               Profile::Uid prfUid = profiles.front();
               profiles.clear();

               Profile::CProfile * pProfile = NULL;
               tCallbacksMap::iterator iter = cbMap.find(profile_api_UID);
               if(iter != cbMap.end())
               {
                  LOG4CPLUS_TRACE(msLogger, "CService::parseServiceXML() callbacks found, loding profile....");
                  Profile::IProfileCallbackProxy* pCallback = iter->second;
                  #ifndef ANDROID
                  err = pPim->loadProfile(prfUid, mPairUid, pCallback, pProfile);
                  #else
                  err = pPim->loadProfile(prfUid, mPairUid, pCallback, pProfile, mAppInfo.privateDirPath);
                  #endif //ANDROID
                  if (err.isNoError())
                  {
                     mpProfilesMapMutex->lock();
                     mProfilesMap[profile_api_UID] = pProfile;
                     mpProfilesMapMutex->unlock();
                  }
                  else
                  {
                     LOG4CPLUS_ERROR(msLogger, "CService::parseServiceXML() unable to load profile, error: "
                        + static_cast<std::string>(err));
                     return false;
                  }
               }
               else
               {
                  LOG4CPLUS_ERROR(msLogger, "CService::parseServiceXML() callbacks not found for profile api '"
                     + profile_api_UID.value() +"' !");
                  return false;
               }
            }
         }
         else
         {
            LOG4CPLUS_ERROR(msLogger, "CService::parseServiceXML() XML parsing error! " +
               std::string(result.description()));
            return false;;
         }
         return true;
      }

      void CService::unload()
      {
         mpProfilesMapMutex->lock();
         std::list<Service::Uid> eraseList;
         for(tProfilesMap::iterator it =mProfilesMap.begin();
               mProfilesMap.end() != it; ++it)
         {
            if (it->second->isLocked())
            {
               mUnloadRequests.insert(it->second->profileInstance()->getProfileApiUid());
            }
            else
            {
               PMAL::IPMALPIM* pPim = PMAL::CPMALComponentMgr::getInstance()->getPIM();
               PMAL::CPMALError err = pPim->unloadProfile(it->second);
               eraseList.push_back(it->first);
            }
         }
         for (std::list<Service::Uid>::iterator it = eraseList.begin();
               eraseList.end() != it; ++it)
         {
            mProfilesMap.erase(*it);
         }
         mpProfilesMapMutex->unlock();
      }

      bool CService::incomingLoad(tCallbacksMap & cbMap)
      {
         #ifndef ANDROID
         mManifestPath = mXmlPath + mUid.value() + ".xml";
         #else
         mManifestPath = mAppInfo.serviceRepoPath + mUid.value() + ".xml";
         #endif //ANDROID
         LOG4CPLUS_TRACE(msLogger, "CService::parseServiceXML filename = '" +
            mManifestPath + "'");

         pugi::xml_document doc;
         pugi::xml_parse_result result = doc.load_file(mManifestPath.c_str());
         if (result)
         {
            std::string service_name(doc.child("service").attribute("name").value());
            LOG4CPLUS_TRACE(msLogger, "CService::incomingLoad()" + service_name);
            mPairUid = doc.child("service").child("pair").attribute("name").value();

            for (pugi::xml_node profile_node = doc.child("service").child("profile"); profile_node;
                  profile_node = profile_node.next_sibling("profile"))
            {
               Profile::ApiUid profile_api_UID(profile_node.attribute("api").value());
               if (profile_api_UID.value() == "")
               {
                  LOG4CPLUS_ERROR(msLogger, "CService::incomingLoad() manifest error");
                  return false;
               }

               mpProfilesMapMutex->lock();
               mProfilesMap[profile_api_UID] = 0;
               mpProfilesMapMutex->unlock();

               tCallbacksMap::iterator fit = cbMap.find(profile_api_UID);
               if (cbMap.end() == fit)
               {
                  LOG4CPLUS_ERROR(msLogger, "CService::incomingLoad() No callback");
                  return false;
               }
               else if (!fit->second)
               {
                  LOG4CPLUS_ERROR(msLogger, "CService::incomingLoad() Null callback");
                  return false;
               }
               else
               {
                  mpCbMapMutex->lock();
                  mCbMap[profile_api_UID] = fit->second;
                  mpCbMapMutex->unlock();
               }
            }
         }
         else
         {
            LOG4CPLUS_ERROR(msLogger, "CService::incomingLoad() XML parsing error! " +
               std::string(result.description()));
            return false;;
         }
         return true;
      }
      
      ERROR_CODE CService::incomingProfileRequest( iviLink::Profile::Uid const& uid,
            iviLink::Profile::ApiUid const& papiUid)
      {
         LOG4CPLUS_TRACE(msLogger, "CService::incomingProfileRequest() " + uid.value()
            + " api = " + papiUid.value());
         ERROR_CODE ret = ERR_FAIL;
         #ifndef ANDROID
         std::string path = mXmlPath + mUid.value() + ".xml";
         #else
         std::string path = mAppInfo.serviceRepoPath + mUid.value() + ".xml";
         #endif //ANDROID
         pugi::xml_document doc;
         pugi::xml_parse_result result = doc.load_file(path.c_str());
         if (result)
         {
            std::map<std::string,std::string> params;
            if(mPairUid.empty())
            {
               mPairUid = doc.child("service").child("pair").attribute("name").value();
            }
            pugi::xml_node profile_node = doc.child("service").find_child_by_attribute("profile",
                  "api",papiUid.value().c_str());
            Profile::ApiUid profile_api(profile_node.attribute("api").value());

            CError err = CError::NoError();
            PMAL::IPMALPIM* pPim = PMAL::CPMALComponentMgr::getInstance()->getPIM();
            assert(pPim);
            iviLink::Profile::Uid const& prfUid = uid;

            Profile::CProfile * pProfile = NULL;
            mpCbMapMutex->lock();
            tCallbacksMap::iterator iter = mCbMap.find(papiUid);
            if(iter !=mCbMap.end())
            {
               mpCbMapMutex->unlock();
               LOG4CPLUS_TRACE(msLogger,
                     "CService::incomingProfileRequest() callbacks found, loding profile....");
               iviLink::Profile::IProfileCallbackProxy* pCallback = iter->second;
               #ifndef ANDROID
               err = pPim->loadProfile(prfUid, mUid, pCallback, pProfile);
               #else
               err = pPim->loadProfile(prfUid, mUid, pCallback, pProfile, mAppInfo.privateDirPath);
               #endif //ANDROID
               mpProfilesMapMutex->lock();
               mProfilesMap[profile_api] = pProfile;
               mpProfilesMapMutex->unlock();
               ret = ERR_OK;
            }
            else
            {
               mpCbMapMutex->unlock();
               LOG4CPLUS_ERROR(msLogger, "CService::incomingProfileRequest() callbacks not found!");
               ret = ERR_NOTFOUND;
            }
         }
         else
         {
            LOG4CPLUS_ERROR(msLogger, "CService::incomingProfileRequest() XML parsing error!");
            ret = ERR_XML_PARSING;
         }
         return ret;
      }

      void CService::profileDied(const Profile::IUid &instance)
      {
         mpProfilesMapMutex->lock();
         for (tProfilesMap::iterator it = mProfilesMap.begin();
               mProfilesMap.end() != it; ++it)
         {
            if (it->second->getProfileID() == instance)
            {
               mCorrect = false;
               if (it->second->isLocked())
               {
                  mUnloadRequests.insert(it->second->profileInstance()->getProfileApiUid());
               }
               else
               {
                  PMAL::IPMALPIM* pPim = PMAL::CPMALComponentMgr::getInstance()->getPIM();
                  PMAL::CPMALError err = pPim->unloadProfile(it->second);
                  mProfilesMap.erase(it);
                  mpProfilesMapMutex->unlock();
               }
               return;
            }
         }
         mpProfilesMapMutex->unlock();
      }

      bool CService::allLoaded() const
      {
         mpProfilesMapMutex->lock();
         for (tProfilesMap::const_iterator it = mProfilesMap.begin();
               mProfilesMap.end() != it; ++it)
         {
            if (!it->second)
            {
               mpProfilesMapMutex->unlock();
               return false;
            }
         }
         mpProfilesMapMutex->unlock();
         return true;
      }

      bool CService::allDied() const
      {
         mpProfilesMapMutex->lock();
         bool res = mProfilesMap.empty();
         mpProfilesMapMutex->unlock();
         return res;
      }

      CProfileApiBase * CService::getProfile(const Profile::ApiUid &profileApi)
      {
         mpProfilesMapMutex->lock();
         tProfilesMap::iterator it = mProfilesMap.find(profileApi);
         if(it != mProfilesMap.end())
         {
            if (it->second)
            {
               CProfileApiBase * api = it->second->profileInstance();
               mpProfilesMapMutex->unlock();
               return api;
            }
            else
            {
               mpProfilesMapMutex->unlock();
               return NULL;
            }
         }
         else
         {
            mpProfilesMapMutex->unlock();
            return NULL;
         }
      }

      void CService::releaseProfile(const Profile::ApiUid &profileApi)
      {
         mpProfilesMapMutex->lock();
         tProfilesMap::iterator it = mProfilesMap.find(profileApi);
         if(it != mProfilesMap.end() && it->second)
         {
            bool unload = mUnloadRequests.end() !=
                  mUnloadRequests.find(it->second->profileInstance()->getProfileApiUid());
            it->second->release();
            if (unload)
            {
               PMAL::IPMALPIM* pPim = PMAL::CPMALComponentMgr::getInstance()->getPIM();
               PMAL::CPMALError err = pPim->unloadProfile(it->second);
               mProfilesMap.erase(it);
            }
         }
         mpProfilesMapMutex->unlock();
      }

      bool CService::correctState() const
      {
         return mCorrect;
      }

      bool CService::hasUnloadWaits() const
      {
         return ! mUnloadRequests.empty();
      }

   }
}
