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


#include <stdlib.h>

#include "Exit.hpp"
#include "CScreenSharingViewer.hpp"
#include "CScreenSharingProfileProxy.hpp"
#include "ScreenSharingMessages.h"

Logger CScreenSharingViewer::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.ScreenSharingViewer"));

CScreenSharingViewer::CScreenSharingViewer(iviLink::Android::AppInfo appInfo
                                       , JavaVM* pJm
                                       , jobject callbacks
                                       , jmethodID onDataReceived)
   : Application(iviLink::Service::Uid("ScreenSharingViewerService"), appInfo)
   , mpJM(pJm)
   , jCallbacks(callbacks)
   , jOnDataReceived(onDataReceived)
   , mAppInfo(appInfo)
   , isServiceAlive(false)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   mpViewerProfileProxy = NULL;
   mpViewerProfileProxy = new CScreenSharingProfileProxy(iviLink::Service::Uid("ScreenSharingViewerService"), mAppInfo);
}

void CScreenSharingViewer::init()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   initInIVILink();
}

CScreenSharingViewer::~CScreenSharingViewer()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   delete mpViewerProfileProxy;
   mpViewerProfileProxy = NULL;
}

void CScreenSharingViewer::onInitDone(iviLink::ELaunchInfo launcher)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   if (iviLink::LAUNCHED_BY_USER == launcher)
   {
      LOG4CPLUS_INFO(msLogger, "started by user");
      registerProfileCallbacks(iviLink::Profile::ApiUid("ScreenSharingProfile_PAPI_UID"), this);
      iviLink::Error loadResult = loadService(iviLink::Service::Uid("ScreenSharingViewerService"));
      if (loadResult.isNoError())
      {
         LOG4CPLUS_INFO(msLogger, "service started");
         isServiceAlive = true;

         std::stringstream startStream;
         startStream << iviLink::SCR_SHARING_START;
         LOG4CPLUS_INFO(msLogger, "CScreenSharingViewer::initDone() : sending data : " + startStream.str());
         mpViewerProfileProxy->sendData(startStream.str() );
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger, "service NOT started: " + loadResult.toString());
         isServiceAlive = false;
         killProcess(1);
      }
   }
   else
   {
      LOG4CPLUS_INFO(msLogger, "started by iviLink");
   }

   LOG4CPLUS_INFO(msLogger, "EXIT: CScreenSharingViewer::initDone()");
}

void CScreenSharingViewer::dataReceived(const std::string &data)
{
   JNIEnv *env;
   iviLink::Android::JniThreadHelper jth(mpJM);
   env = jth.getEnv();
   env->CallVoidMethod(jCallbacks, jOnDataReceived, iviLink::Android::StringConversion::stdToJ(data, env) );
}

void CScreenSharingViewer::handleError(BaseError const & error)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(msLogger,static_cast<std::string>(error));
}

void CScreenSharingViewer::onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(msLogger, "Registering profile callbacks");
   registerProfileCallbacks(iviLink::Profile::ApiUid("ScreenSharingProfile_PAPI_UID"), this);
}

void CScreenSharingViewer::onIncomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   isServiceAlive = true;
}

void CScreenSharingViewer::onServiceDropped(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_WARN(msLogger, "Other side has died!");

   killProcess();
}

void CScreenSharingViewer::sendData(const jstring &data)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   JNIEnv *env;
   iviLink::Android::JniThreadHelper jth(mpJM);
   env = jth.getEnv();
   std::string outData = iviLink::Android::StringConversion::jToStd(data, env);
   LOG4CPLUS_INFO(msLogger, "CScreenSharingViewer::sendData() : sending data : " + outData);
   if (isServiceAlive)
   {
      if (mpViewerProfileProxy)
      {
         mpViewerProfileProxy->sendData(outData);
         LOG4CPLUS_INFO(msLogger, "CScreenSharingViewer::sendData() : data is sent : " + outData);
      }
      else
         LOG4CPLUS_WARN(msLogger, "CScreenSharingViewer::sendData() : send attempt with invalid ProfileProxy ptr");
   }
   else
      LOG4CPLUS_WARN(msLogger, "CScreenSharingViewer::sendData() : send attempt with !isServiceAlive");
}
