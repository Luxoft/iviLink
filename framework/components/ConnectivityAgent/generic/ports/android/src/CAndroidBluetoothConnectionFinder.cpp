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

#ifndef ANDROID
#else

#include <cassert>

#include "CAndroidBluetoothConnectionFinder.hpp"
#include "CAndroidBluetoothAdapter.hpp"
#include "GlobalJavaVmGetter.h"
#include "JniThreadHelper.hpp"
#include "BluetoothObject.h"

using namespace iviLink::ConnectivityAgent::HAL;


Logger CAndroidBluetoothConnectionFinder::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CAndroidBluetoothConnectionFinder"));


CAndroidBluetoothConnectionFinder::CAndroidBluetoothConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender)
:
   CConnectionFinder(handler, gender)
{
 	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
 	mpJM = globalGetJavaVM();
 	jImplementation = getBluetoothHelperInstance();
 	if(jImplementation == NULL)
 	{
 		LOG4CPLUS_ERROR(logger, "jImplementation is NULL, isValid = false");
 		isValid = false;
 		return;
 	}
 	isValid = true;
	iviLink::Android::JniThreadHelper jth(mpJM);
	JNIEnv * env = jth.getEnv();
	assert(env);
	jclass claz = env->GetObjectClass(jImplementation);
	performSearchJava = env->GetMethodID(claz, "performSearch", "()I");
	assert(performSearchJava>=0);
}


CAndroidBluetoothConnectionFinder::~CAndroidBluetoothConnectionFinder()
{
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);	
}

void CAndroidBluetoothConnectionFinder::performSearch()
{
	if (!isValid) 
	{
		sleep(1000);
		LOG4CPLUS_ERROR(logger, "is not valid, sleeping");
		return;
	}
	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	iviLink::Android::JniThreadHelper jth(mpJM);
	JNIEnv * env = jth.getEnv();
	int result = (int)env->CallIntMethod(jImplementation, performSearchJava);
	LOG4CPLUS_INFO(logger, "performSearch returned: ");
	if(result != 0) {
		LOG4CPLUS_INFO(logger, "performSearch returned true (found and connected)");
		CBluetoothInfo info;
		if(result == 1) {
			info.isServer = true;
		} else {
			info.isServer = false;
		}
   	    CAndroidBluetoothAdapter * pAdapter = new CAndroidBluetoothAdapter(info);
		setReady(pAdapter);
	} else if (result == 0) {
		LOG4CPLUS_INFO(logger, "performSearch returned false (not found anything)");
	}
}


#endif //ANDROID
