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








#include "CBasicSample.hpp"
//profile proxy header provides the access to profile API, must be included
#include "samples/linux/Profiles/ProfileProxy/CBasicSampleProfileProxy.hpp"

Logger CBasicSample::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.BasicSample"));

#ifndef ANDROID
CBasicSample::CBasicSample(CSignalSemaphore & semaphore)
#else
CBasicSample::CBasicSample(CSignalSemaphore & semaphore, iviLink::Android::AppInfo appInfo, JavaVM* pJm, jobject callbacks, jmethodID operands, jmethodID result)
#endif //ANDROID

   //informing iviLink that this app supports service with this UID
   #ifndef ANDROID
   : CApp(iviLink::Service::Uid("BasicSampleServiceHUI"))
   , mBasicSampleCallbacks(semaphore)
   #else
   : CApp(iviLink::Service::Uid("BasicSampleServiceHUI"), appInfo)
   , mBasicSampleCallbacks(semaphore,pJm,callbacks,operands,result)
   , mAppInfo(appInfo)
   #endif //ANDROID
   , mpSemaphore(&semaphore)
{
   //registration profile callbacks for given profile API
   registerProfileCallbacks(iviLink::Profile::ApiUid("BasicSampleProfile_PAPI_UID"), &mBasicSampleCallbacks);
}

CBasicSample::~CBasicSample()
{

}

/**
 * Callback that should be invoked by application library thread 
 * after initialization application in application manager 
 */
void CBasicSample::initDone(iviLink::ELaunchInfo launcher)
{

   //if application was launched by user
   if (iviLink::LAUNCHED_BY_USER == launcher)
   {
      LOG4CPLUS_INFO(msLogger, "started by user");

      //loading service with required UID
      if (loadService(iviLink::Service::Uid("BasicSampleServiceHUI")))
      {
         LOG4CPLUS_INFO(msLogger, "service started");

         //loading profile proxy which corresponds to given service UID
         #ifndef ANDROID
         CBasicSampleProfileProxy sampleProxy(iviLink::Service::Uid("BasicSampleServiceHUI"));
         #else
         CBasicSampleProfileProxy sampleProxy(iviLink::Service::Uid("BasicSampleServiceHUI"), mAppInfo);
         #endif //ANDROID

         //using method from profile API
         sampleProxy.sendOperands(8,21);
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger, "service NOT started");
         mpSemaphore->signal();
      }
   }
  
   //if application was launched by other side
   else
   {
      LOG4CPLUS_INFO(msLogger, "started by iviLink");
   }
}
