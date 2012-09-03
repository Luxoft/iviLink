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
#include <algorithm>
#include <cstdio>
#include <unistd.h>
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
#include "framework/libraries/ServiceManager/CServiceManager.hpp"
#include "CService.hpp"
/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "utils/xml/pugixml.hpp"
#include "framework/libraries/AppMan/App/CAppManConnectController.hpp"
#include "framework/components/ProfileManager/PMAL/CComponentMgr.hpp"
#include "framework/components/ProfileManager/PMAL/CPMALError.hpp"

namespace iviLink
{
   namespace Service
   {

      static const char gModuleName[] = "CServiceManager";

      Logger CServiceManager::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ServicesLib.ServiceManager"));
      CServiceManager* CServiceManager::mspInstance = NULL;
      CMutex CServiceManager::msSingletonMutex;


      #ifndef ANDROID
      CServiceManager* CServiceManager::getInstance()
      #else
      CServiceManager* CServiceManager::getInstance(iviLink::Android::AppInfo appInfo)
      #endif //ANDROID
      {
         msSingletonMutex.lock();
         if (mspInstance == NULL)
         {
            #ifndef ANDROID
            mspInstance = new CServiceManager();
            #else
            mspInstance = new CServiceManager(appInfo);
            #endif //ANDROID
         }
         msSingletonMutex.unlock();
         return mspInstance;
      }

      void CServiceManager::deleteInstance()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         msSingletonMutex.lock();
         delete mspInstance;
         mspInstance = NULL;
         msSingletonMutex.unlock();
      }

