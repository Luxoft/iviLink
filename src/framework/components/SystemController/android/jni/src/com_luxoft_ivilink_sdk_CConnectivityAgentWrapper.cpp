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

#include <cstdlib>
#include <time.h>
#include <unistd.h>

#include "framework/components/SystemController/android/jni/src/com_luxoft_ivilink_sdk_CConnectivityAgentWrapper.hpp"
#include "framework/components/ConnectivityAgent/generic/L1/L1InterfaceStub.hpp"
#include "framework/components/ConnectivityAgent/linux/config.hpp"

using namespace iviLink::ConnectivityAgent::L1;

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_CConnectivityAgentWrapper_startCA
  (JNIEnv *, jobject)
{
   std::string sock_addr;
   sock_addr = std::string(first_lsn_path);
   L1InterfaceStub::getInstance()->start(false, sock_addr.c_str()); //isServer doesn't matter anymore
   while(L1InterfaceStub::getInstance()->getShutdownState() == false)
   {
      sleep(1000);
   };
}
