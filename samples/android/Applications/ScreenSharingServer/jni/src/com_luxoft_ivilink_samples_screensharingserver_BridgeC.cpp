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


#include <string>
#include <cassert>
#include <tr1/memory>

#include "com_luxoft_ivilink_samples_screensharingserver_BridgeC.hpp"
#include "CScreenSharingServer.hpp"

#include "Logger.hpp"
#include "AppInfo.hpp"
#include "StringConversion.hpp"


#include "json.h"

static JavaVM * jm;
std::tr1::shared_ptr<CScreenSharingServer> gApp;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
	jm = jvm;
	return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_screensharingserver_BridgeC_startApp(
																				JNIEnv * env
																				, jobject activity
																				, jstring servicePath
																				, jstring jlaunchInfo
																				, jstring internalPath)
{
	jobject callbacks = env->NewGlobalRef(activity);
	// we get object's class to extract method id's of the methods we intend to use as callbacks
	jclass claz = (jclass)env->GetObjectClass(activity);
	jmethodID onData = env->GetMethodID(claz, "dataReceived","(Ljava/lang/String;)V");
	jmethodID finish = env->GetMethodID(claz, "finishApp","()V");
	// this is a structure with android-specific application's info
	std::string privateDirPath = iviLink::Android::StringConversion::jToStd(internalPath, env);
	std::string serviceRepoPath = iviLink::Android::StringConversion::jToStd(servicePath, env);
	std::string launchInfo=iviLink::Android::StringConversion::jToStd(jlaunchInfo, env);
	iviLink::Android::AppInfo appInfo(serviceRepoPath, privateDirPath, launchInfo);

	Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.ScreenSharingServer"));
	
	gApp = std::tr1::shared_ptr<CScreenSharingServer>(new CScreenSharingServer(appInfo
	                                                                           , jm
	                                                                           , callbacks
	                                                                           , onData
	                                                                           , finish));
	gApp.get()->init();
	LOG4CPLUS_INFO(logger, "CScreenSharingServer(native) created");
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_screensharingserver_BridgeC_sendData
  (JNIEnv *, jobject, jstring data) {
  	if (gApp)
  		gApp->sendData(data);
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_screensharingserver_BridgeC_sendServerAddress
  (JNIEnv *, jobject) {
  	if (gApp)
  		gApp->sendServerAddress();
}
