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

#include "NotificationsApp.hpp"
#include "StringConversion.hpp"
#include "Exit.hpp"

Logger NotificationsApp::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.NotificationsApp"));


NotificationsApp::NotificationsApp(iviLink::Android::AppInfo appInfo, JavaVM * pJM, jobject callbackObj, const std::string & serviceName)
   : Application(iviLink::Service::Uid(serviceName.c_str()), appInfo)
   , mServiceName(serviceName)
   , mAppInfo(appInfo)
   , mJM(pJM)
   , jCallbacks(callbackObj)
   , isServiceAlive(false)
   , midHandleMessage(NULL)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   assert(mJM);
   mProxy = new CNotificationsProfileProxy(iviLink::Service::Uid(mServiceName.c_str()), mAppInfo);
   iviLink::Android::JniThreadHelper jth(mJM);
   JNIEnv * env = jth.getEnv();
   jclass claz = env->GetObjectClass(jCallbacks);
   midHandleMessage = env->GetMethodID(claz, "handleMessage", "(Ljava/lang/String;)V");
   assert(midHandleMessage != NULL);
}

void NotificationsApp::init()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   initInIVILink();
}

NotificationsApp::~NotificationsApp()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   delete mProxy;
}


void NotificationsApp::gotString(const std::string & incomingString)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_TRACE(msLogger, "incoming string: ");
   LOG4CPLUS_TRACE(msLogger, incomingString);
   iviLink::Android::JniThreadHelper jth(mJM);
   JNIEnv * env = jth.getEnv();
   jstring jIncomingString = iviLink::Android::StringConversion::stdToJ(incomingString, env);
   env->CallVoidMethod(jCallbacks, midHandleMessage, jIncomingString);
}


void NotificationsApp::onInitDone(iviLink::ELaunchInfo launcher)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   if (iviLink::LAUNCHED_BY_USER == launcher)
   {
      LOG4CPLUS_INFO(msLogger, "started by user");
      registerProfileCallbacks(iviLink::Profile::ApiUid("NotificationsProfile_PAPI_UID"),this);
      iviLink::Error loadResult = loadService(iviLink::Service::Uid(mServiceName.c_str()));
      if (loadResult.isNoError())
      {
         LOG4CPLUS_INFO(msLogger, "service started");   
         isServiceAlive = true;
      }
      else
      {
         LOG4CPLUS_FATAL(msLogger, "service NOT started: " + loadResult.toString());
         isServiceAlive = false;
         killProcess();
      }
   }
   else
   {
      LOG4CPLUS_INFO(msLogger, "started by iviLink");
   }
}

void NotificationsApp::onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_TRACE(msLogger, "Registering profile callbacks");
   registerProfileCallbacks(iviLink::Profile::ApiUid("NotificationsProfile_PAPI_UID"),this);
}

void NotificationsApp::onIncomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   isServiceAlive = true;
}

void NotificationsApp::sendString(std::string json) 
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   if(isServiceAlive)
      mProxy->sendString(json);
}

void NotificationsApp::onServiceDropped(iviLink::Service::Uid const&)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_WARN(msLogger, "Other side has died!");
   killProcess();
}
