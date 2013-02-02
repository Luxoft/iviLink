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
#include "CServiceManager.hpp"
#include "CService.hpp"
/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "pugixml.hpp"
#include "CAppManConnectController.hpp"
#include "CComponentMgr.hpp"
#include "CPMALError.hpp"

namespace iviLink
{
   namespace Service
   {

      static const char gModuleName[] = "CServiceManager";

      Logger CServiceManager::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ServicesLib.ServiceManager"));
      CServiceManager* CServiceManager::mspInstance = NULL;
      CMutex CServiceManager::msSingletonMutex;

      CServiceManager* CServiceManager::getInstance(iviLink::Android::AppInfo appInfo)
      {
         msSingletonMutex.lock();
         if (mspInstance == NULL)
         {
            mspInstance = new CServiceManager(appInfo);
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

      CServiceManager::CServiceManager(iviLink::Android::AppInfo appInfo)
         : mpClient(0)
#ifndef ANDROID
         , mXmlPath((std::string)SERVICE_REPOSITORY)
#else 
         , mXmlPath(appInfo.serviceRepoPath)
#endif //ANDROID
         , mAppInfo(appInfo)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         loadDb();
         
         BaseError err = initPmal();
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

         AppMan::CAppManConnectController::instance(mAppInfo)->init(0);
         PMAL::CPMALComponentMgr::destroy();
      }

      void CServiceManager::loadDb()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         std::string path = mXmlPath 
#ifndef ANDROID
                           + "HeadUnitServices.xml";
#else
                           + "AndroidServices.xml";
#endif //ANDROID
         pugi::xml_document doc;

         LOG4CPLUS_INFO(msLogger, "path = " + path);

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
            LOG4CPLUS_FATAL(msLogger, "xml error: " + std::string(result.description()));
         }
      }

      BaseError CServiceManager::initPmal()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

         AppMan::CAppManConnectController * pAppmanCC
                       = AppMan::CAppManConnectController::instance(mAppInfo);

         PMAL::CPMALError err = PMAL::CPMALComponentMgr::create(this, NULL);
         if (!err.isNoError())
         {
            return err;
         }
         PMAL::CPMALComponentMgr* pPmalMgr = PMAL::CPMALComponentMgr::getInstance();

         if (!pAppmanCC)
         {
            return BaseError(1, gModuleName, BaseError::IVILINK_FATAL, "unable to get app man connect controller");
         }

         iviLink::AppMan::IAppManHandler* pHandler = pPmalMgr->getAppManHandler();
         if (!pHandler)
         {
            LOG4CPLUS_WARN(msLogger, "unable to get app man handler");
            return BaseError(1, gModuleName, BaseError::IVILINK_FATAL, "unable to get app man handler");
         }

         pAppmanCC->init(pHandler);

         return BaseError::NoError();
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

      Error CServiceManager::load(const Service::Uid & service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mSystemServicesMutex.lock();
         tServiceSet::const_iterator it = mSystemServices.find(service);
         if (mSystemServices.end() == it)
         {
            mSystemServicesMutex.unlock();
            LOG4CPLUS_ERROR(msLogger, "hasn't found the requested service");
            return Error(SERVICE_NOT_FOUND, "The service requested: " + service.value() 
               + " was not found among system services, please check that it is present in " + mXmlPath, false);
         }
         mSystemServicesMutex.unlock();
         mActiveServicesMutex.lock();
         tServiceMap::const_iterator sit = mActiveServices.find(service);
         if (mActiveServices.end() != sit)
         {
            mActiveServicesMutex.unlock();
            LOG4CPLUS_ERROR(msLogger, "The requested service is among active!");
            return Error(SERVICE_ALREADY_LOADED, "The service requested: " + service.value() 
               + " is already loaded!", false);
         }
         mActiveServicesMutex.unlock();
         CService * pService = new CService(mXmlPath, mAppInfo, service);
         mCallbacksMutex.lock();
         Error cserviceError = pService->load(mCallbacks);
         if (!cserviceError.isNoError())
         {
            mCallbacksMutex.unlock();
            LOG4CPLUS_ERROR(msLogger, "CService couldn't load the service: " + cserviceError.toString());
            delete pService;
            return cserviceError;
         }
         mCallbacksMutex.unlock();
         mActiveServicesMutex.lock();
         mActiveServices[service] = pService;
         mActiveServicesMutex.unlock();
         LOG4CPLUS_TRACE(msLogger, "Loaded a service successfully");
         return Error::NoError();
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
         if(isServiceAlreadyLoaded(service)) 
         {
            return true;
         }
         CService * pService = new CService(mXmlPath, mAppInfo, service);
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

      BaseError CServiceManager::serviceRequestCallback(iviLink::Service::Uid const& service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         if (mpClient)
         {
            if(!isServiceAlreadyLoaded(service))
            {
               mpClient->serviceRequestCallback(service);
            }
         }
         if (!incomingLoad(service))
         {
            if (mpClient)
            {
                LOG4CPLUS_ERROR(msLogger, service.value() + ": " + "Failed to load incoming service"  );
               mpClient->serviceLoadError(service);
            }
            return BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "Can't load incoming service");
         }
         return BaseError::NoError();
      }

