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

#include "samples/android/BasicSample/jni/src/com_luxoft_ivilink_samples_basic_BasicSampleActivity.hpp"
#include "samples/linux/BasicSample/CBasicSample.hpp"

#include "utils/misc/Logger.hpp"
#include "utils/android/AppInfo.hpp"
#include "utils/android/StringConversion.hpp"

#include <string>
#include <cassert>

static JavaVM * jm;

// Standard callback. Called when System.load is called for the appwrapper library (see BasicSampleActivity)
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
   // caching reference to the jvm our app is running in
	jm = jvm;
	return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_basic_BasicSampleActivity_main(JNIEnv * env, jobject activity, jstring servicePath, jstring launchInfo, jstring internalPath)
{
   // jobject activity is an instance of BasicSampleAcitivity that has called this method
   // we store it as a global reference, otherwise attempts to use it later may cause "stale local reference" error
	jobject callbacks = env->NewGlobalRef(activity);
	// we get object's class to extract method id's of the methods we intend to use as callbacks
	jclass claz = (jclass)env->GetObjectClass(activity);
	// "operandsReceived" is the method's name (as it is declared in the BasicSampleActivity class)
	// "(II)V" is the method's signature meaning it takes two ints as parameters and returns void
	jmethodID onOperands = env->GetMethodID(claz, "operandsReceived","(II)V");
	// GetMethodID returns 0 in case of failure
	assert(onOperands);
	jmethodID onResult = env->GetMethodID(claz, "resultReceived","(I)V");
	assert(onResult);
	
	// this is a structure with android-specific application's info
	iviLink::Android::AppInfo appInfo;
	appInfo.privateDirPath = iviLink::Android::StringConversion::jToStd(internalPath, env);
	appInfo.serviceRepoPath = iviLink::Android::StringConversion::jToStd(servicePath, env);
	appInfo.launchInfo=iviLink::Android::StringConversion::jToStd(launchInfo, env);

	CSignalSemaphore semaphore;

	Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.BasicSample"));
	CBasicSample* basicSample = new CBasicSample(semaphore, appInfo, jm, callbacks, onOperands, onResult);
	LOG4CPLUS_INFO(logger, "basic sample created");
	semaphore.waitTimeout(20000);
	LOG4CPLUS_INFO(logger, "after wait");
	delete basicSample;
	LOG4CPLUS_INFO(logger, "end BasicSample main()");
}
