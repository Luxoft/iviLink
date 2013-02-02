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


#include "src/com_luxoft_ivilink_samples_screensharingviewer_BridgeC.hpp"
#include "CScreenSharingViewer.hpp"

#include "Logger.hpp"
#include "AppInfo.hpp"
#include "StringConversion.hpp"

#include <string>
#include <cassert>
#include <tr1/memory>

static JavaVM * jm;
std::tr1::shared_ptr<CScreenSharingViewer> gApp;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
	jm = jvm;
	return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_screensharingviewer_BridgeC_startApp(JNIEnv * env
																				, jobject activity
																				, jstring servicePath
																				, jstring launchInfo
																				, jstring internalPath)
{
	jobject callbacks = env->NewGlobalRef(activity);
	// we get object's class to extract method id's of the methods we intend to use as callbacks
	jclass claz = (jclass)env->GetObjectClass(activity);
	jmethodID onData = env->GetMethodID(claz, "dataReceived","(Ljava/lang/String;)V");
	// GetMethodID returns 0 in case of failure
	assert(onData);
		// this is a structure with android-specific application's info
	iviLink::Android::AppInfo appInfo;
	appInfo.privateDirPath = iviLink::Android::StringConversion::jToStd(internalPath, env);
	appInfo.serviceRepoPath = iviLink::Android::StringConversion::jToStd(servicePath, env);
	appInfo.launchInfo=iviLink::Android::StringConversion::jToStd(launchInfo, env);
	appInfo.isValid = true;

	Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.ScreenSharingViewer"));
	
	gApp = std::tr1::shared_ptr<CScreenSharingViewer>(new CScreenSharingViewer(appInfo, jm, callbacks, onData));
	gApp.get()->init();
	LOG4CPLUS_INFO(logger, "CScreenSharingViewer(native) created");
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_screensharingviewer_BridgeC_sendData
  (JNIEnv *, jobject, jstring data) {
  	gApp->sendData(data);
}
