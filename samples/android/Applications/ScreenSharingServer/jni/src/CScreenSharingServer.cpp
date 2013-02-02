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
#include "UnstableAPI.hpp"
#include "CScreenSharingServer.hpp"
#include "CScreenSharingProfileProxy.hpp"
#include "ScreenSharingMessages.h"

Logger CScreenSharingServer::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.ScreenSharingServer"));

CScreenSharingServer::CScreenSharingServer(iviLink::Android::AppInfo appInfo
                                       , JavaVM* pJm
                                       , jobject callbacks
                                       , jmethodID onDataReceived
                                       , jmethodID finishApp)
   : Application(iviLink::Service::Uid("ScreenSharingServerService"), appInfo)
   , mpJM(pJm)
   , jOnDataReceived(onDataReceived)
   , jFinishApp(finishApp)
   , jCallbacks(callbacks)
   , mAppInfo(appInfo)
   , isServiceAlive(false)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   mpServerProfileProxy = NULL;
   mpServerProfileProxy = new CScreenSharingProfileProxy(iviLink::Service::Uid("ScreenSharingServerService"), mAppInfo);
}


void CScreenSharingServer::init()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   initInIVILink();
}

CScreenSharingServer::~CScreenSharingServer()
{
	LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   delete mpServerProfileProxy;
   mpServerProfileProxy = NULL;
}

void CScreenSharingServer::onInitDone(iviLink::ELaunchInfo launcher)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   if (iviLink::LAUNCHED_BY_USER == launcher)
   {
      LOG4CPLUS_INFO(msLogger, "started by user");
      registerProfileCallbacks(iviLink::Profile::ApiUid("ScreenSharingProfile_PAPI_UID"), this);
      iviLink::Error loadResult = loadService(iviLink::Service::Uid("ScreenSharingServerService"));
      if (loadResult.isNoError())
      {
         LOG4CPLUS_INFO(msLogger, "service started");
         isServiceAlive = true;

         std::stringstream startStream;
         startStream << iviLink::SCR_SHARING_START;
         LOG4CPLUS_INFO(msLogger, "CScreenSharingServer::initDone() : sending data : " + startStream.str() );
         mpServerProfileProxy->sendData(startStream.str() );

         sendServerAddress();
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger, "service NOT started: " + loadResult.toString());
         isServiceAlive = false;
         killProcess(1);
      }
   }
   else
      LOG4CPLUS_INFO(msLogger, "started by iviLink");
   
   LOG4CPLUS_INFO(msLogger, "EXIT: CScreenSharingServer::initDone()");
}

void CScreenSharingServer::dataReceived(const std::string &data)
{
   JNIEnv *env;
   iviLink::Android::JniThreadHelper jth(mpJM);
   env = jth.getEnv();
   env->CallVoidMethod(jCallbacks, jOnDataReceived, iviLink::Android::StringConversion::stdToJ(data, env) );
}

void CScreenSharingServer::handleError(BaseError const & error)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(msLogger,static_cast<std::string>(error));
}

void CScreenSharingServer::sendData(const std::string &data)
{
   if (isServiceAlive)
   {
      if (mpServerProfileProxy)
      {
         mpServerProfileProxy->sendData(data);
         LOG4CPLUS_INFO(msLogger, "CScreenSharingServer::sendData() : data is sent : " + data);
      }
      else
         LOG4CPLUS_WARN(msLogger, "CScreenSharingViewer::sendData() : \
                                   send attempt with invalid ProfileProxy ptr");
   }
   else
      LOG4CPLUS_WARN(msLogger, "CScreenSharingViewer::sendData() : send attempt with !isServiceAlive");
}

void CScreenSharingServer::sendData(const jstring &data)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   JNIEnv *env;
   iviLink::Android::JniThreadHelper jth(mpJM);
   env = jth.getEnv();
   std::string outData = iviLink::Android::StringConversion::jToStd(data, env);
   LOG4CPLUS_INFO(msLogger, "CScreenSharingServer::sendData() : sending data : " + outData);

   sendData(outData);
}

void CScreenSharingServer::onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(msLogger, "Registering profile callbacks");
   registerProfileCallbacks(iviLink::Profile::ApiUid("ScreenSharingProfile_PAPI_UID"), this);
}

void CScreenSharingServer::onIncomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   isServiceAlive = true;
}

void CScreenSharingServer::onServiceDropped(const iviLink::Service::Uid &service)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_WARN(msLogger, "Other side has died!");

   JNIEnv *env;
   iviLink::Android::JniThreadHelper jth(mpJM);
   env = jth.getEnv();
   env->CallVoidMethod(jCallbacks, jFinishApp);
}

void CScreenSharingServer::sendServerAddress()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   std::stringstream stream;
   stream << iviLink::SCR_SHARING_SERVER_ADDRESS;
   std::string outData = stream.str();

   LOG4CPLUS_INFO(msLogger, "local IP : ");

   ConnectionInformation ci;
   if(::getConnectionInformation(&ci).isNoError())
   {
      LOG4CPLUS_INFO(msLogger, ci.getLocalAddress());
      outData += ci.getLocalAddress();
   }

   LOG4CPLUS_INFO(msLogger, "CScreenSharingServer::initDone() : sending data : " + outData);

   sendData(outData);
}