      BaseError CServiceManager::incomingProfile(Service::Uid const& service,
            Profile::Uid const& profile, Profile::ApiUid const& api)
      {
         LOG4CPLUS_TRACE(msLogger, "CServiceManager::incomingProfileRequest()=> requested  profile "
            + profile.value() +"for service"+service.value() );

         mActiveServicesMutex.lock();
         tServiceMap::iterator it = mActiveServices.find(service);
         if (mActiveServices.end() == it)
         {
            mActiveServicesMutex.unlock();
            return BaseError(1,"ServiceManager",BaseError::IVILINK_ERROR,"Can't load incoming profile");
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
            return BaseError(1,gModuleName, BaseError::IVILINK_ERROR, "incomingProfile-Error");
         }
         if (allLoaded && mpClient)
         {
            mpClient->serviceLoaded(service);
         }
         return BaseError::NoError(gModuleName, "incomingProfile-No_Error");
      }

      void CServiceManager::serviceDropped(iviLink::Service::Uid const&)
      {
         // not used
      }

      void CServiceManager::profileDied(Service::Uid const& service,
            Profile::IUid const& profileInstance )
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         LOG4CPLUS_WARN(msLogger, "service dropped: "+service.value());
         mActiveServicesMutex.lock();
         tServiceMap::iterator it = mActiveServices.find(service);
         Service::Uid complementedService = service;
         bool hasFound = false;
         if(mActiveServices.end() == it)
         {
            LOG4CPLUS_INFO(msLogger, "service was not among active, looking for its complement");
            for(it = mActiveServices.begin(); it != mActiveServices.end(); it++)
            {
               LOG4CPLUS_INFO(msLogger, "service: "+it->second->getUid().value());
               LOG4CPLUS_INFO(msLogger, "service's complement: "+it->second->getPairUid().value());
               if(it->second->getPairUid().value().compare(service.value()) == 0)
               {
                  LOG4CPLUS_INFO(msLogger, "complement dropped: "+service.value());
                  LOG4CPLUS_INFO(msLogger, "of: "+it->second->getUid().value());
                  complementedService = it->second->getUid().value();
                  hasFound = true;
                  break;
               } else {
                  LOG4CPLUS_TRACE(msLogger, "service dropped and current service's complement are not equal");
               }
            }
         } else {
            hasFound = true;
         }
         if(!hasFound)
         {  
            LOG4CPLUS_ERROR(msLogger, "The service that was dropped was now found among mActiveServices and their complements");
            mActiveServicesMutex.unlock();
            return;
         }
         if (it->second->correctState() && mpClient)
         {
            mpClient->serviceDropped(complementedService);
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

      BaseProfileApi* CServiceManager::getProfile(const Service::Uid &service, const Profile::ApiUid &profileApi)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mActiveServicesMutex.lock();
         tServiceMap::iterator it = mActiveServices.find(service);
         if (mActiveServices.end() == it)
         {
            mActiveServicesMutex.unlock();
            LOG4CPLUS_WARN(msLogger, "the service is not among active: " + service.value());
            LOG4CPLUS_INFO(msLogger, "List of active services");
            for (tServiceMap::iterator it = mActiveServices.begin();
               mActiveServices.end() != it; ++it)
            {
               LOG4CPLUS_INFO(msLogger, it->second->getUid().value());
            }
            return 0;
         }
         BaseProfileApi * pProfile = it->second->getProfile(profileApi);
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


      bool CServiceManager::isServiceAlreadyLoaded(const Service::Uid & service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         MutexLocker lock(mActiveServicesMutex);
         tServiceMap::const_iterator sit = mActiveServices.find(service);
         if (mActiveServices.end() != sit)
         {
            LOG4CPLUS_WARN( msLogger, "incomingLoad(" + service.value() + "): service already loaded");
            return true; //dmi3s  #1248579 was false. should be false.
         }
         return false;
      }
   }
}
