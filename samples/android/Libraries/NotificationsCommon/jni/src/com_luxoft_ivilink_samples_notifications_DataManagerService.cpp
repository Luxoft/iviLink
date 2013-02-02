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
#include "com_luxoft_ivilink_samples_notifications_DataManagerService.hpp"

#include "AppInfo.hpp"
#include "StringConversion.hpp"
#include "NotificationsApp.hpp"

NotificationsApp * app;
JavaVM * jm;
jobject gServiceObject;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) 
{
	jm = jvm;
	return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_notifications_DataManagerService_startApp
  (JNIEnv * env, jobject serviceObject, jstring servicePath, jstring launchInfo, jstring internalPath, jstring serviceName)
{
	iviLink::Android::AppInfo appInfo;
	appInfo.privateDirPath = iviLink::Android::StringConversion::jToStd(internalPath, env);
	appInfo.serviceRepoPath = iviLink::Android::StringConversion::jToStd(servicePath, env);
	appInfo.launchInfo=iviLink::Android::StringConversion::jToStd(launchInfo, env);
	appInfo.isValid = true;
	std::string stdServiceName = iviLink::Android::StringConversion::jToStd(serviceName, env);
	gServiceObject = env->NewGlobalRef(serviceObject);
	app = new NotificationsApp(appInfo, jm, gServiceObject, stdServiceName);
	app->init();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_samples_notifications_DataManagerService_sendString
  (JNIEnv * env, jobject, jstring str)
{
   std::string json = iviLink::Android::StringConversion::jToStd(str, env);
   app->sendString(json);
}
