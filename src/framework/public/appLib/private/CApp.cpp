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








#include <cassert>

#include "CAppManProxy.hpp"
#include "CServManProxy.hpp"

#include "framework/public/appLib/CApp.hpp"
#include "framework/libraries/ServiceManager/CServiceManager.hpp"
#include "CLogger.hpp"

 #include "framework/components/ConnectivityAgent/generic/common/API.hpp"

namespace iviLink
{
   using Service::CServiceManager;

   #ifndef ANDROID
   CApp::CApp()
      : mpAppManProxy(new App::CAppManProxy(this))
      , mpServManProxy(new App::CServManProxy(this))
   #else
   CApp::CApp(iviLink::Android::AppInfo appInfo)
      : mpAppManProxy(new App::CAppManProxy(this,appInfo.launchInfo))
      , mpServManProxy(new App::CServManProxy(this))
   #endif //ANDROID
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      #ifndef ANDROID
      CServiceManager::getInstance()->registerClient(mpServManProxy);
      #else
      mAppInfo = appInfo;
      CServiceManager::getInstance(mAppInfo)->registerClient(mpServManProxy);
      #endif //ANDROID
      mpAppManProxy->start();
      Service::ListOfUids services;
      mpAppManProxy->init(services);
   }

   #ifndef ANDROID
   CApp::CApp(const Service::Uid &service)
      : mpAppManProxy(new App::CAppManProxy(this))
      , mpServManProxy(new App::CServManProxy(this))
   #else
   CApp::CApp(const Service::Uid &service, iviLink::Android::AppInfo appInfo)
      : mpAppManProxy(new App::CAppManProxy(this, appInfo.launchInfo))
      , mpServManProxy(new App::CServManProxy(this))
   #endif //ANDROID
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      #ifndef ANDROID
      CServiceManager::getInstance()->registerClient(mpServManProxy);
      #else
      mAppInfo = appInfo;
      CServiceManager::getInstance(mAppInfo)->registerClient(mpServManProxy);
      #endif //ANDROID
      mpAppManProxy->start();
      Service::ListOfUids services;
      services.push_back(service);
      mpAppManProxy->init(services);
   }

   #ifndef ANDROID
   CApp::CApp(const Service::ListOfUids &services)
      : mpAppManProxy(new App::CAppManProxy(this))
      , mpServManProxy(new App::CServManProxy(this))
   #else
   CApp::CApp(const Service::ListOfUids &services, iviLink::Android::AppInfo appInfo)
      : mpAppManProxy(new App::CAppManProxy(this, appInfo.launchInfo))
      , mpServManProxy(new App::CServManProxy(this))
   #endif //ANDROID 
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      #ifndef ANDROID
      CServiceManager::getInstance()->registerClient(mpServManProxy);
      #else
      mAppInfo = appInfo;
      CServiceManager::getInstance(mAppInfo)->registerClient(mpServManProxy);
      #endif //ANDROID
      mpAppManProxy->start();
      mpAppManProxy->init(services);
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
      ::destroyConnectivityAgent();
      #endif
   }

   void CApp::setEnabled(const Service::Uid &service, bool enable)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      mpAppManProxy->useService(service, enable);
   }

   void CApp::setEnabled(const Service::ListOfUids &services, bool enable)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      for (Service::ListOfUids::const_iterator it = services.begin();
            services.end() != it; ++it)
      {
         mpAppManProxy->useService(*it, enable);
      }
   }

   ELaunchInfo CApp::getLaunchInfo()
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      return mpAppManProxy->launcher();
   }

   void CApp::registerProfileCallbacks(const Profile::ApiUid &profileApi, Profile::IProfileCallbackProxy* pCallbacks)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      #ifndef ANDROID
      CServiceManager::getInstance()->registerProfileCallbacks(profileApi, pCallbacks);
      #else
      CServiceManager::getInstance(mAppInfo)->registerProfileCallbacks(profileApi, pCallbacks);
      #endif //ANDROID
   }

   bool CApp::loadService(const Service::Uid &service)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      #ifndef ANDROID
      return CServiceManager::getInstance()->load(service);
      #else
      return CServiceManager::getInstance(mAppInfo)->load(service);
      #endif //ANDROID
   }

   void CApp::unloadService(const Service::Uid &service)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      #ifndef ANDROID
      CServiceManager::getInstance()->unload(service);
      #else
      CServiceManager::getInstance(mAppInfo)->unload(service);
      #endif //ANDROID
   }

   Service::ListOfUids CApp::getActiveServices()
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      Service::ListOfUids list;
      #ifndef ANDROID
      CServiceManager::getInstance()->getActive(list);
      #else
      CServiceManager::getInstance(mAppInfo)->getActive(list);
      #endif //ANDROID
      return list;
   }

   bool CApp::isActive(const Service::Uid &service)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      #ifndef ANDROID
      return CServiceManager::getInstance()->isActive(service);
      #else
      return CServiceManager::getInstance(mAppInfo)->isActive(service);
      #endif //ANDROID
   }

   void CApp::initDone(ELaunchInfo /*launcher*/)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
   }

   void CApp::incomingServiceBeforeLoading(const Service::Uid &/*service*/)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
   }

   void CApp::incomingServiceAfterLoading(const Service::Uid &/*service*/)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
   }

   void CApp::serviceLoadError(const Service::Uid &/*service*/)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
   }

   void CApp::serviceDropped(const Service::Uid &/*service*/)
   {
      LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
   }

}
