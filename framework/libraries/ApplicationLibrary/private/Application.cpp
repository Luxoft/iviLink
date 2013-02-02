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
#include <iostream>
#include <string>

#include "CAppManProxy.hpp"
#include "CServManProxy.hpp"
#include "InternalThread.hpp"

#include "Application.hpp"
#include "CServiceManager.hpp"
#include "CAppManConnectController.hpp"

 #include "API.hpp"

namespace iviLink
{
   using Service::CServiceManager;

   static std::map<Profile::ApiUid, Profile::IProfileCallbackProxy*> gCallbacks;

   static Logger gCAppLogger = Logger::getInstance(LOG4CPLUS_TEXT("appLib"));

   Application::Application(iviLink::Android::AppInfo appInfo)
      : mpAppManProxy(0)
      , mpServManProxy(0)
      , mAppInfo(appInfo)
      , mInternalThread(NULL)
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);
   }

   Application::Application(const Service::Uid &service, iviLink::Android::AppInfo appInfo)
      : mpAppManProxy(0)
      , mpServManProxy(0)
      , mAppInfo(appInfo)
      , mInternalThread(NULL)
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);
      mSupportedServices.push_back(service);
   }

   Application::Application(const Service::ListOfUids &services, iviLink::Android::AppInfo appInfo)
      : mpAppManProxy(0)
      , mpServManProxy(0)
      , mSupportedServices(services)
      , mAppInfo(appInfo)
      , mInternalThread(NULL)
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);
   }

   void Application::initInIVILink()
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);
      mInternalThread = new App::InternalThread(this);
      mInternalThread->startInternalThread();
   }

   Application::~Application()
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);
      mInternalThread->stopInternalThread();
      CServiceManager::deleteInstance();
      if(mpAppManProxy)
      {
        mpAppManProxy->finish();
      }
      delete mpServManProxy;
      delete mpAppManProxy;
      delete mInternalThread;

#ifndef ANDROID
#else
      /*
       * Deinit of ConnectivityAgent fixes segfault of BasicSample on SystemController shutdown
      */
      ::deinitConnectivityAgentProxy();
