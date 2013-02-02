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


#include "MakeRequest.hpp"
#include <cstdlib>
#include <android/log.h>

#include "Logger.hpp"
#include "JniThreadHelper.hpp"

namespace iviLink{
   namespace Android{
   	
      static JavaVM * gJm;
      static jobject gLauncher;
      static jmethodID gMethod;
      
      void setup(JavaVM * jm, jobject launcher, jmethodID launchMethod)
      {
         gJm = jm;
         gLauncher = launcher;
         gMethod = launchMethod;
      }


      void makeRequest(Request req)
      {
         JNIEnv * env;
         JniThreadHelper  jth(gJm);
         env=jth.getEnv();
         __android_log_print(ANDROID_LOG_INFO, "iviLink::Android::MakeRequest", convertIntegerToString(req).c_str());
         env->CallVoidMethod(gLauncher, gMethod, req);
      }
   } // Android
} // iviLink
