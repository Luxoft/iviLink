/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#include <cassert>
#include <iostream>
#include <string>

#include "CAppManProxy.hpp"
#include "CServManProxy.hpp"

#include "CApp.hpp"
#include "CServiceManager.hpp"
#include "CLogger.hpp"

 #include "API.hpp"

namespace iviLink
{
   using Service::CServiceManager;

   CApp::CApp(iviLink::Android::AppInfo appInfo)
      : mpAppManProxy(0)
      , mpServManProxy(0)
      , mAppInfo(appInfo)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
   }

   CApp::CApp(const Service::Uid &service, iviLink::Android::AppInfo appInfo)
      : mpAppManProxy(0)
      , mpServManProxy(0)
      , mAppInfo(appInfo)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      mSupportedServices.push_back(service);
   }

   CApp::CApp(const Service::ListOfUids &services, iviLink::Android::AppInfo appInfo)
      : mpAppManProxy(0)
      , mpServManProxy(0)
      , mSupportedServices(services)
      , mAppInfo(appInfo)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
   }

   void CApp::initInIVILink()
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);

      mpAppManProxy = new App::CAppManProxy(this, mAppInfo);
      mpServManProxy = new App::CServManProxy(this);
      CServiceManager::getInstance(mAppInfo)->registerClient(mpServManProxy);

      mpAppManProxy->startProxyThread();
      mpAppManProxy->init(mSupportedServices);
   }

   CApp::~CApp()
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      CServiceManager::deleteInstance();
      mpAppManProxy->finish();
      delete mpServManProxy;
      delete mpAppManProxy;

#ifndef ANDROID
#else
      /*
       * Deinit of ConnectivityAgent fixes segfault of BasicSample on SystemController shutdown
      */
      ::deinitConnectivityAgentProxy();
#endif
   }

   void CApp::setEnabled(const Service::Uid &service, bool enable)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      if (mpAppManProxy)
      {
         mpAppManProxy->useService(service, enable);
      }
      else
      {
         LOG4CPLUS_WARN(*App::CLogger::logger(),"AppManProxy doesn't exist. App is not inited!!!");
      }
   }

   void CApp::setEnabled(const Service::ListOfUids &services, bool enable)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      if (mpAppManProxy)
      {
         for (Service::ListOfUids::const_iterator it = services.begin();
               services.end() != it; ++it)
         {
            mpAppManProxy->useService(*it, enable);
         }
      }
      else
      {
         LOG4CPLUS_WARN(*App::CLogger::logger(),"AppManProxy doesn't exist. App is not inited!!!");
      }
   }

   ELaunchInfo CApp::getLaunchInfo()
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      if (mpAppManProxy)
      {
         return mpAppManProxy->launcher();
      }
      else
      {
         return ERROR_APP_NOT_INITED_IN_IVILINK;
      }
   }

   void CApp::registerProfileCallbacks(const Profile::ApiUid &profileApi, Profile::IProfileCallbackProxy* pCallbacks)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      if (mpServManProxy)
      {
         CServiceManager::getInstance(mAppInfo)->registerProfileCallbacks(profileApi, pCallbacks);
      }
      else
      {
         LOG4CPLUS_WARN(*App::CLogger::logger(),"ServManProxy doesn't exist. App is not inited!!!");
      }
   }

   bool CApp::loadService(const Service::Uid &service)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      if (mpServManProxy)
      {
         return CServiceManager::getInstance(mAppInfo)->load(service);
      }
      else
      {
         LOG4CPLUS_WARN(*App::CLogger::logger(),"ServManProxy doesn't exist. App is not inited!!!");
      }
   }

   void CApp::unloadService(const Service::Uid &service)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      if (mpServManProxy)
      {
         CServiceManager::getInstance(mAppInfo)->unload(service);
      }
      else
      {
         LOG4CPLUS_WARN(*App::CLogger::logger(),"ServManProxy doesn't exist. App is not inited!!!");
      }
   }

   Service::ListOfUids CApp::getActiveServices()
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      Service::ListOfUids list;
      if (mpServManProxy)
      {
         CServiceManager::getInstance(mAppInfo)->getActive(list);
      }
      else
      {
         LOG4CPLUS_WARN(*App::CLogger::logger(),"ServManProxy doesn't exist. App is not inited!!!");
      }
      return list;
   }

   bool CApp::isActive(const Service::Uid &service)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      if (mpServManProxy)
      {
         return CServiceManager::getInstance(mAppInfo)->isActive(service);
      }
      else
      {
         LOG4CPLUS_WARN(*App::CLogger::logger(),"ServManProxy doesn't exist. App is not inited!!!");
      }
   }

   void CApp::onInitDone(ELaunchInfo /*launcher*/)
   {
      LOG4CPLUS_WARN(*App::CLogger::logger(),__PRETTY_FUNCTION__);
   }

   void CApp::onIncomingServiceBeforeLoading(const Service::Uid & /*service*/)
   {
      LOG4CPLUS_WARN(*App::CLogger::logger(),__PRETTY_FUNCTION__);
   }

   void CApp::onIncomingServiceAfterLoading(const Service::Uid &/*service*/)
   {
      LOG4CPLUS_WARN(*App::CLogger::logger(),__PRETTY_FUNCTION__);
   }

   void CApp::onServiceLoadError(const Service::Uid & /*service*/)
   {
      LOG4CPLUS_WARN(*App::CLogger::logger(),__PRETTY_FUNCTION__);
   }

   void CApp::onServiceDropped(const Service::Uid & /*service*/)
   {
      LOG4CPLUS_WARN(*App::CLogger::logger(),__PRETTY_FUNCTION__);
   }

    void CApp::onLinkUp()
    {
      LOG4CPLUS_WARN(*App::CLogger::logger(),__PRETTY_FUNCTION__);
    }

    void CApp::onPhysicalLinkDown()
    {
      LOG4CPLUS_WARN(*App::CLogger::logger(),__PRETTY_FUNCTION__);
    }

    bool CApp::isLinkAlive()
    {
       if (mpAppManProxy)
       {
        return mpAppManProxy->isLinkAlive();
       }
       else
       {
          LOG4CPLUS_WARN(*App::CLogger::logger(),"AppManProxy doesn't exist. App is not inited!!!");
          return false;
       }
    }


}
