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







#ifndef CBASICSAMPLE_HPP_
#define CBASICSAMPLE_HPP_

//CApp header, must be included for interaction with apllication manager
#include "framework/public/appLib/CApp.hpp"
#include "utils/misc/Logger.hpp"
//basic sample api profile header, must be included for implementing callbacks
#include "samples/linux/Profiles/ProfileAPI/IBasicSampleProfileAPI.hpp"
#include "utils/threads/CSignalSemaphore.hpp"

#ifndef ANDROID
#else
#include <jni.h>
#include "utils/android/AppInfo.hpp"
#include "utils/android/JniThreadHelper.hpp"
#endif //ANDROID

// CBasicSample class inherits CApp class
class CBasicSample : public iviLink::CApp
{
   // BasicSampleProfileCallbacks class inherits IBasicSampleProfile_API::Callbacks class which describing callbacks
   class BasicSampleProfileCallbacks : public IBasicSampleProfile_API::Callbacks
   {
      CSignalSemaphore * mpSemaphore;
      public:
         #ifndef ANDROID
         explicit BasicSampleProfileCallbacks(CSignalSemaphore & semaphore)
            : mpSemaphore(&semaphore)
         {}
         #else
         explicit BasicSampleProfileCallbacks(CSignalSemaphore & semaphore, JavaVM* pJm, jobject callbacks, jmethodID operands, jmethodID result)
            : mpSemaphore(&semaphore),
              mpJM(pJm),
              jCallbacks(callbacks),
              jOnOperands(operands),
              jOnResult(result)
         {}
         #endif //ANDROID


         virtual void handleError(CError const & error)
         {
         }

	      /**
          * Callback that should be invoked when paired profile requested from other side
          * @param a, b - operands
          * @return none
          */
         virtual void operandsReceived(int a, int b)
         {
            #ifndef ANDROID
            #else
            JNIEnv *env;
            iviLink::Android::JniThreadHelper jth(mpJM);
            env = jth.getEnv();
            env->CallVoidMethod(jCallbacks, jOnOperands, a,b);
            #endif //ANDROID
            mpSemaphore->signal();
         }

	      /**
          * Callback that should be invoked when paired profile requested from other side 
          * @param a - result of operation
          * @return none
          */
         virtual void resultReceived(int a)
         {
            #ifndef ANDROID
            #else
            JNIEnv *env;
            iviLink::Android::JniThreadHelper jth(mpJM);
            env = jth.getEnv();
            env->CallVoidMethod(jCallbacks, jOnResult, a);
            #endif //ANDROID
            mpSemaphore->signal();
         }
        
         private:
            #ifndef ANDROID
            #else
            JavaVM* mpJM;
            jmethodID jOnOperands;
            jmethodID jOnResult;
            jobject jCallbacks;
            #endif //ANDROID
   };

public:
   #ifndef ANDROID
   CBasicSample(CSignalSemaphore & semaphore);
   #else
   CBasicSample(CSignalSemaphore & semaphore, iviLink::Android::AppInfo appInfo, JavaVM* pJm, jobject callbacks, jmethodID operands, jmethodID result);
   #endif //ANDROID
   virtual ~CBasicSample();

   /**
    * Callback that should be invoked by application library thread 
    * after initialization application in application manager 
    * @param launcher shows the initiator of application start
    * @return none
    */
   virtual void initDone(iviLink::ELaunchInfo launcher);

private:

   //instance of class implementing callbacks
   BasicSampleProfileCallbacks mBasicSampleCallbacks;
   IBasicSampleProfile_API* mpBasicSampleAPI;

   static Logger msLogger;
   CSignalSemaphore * mpSemaphore;  
    
   #ifndef ANDROID
   #else
   // this is a structure with additional info needed for android version
   // see utils/android/AppInfo.hpp and documentation
   iviLink::Android::AppInfo mAppInfo;
   #endif //ANDROID
};

#endif /* CBASICSAMPLE_HPP_ */
