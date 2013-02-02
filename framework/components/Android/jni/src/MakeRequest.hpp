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


#ifndef __SEND_INTENT_HPP
#define __SEND_INTENT_HPP

#include <string>
#include <jni.h>

namespace iviLink{
   namespace Android{

      enum Request{
         eLaunchWatchdog = 0,
         eLaunchAgent,
         eLaunchNegotiator,
         eLaunchProfileManager,
         eLaunchApplicationManager,
         eStartAuth,
         eIdle,
         eShutdown,
         eReset,
      };

      void setup(JavaVM * jm, jobject launcher, jmethodID launchMethod); //to be called in the jni layer - jobject is expected to be a global reference

      void makeRequest(Request req); // to be called from deep native code

   } // Android
} // iviLink

#endif //__SEND_INTENT_HPP