#endif
   }

   void Application::setEnabled(const Service::Uid &service, bool enable)
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);
      if (mpAppManProxy)
      {
         mpAppManProxy->useService(service, enable);
      }
      else
      {
         LOG4CPLUS_WARN(gCAppLogger,"AppManProxy doesn't exist. App is not inited!!!");
      }
   }

   void Application::setEnabled(const Service::ListOfUids &services, bool enable)
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);
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
         LOG4CPLUS_WARN(gCAppLogger,"AppManProxy doesn't exist. App is not inited!!!");
      }
   }

   ELaunchInfo Application::getLaunchInfo()
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);
      if (mpAppManProxy)
      {
         return mpAppManProxy->launcher();
      }
      else
      {
         return ERROR_APP_NOT_INITED_IN_IVILINK;
      }
   }

   void Application::registerProfileCallbacks(const Profile::ApiUid &profileApi, Profile::IProfileCallbackProxy* pCallbacks)
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);

      if (mpServManProxy)
      {
         CServiceManager::getInstance(mAppInfo)->registerProfileCallbacks(profileApi, pCallbacks);
      }
      else
      {
          LOG4CPLUS_INFO(gCAppLogger, "Save callbacks");
          gCallbacks[profileApi] = pCallbacks;
      }
   }

   Error Application::loadService(const Service::Uid &service)
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);
      if (mpServManProxy)
      {
         return CServiceManager::getInstance(mAppInfo)->load(service);
      }
      LOG4CPLUS_WARN(gCAppLogger,"ServManProxy doesn't exist. App is not inited!");
      return Error(INITIALIZATION_ERROR, "Service Manager Proxy does not exist, meaning this Application hasn't executed initInIVILink yet!",false);
   }

   Error Application::unloadService(const Service::Uid &service)
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);
      if (mpServManProxy)
      {
         if(CServiceManager::getInstance(mAppInfo)->unload(service))
         {
            return Error::NoError();
         }
         else
         {
            LOG4CPLUS_ERROR(gCAppLogger,"Unable to unload the service for some reason!!!");
            return Error(UNKNOWN_ERROR, "Unable to unload the service for some reason",false);
         }
      }
      else
      {
         LOG4CPLUS_WARN(gCAppLogger,"ServManProxy doesn't exist. App is not inited!!!");
         return Error(INITIALIZATION_ERROR, "Service Manager Proxy does not exist, meaning this Application hasn't executed initInIVILink yet!",false);
      }
   }

   Service::ListOfUids Application::getActiveServices()
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);
      Service::ListOfUids list;
      if (mpServManProxy)
      {
         CServiceManager::getInstance(mAppInfo)->getActive(list);
      }
      else
      {
         LOG4CPLUS_WARN(gCAppLogger,"ServManProxy doesn't exist. App is not inited!!!");
      }
      return list;
   }

   bool Application::isActive(const Service::Uid &service)
   {
      LOG4CPLUS_TRACE_METHOD(gCAppLogger,__PRETTY_FUNCTION__);
      if (mpServManProxy)
      {
         return CServiceManager::getInstance(mAppInfo)->isActive(service);
      }
      else
      {
         LOG4CPLUS_WARN(gCAppLogger,"ServManProxy doesn't exist. App is not inited!!!");
      }
   }

   void Application::onInitDone(ELaunchInfo /*launcher*/)
   {
      LOG4CPLUS_WARN(gCAppLogger,__PRETTY_FUNCTION__);
   }

   void Application::onIncomingServiceBeforeLoading(const Service::Uid & /*service*/)
   {
      LOG4CPLUS_WARN(gCAppLogger,__PRETTY_FUNCTION__);
   }

   void Application::onIncomingServiceAfterLoading(const Service::Uid &/*service*/)
   {
      LOG4CPLUS_WARN(gCAppLogger,__PRETTY_FUNCTION__);
   }

   void Application::onServiceLoadError(const Service::Uid & /*service*/)
   {
      LOG4CPLUS_WARN(gCAppLogger,__PRETTY_FUNCTION__);
   }

   void Application::onServiceDropped(const Service::Uid & /*service*/)
   {
      LOG4CPLUS_WARN(gCAppLogger,__PRETTY_FUNCTION__);
   }

    void Application::onLinkUp()
    {
      LOG4CPLUS_WARN(gCAppLogger,__PRETTY_FUNCTION__);
    }

    void Application::onPhysicalLinkDown()
    {
      LOG4CPLUS_WARN(gCAppLogger,__PRETTY_FUNCTION__);
    }

    bool Application::isLinkAlive()
    {
       if (mpAppManProxy)
       {
        return mpAppManProxy->isLinkAlive();
       }
       else
       {
          LOG4CPLUS_WARN(gCAppLogger,"AppManProxy doesn't exist. App is not inited!!!");
          return false;
       }
    }

    void Application::internalRun()
    {
        LOG4CPLUS_WARN(gCAppLogger,__PRETTY_FUNCTION__);

        mpAppManProxy = new App::CAppManProxy(this, mAppInfo);
        mpServManProxy = new App::CServManProxy(this);
        CServiceManager::getInstance(mAppInfo)->registerClient(mpServManProxy);
        AppMan::CAppManConnectController::instance(mAppInfo)->registerAMProxy(mpAppManProxy);

        mpAppManProxy->startProxyThread();
        mpAppManProxy->init(mSupportedServices);

        if (!gCallbacks.empty())
        {
            LOG4CPLUS_INFO(gCAppLogger, "");
            for (std::map<Profile::ApiUid, Profile::IProfileCallbackProxy*>::const_iterator
                    it = gCallbacks.begin(); gCallbacks.end() != it; ++it)
            {
                if (it->second)
                {
                    CServiceManager::getInstance(mAppInfo)->
                            registerProfileCallbacks(it->first, it->second);
                }
            }
            gCallbacks.clear();
        }
    }


}