      #ifndef ANDROID
      CServiceManager::CServiceManager()
         : mpClient(0)
         , mXmlPath(SERVICE_REPOSITORY)
      #else
      CServiceManager::CServiceManager(iviLink::Android::AppInfo appInfo)
         : mpClient(0)
         , mAppInfo(appInfo)
      #endif //ANDROID
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         #ifndef ANDROID
         mXmlPath += "/";
         #endif //ANDROID
         loadDb();
         #ifndef ANDROID
         CError err = initPmal();
         #else
         CError err = initPmal(mAppInfo.launchInfo);
         #endif //ANDROID
         if(!err.isNoError())
         {
            LOG4CPLUS_ERROR(msLogger,
                  "CServiceManager::CServiceManager()=> PMAL component manager creation error "
               + static_cast<std::string>(err));
         }
      }

      CServiceManager::~CServiceManager()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mpClient = 0;
         mActiveServicesMutex.lock();
         for (tServiceMap::iterator it = mActiveServices.begin();
               mActiveServices.end() != it; ++it)
         {
            it->second->unload();
            delete it->second;
         }
         mActiveServicesMutex.unlock();

         #ifndef ANDROID
         AppMan::CAppManConnectController::instance()->init(0);
         #else
         AppMan::CAppManConnectController::instance(mAppInfo.launchInfo)->init(0);
         #endif //ANDROID
         PMAL::CPMALComponentMgr::destroy();
      }

      void CServiceManager::loadDb()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         #ifndef ANDROID
         std::string path = mXmlPath + "HeadUnitServices.xml";
         #else
         std::string path = mAppInfo.serviceRepoPath + "AndroidServices.xml";
         #endif //ANDROID
         pugi::xml_document doc;

         LOG4CPLUS_TRACE(msLogger, "path = " + path);

         pugi::xml_parse_result result = doc.load_file(path.c_str());
         if (result)
         {
            pugi::xml_node device_node = doc.child("device");
            mSystemServicesMutex.lock();
            for (pugi::xml_node service_node = device_node.child("service");
                  service_node; service_node = service_node.next_sibling("service"))
            {
               if (strcmp(service_node.attribute("name").value(), "") != 0)
               {
                  mSystemServices.insert(Service::Uid(service_node.attribute("name").value()));
               }
            }
            mSystemServicesMutex.unlock();
         }
         else
         {
            LOG4CPLUS_ERROR(msLogger, "xml error: " + std::string(result.description()));
         }
      }

      #ifndef ANDROID
      CError CServiceManager::initPmal()
      #else
      CError CServiceManager::initPmal(std::string launchInfo)
      #endif //ANDROID
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         PMAL::CPMALError err = PMAL::CPMALComponentMgr::create(this, NULL);
         if (!err.isNoError())
         {
            return err;
         }
         PMAL::CPMALComponentMgr* pPmalMgr = PMAL::CPMALComponentMgr::getInstance();

         #ifndef ANDROID
         AppMan::CAppManConnectController * pAppmanCC = AppMan::CAppManConnectController::instance();
         #else
         AppMan::CAppManConnectController * pAppmanCC = AppMan::CAppManConnectController::instance(launchInfo);
         #endif //ANDROID
         if (!pAppmanCC)
         {
            return CError(1, gModuleName, CError::FATAL, "unable to get app man connect controller");
         }

         iviLink::AppMan::IAppManHandler* pHandler = pPmalMgr->getAppManHandler();
         if (!pHandler)
         {
            return CError(1, gModuleName, CError::FATAL, "unable to get app man handler");
         }

         pAppmanCC->init(pHandler);

         return CError::NoError();
      }

      void CServiceManager::registerClient(CServiceManager::IClientCallbacks* pClient)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mpClient = pClient;
      }

      void CServiceManager::registerProfileCallbacks(const Profile::ApiUid &profileApi,
            Profile::IProfileCallbackProxy* pCallbacks)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mCallbacksMutex.lock();
         mCallbacks[profileApi] = pCallbacks;
         mCallbacksMutex.unlock();
      }

      void CServiceManager::getActive(Service::ListOfUids &services)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         services.clear();
         mActiveServicesMutex.lock();
         for (tServiceMap::const_iterator it = mActiveServices.begin();
               mActiveServices.end() != it; ++it)
         {
            services.push_back(it->first);
         }
         mActiveServicesMutex.unlock();
      }

      bool CServiceManager::isActive(const Service::Uid &service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mActiveServicesMutex.lock();
         tServiceMap::const_iterator it = mActiveServices.find(service);
         bool res = mActiveServices.end() == it ? false : true;
         mActiveServicesMutex.unlock();
         return res;
      }

      bool CServiceManager::load(const Service::Uid & service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mSystemServicesMutex.lock();
         tServiceSet::const_iterator it = mSystemServices.find(service);
         if (mSystemServices.end() == it)
         {
            mSystemServicesMutex.unlock();
            LOG4CPLUS_ERROR(msLogger, "hasn't found the requested service");
            return false;
         }
         mSystemServicesMutex.unlock();
         mActiveServicesMutex.lock();
         tServiceMap::const_iterator sit = mActiveServices.find(service);
         if (mActiveServices.end() != sit)
         {
            mActiveServicesMutex.unlock();
            LOG4CPLUS_ERROR(msLogger, "The requested service is not among active!");
            return false;
         }
         mActiveServicesMutex.unlock();
         #ifndef ANDROID
         CService * pService = new CService(mXmlPath, service);
         #else
         CService * pService = new CService(mAppInfo, service);
         #endif //ANDROID
         mCallbacksMutex.lock();
         if (!pService->load(mCallbacks))
         {
            mCallbacksMutex.unlock();
            LOG4CPLUS_ERROR(msLogger, "CService couldn't load the service");
            delete pService;
            return false;
         }
         mCallbacksMutex.unlock();
         mActiveServicesMutex.lock();
         mActiveServices[service] = pService;
         mActiveServicesMutex.unlock();
         LOG4CPLUS_ERROR(msLogger, "Loaded a service just fine");
         return true;
      }

      bool CServiceManager::incomingLoad(const Service::Uid & service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mSystemServicesMutex.lock();
         tServiceSet::const_iterator it = mSystemServices.find(service);
         if (mSystemServices.end() == it)
         {
             LOG4CPLUS_ERROR( msLogger, "incomingLoad(" + service.value() + "): failed to find service");
            mSystemServicesMutex.unlock();
            return false;
         }
         mSystemServicesMutex.unlock();
         mActiveServicesMutex.lock();
         tServiceMap::const_iterator sit = mActiveServices.find(service);
         if (mActiveServices.end() != sit)
         {
             LOG4CPLUS_WARN( msLogger, "incomingLoad(" + service.value() + "): service already loaded");
            mActiveServicesMutex.unlock();
            return true; //dmi3s  #1248579 was false. should be false.
         }
         mActiveServicesMutex.unlock();
         #ifndef ANDROID
         CService * pService = new CService(mXmlPath, service);
         #else
         CService * pService = new CService(mAppInfo, service);
         #endif //ANDROID
         mCallbacksMutex.lock();
         if (!pService->incomingLoad(mCallbacks))
         {
            mCallbacksMutex.unlock();
            delete pService;
             LOG4CPLUS_ERROR( msLogger, "pService->incomingLoad(" + service.value() + ") return false");
            return false;
         }
         mCallbacksMutex.unlock();
         mActiveServicesMutex.lock();
         mActiveServices[service] = pService;
         mActiveServicesMutex.unlock();
         return true;
      }

      bool CServiceManager::unload(const Service::Uid &service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mActiveServicesMutex.lock();
         tServiceMap::iterator it = mActiveServices.find(service);
         if (mActiveServices.end() == it)
         {
            mActiveServicesMutex.unlock();
            return false;
         }
         CService * pService = it->second;
         pService->unload();
         if (!pService->hasUnloadWaits())
         {
            mActiveServices.erase(it);
            delete pService;
         }
         mActiveServicesMutex.unlock();
         return true;
      }

      CError CServiceManager::serviceRequestCallback(iviLink::Service::Uid const& service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         if (mpClient)
         {
            mpClient->serviceRequestCallback(service);
         }
         if (!incomingLoad(service))
         {
            if (mpClient)
            {
                LOG4CPLUS_ERROR(msLogger, service.value() + ": " + "Failed to load incoming service"  );
               mpClient->serviceLoadError(service);
            }
            return CError(1, gModuleName, CError::ERROR, "Can't load incoming service");
         }
         return CError::NoError();
      }

      CError CServiceManager::incomingProfile(Service::Uid const& service,
            Profile::Uid const& profile, Profile::ApiUid const& api)
      {
         LOG4CPLUS_TRACE(msLogger, "CServiceManager::incomingProfileRequest()=> requested  profile "
            + profile.value() +"for service"+service.value() );

         mActiveServicesMutex.lock();
         tServiceMap::iterator it = mActiveServices.find(service);
         if (mActiveServices.end() == it)
         {
            mActiveServicesMutex.unlock();
            return CError(1,"ServiceManager",CError::ERROR,"Can't load incoming profile");
         }
         ERROR_CODE err = it->second->incomingProfileRequest(profile,api);
         bool allLoaded = it->second->allLoaded();
         mActiveServicesMutex.unlock();

         if (ERR_OK != err)
         {
            if (mpClient)
            {
                LOG4CPLUS_ERROR(msLogger, service.value() + ": error " + convertIntegerToString(int(err)) +
                                "in incomingProfileRequest(" + profile.value() + ","+ api.value() + ")");
               mpClient->serviceLoadError(service);
            }
            unload(service);
            return CError(1,gModuleName, CError::ERROR, "incomingProfile-Error");
         }
         if (allLoaded && mpClient)
         {
            mpClient->serviceLoaded(service);
         }
         return CError::NoError(gModuleName, "incomingProfile-No_Error");
      }

      void CServiceManager::serviceDropped(iviLink::Service::Uid const&)
      {
         // not used
      }

      void CServiceManager::profileDied(Service::Uid const& service,
            Profile::IUid const& profileInstance )
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mActiveServicesMutex.lock();
         tServiceMap::iterator it = mActiveServices.find(service);
         if(mActiveServices.end() == it)
         {
            mActiveServicesMutex.unlock();
            return;
         }
         if (it->second->correctState() && mpClient)
         {
            mpClient->serviceDropped(service);
         }
         it->second->profileDied(profileInstance);
         if (it->second->allDied())
         {
            CService * pService = it->second;
            mActiveServices.erase(it);
            mActiveServicesMutex.unlock();
            delete pService;
         }
         else
         {
            mActiveServicesMutex.unlock();
         }
      }

      CProfileApiBase* CServiceManager::getProfile(const Service::Uid &service, const Profile::ApiUid &profileApi)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mActiveServicesMutex.lock();
         tServiceMap::iterator it = mActiveServices.find(service);
         if (mActiveServices.end() == it)
         {
            mActiveServicesMutex.unlock();
            return 0;
         }
         CProfileApiBase * pProfile = it->second->getProfile(profileApi);
         mActiveServicesMutex.unlock();
         return pProfile;
      }

      void CServiceManager::releaseProfile(const Service::Uid &service, const Profile::ApiUid &profileApi)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mActiveServicesMutex.lock();
         tServiceMap::iterator it = mActiveServices.find(service);
         if (mActiveServices.end() != it)
         {
            it->second->releaseProfile(profileApi);
            if (it->second->allDied())
            {
               CService * pService = it->second;
               pService->unload();
               if (!pService->hasUnloadWaits())
               {
                  mActiveServices.erase(it);
                  delete pService;
               }

            }
         }
         mActiveServicesMutex.unlock();
      }
   }
}
