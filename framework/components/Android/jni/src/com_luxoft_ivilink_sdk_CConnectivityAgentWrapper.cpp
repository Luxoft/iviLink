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


#include <cstdlib>
#include <time.h>
#include <unistd.h>

#include "com_luxoft_ivilink_sdk_CConnectivityAgentWrapper.hpp"
#include "BluetoothObject.h"
#include "ConnectivityAgentEntryPoint.hpp"

using namespace iviLink::ConnectivityAgent::L1;

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_CConnectivityAgentWrapper_startCA
  (JNIEnv * env, jobject, jobject bluetooth)
{
	//doing this just to avoid "stale local reference error"
	// BluetoothHelper is a singleton, and thus cannot be garbage collected
   jobject toStore = env->NewGlobalRef(bluetooth);
   gBlueHelper = toStore;
   iviLink::ConnectivityAgent::connectivityAgentEntryPoint();
}
