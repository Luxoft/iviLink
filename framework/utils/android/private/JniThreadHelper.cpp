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


#include <android/log.h>
#include <cstddef>
#include <cassert>

 
#include "JniThreadHelper.hpp"

namespace iviLink
{
   namespace Android
   {
      JniThreadHelper::JniThreadHelper(JavaVM* jm)
      {
         mJm = jm;
         mAttached = false;
         if(mJm->GetEnv((void**)&mEnv, JNI_VERSION_1_6) == JNI_EDETACHED){
            mJm->AttachCurrentThread(&mEnv, NULL);
            mAttached = true;
            __android_log_print(ANDROID_LOG_VERBOSE, "Utils.Android.JniThreadHelper", "in constructor: attached");
         } else {
            __android_log_print(ANDROID_LOG_VERBOSE, "Utils.Android.JniThreadHelper", "in constructor: no attach needed");
         }
      }
      
      JniThreadHelper::~JniThreadHelper()
      {
         if(mAttached)
         {
            __android_log_print(ANDROID_LOG_VERBOSE, "Utils.Android.JniThreadHelper", "in destructor: detach is needed");
            mJm->DetachCurrentThread();
            __android_log_print(ANDROID_LOG_VERBOSE, "Utils.Android.JniThreadHelper", "in destructor: detached");
         }
         else 
         {
            __android_log_print(ANDROID_LOG_VERBOSE, "Utils.Android.JniThreadHelper", "in destructor: detach is not needed");
         }
      }
      
      JNIEnv* JniThreadHelper::getEnv()
      {
         __android_log_print(ANDROID_LOG_VERBOSE, "Utils.Android.JniThreadHelper", "JNIEnv was requested");
         assert(mEnv);
         return mEnv;
      }
      
   } // namespace Android
} // namespace iviLink
