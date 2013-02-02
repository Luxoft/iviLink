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
#ifndef SCREENSHARINGVIEWER_HPP_
#define SCREENSHARINGVIEWER_HPP_

#include "Application.hpp"
#include "Logger.hpp"

#include "IScreenSharingProfileAPI.hpp"
#include "CSignalSemaphore.hpp"


#include <jni.h>
#include "AppInfo.hpp"
#include "JniThreadHelper.hpp"
#include "StringConversion.hpp"

class CScreenSharingProfileProxy;

class CScreenSharingViewer : public iviLink::Application
                           , public IScreenSharingProfile_API::Callbacks
{   
public:
   CScreenSharingViewer(iviLink::Android::AppInfo appInfo
                        , JavaVM* pJm
                        , jobject callbacks
                        , jmethodID onDataReceived);

   void init();
   
   virtual ~CScreenSharingViewer();

   /**
    * Called after the initialization of this application.
    * @param launcher is the output parameter providing information on how the
    *    application was launched: app may be launched either by user (in this
    *    case services should be loaded with loadService() method call
    *    ,and then proxies for profiles may be obtained),
    *    or by Application Manager (after request from the other side -
    *    then proxies may be used).
    */
   virtual void onInitDone(iviLink::ELaunchInfo launcher);

   /**
    * Callback is called before loading incoming service
    * @param service is UID of service
    */
   virtual void onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service);

   /**
    * Callback is called after loading of incoming service
    */
   virtual void onIncomingServiceAfterLoading(const iviLink::Service::Uid &service);
   
   /**
    * Callback is called in case of and service session
    * @param service UID is uid of service
    */
   virtual void onServiceDropped(const iviLink::Service::Uid &service);

   /**
    * Callback is called in case of incoming data
    * @param data is data string
    */
   virtual void dataReceived(const std::string &data);

   /**
    * Error handler
    * @param error is error string
    */
   virtual void handleError(BaseError const & error);

   /**
    * Send data to the server app
    * @param data is data string
    */
   void sendData(const jstring &data);

private:
   IScreenSharingProfile_API* mpScreenSharingAPI;
   CScreenSharingProfileProxy *mpViewerProfileProxy;
   JavaVM* mpJM;
   jmethodID jOnDataReceived;
   jobject jCallbacks;
   bool isServiceAlive;

   static Logger msLogger;
    
   iviLink::Android::AppInfo mAppInfo;
};

#endif /* SCREENSHARINGVIEWER_HPP_ */
