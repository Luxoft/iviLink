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


#ifndef NAVIAPP_HPP_
#define NAVIAPP_HPP_

#include <jni.h>

#include "Application.hpp"
#include "Logger.hpp"
#include "CNotificationsProfileProxy.hpp"
#include "AppInfo.hpp"
#include "JniThreadHelper.hpp"

class NotificationsApp : public iviLink::Application
                        ,public INotificationsProfileAPI::Callbacks
{
   

public:
   NotificationsApp(iviLink::Android::AppInfo appInfo, JavaVM * pJM, jobject callbackObj, const std::string & serviceName);

   void init();

   virtual ~NotificationsApp();

   virtual void onInitDone(iviLink::ELaunchInfo launcher);

   void sendString(std::string sendUpdate);

   virtual void onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service);

   virtual void onIncomingServiceAfterLoading(const iviLink::Service::Uid &service);

   virtual void onServiceDropped(iviLink::Service::Uid const&);

   virtual void handleError(BaseError const & error)
   {
      LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
      LOG4CPLUS_INFO(msLogger,static_cast<std::string>(error));
   }
   
   virtual void gotString(const std::string & incomingString);

private:

   static Logger msLogger;
   iviLink::Android::AppInfo mAppInfo;
   CNotificationsProfileProxy * mProxy;

   std::string mServiceName;

   JavaVM * mJM;
   jobject jCallbacks;
   jmethodID midHandleMessage;
   bool isServiceAlive;
};

#endif /* NAVIAPP_HPP_ */

