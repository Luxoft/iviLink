/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#include <android/log.h>

#include "com_luxoft_ivilink_sdk_authentication_AuthenticationActivity.hpp"
#include "AppInfo.hpp"
#include "StringConversion.hpp"
#include "AuthenticationDialog.hpp"

static JavaVM * jm;
jobject appObj;
authentication::AuthenticationDialog  * auth;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
	jm = jvm;
	return JNI_VERSION_1_6;
}


JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity_start
  (JNIEnv * env, jobject callbacks, jstring pathToService, jstring jlaunchInfo, jstring internalPath, jstring trlistPath)
{
  std:: string privateDirPath = iviLink::Android::StringConversion::jToStd(internalPath, env);
  std::string serviceRepoPath = iviLink::Android::StringConversion::jToStd(pathToService, env);
  std::string launchInfo = iviLink::Android::StringConversion::jToStd(jlaunchInfo, env);
  iviLink::Android::AppInfo appInfo(serviceRepoPath, privateDirPath, launchInfo, NULL, NULL);

	std::string trlist = iviLink::Android::StringConversion::jToStd(trlistPath, env);
	
	appObj = env->NewGlobalRef(callbacks);
	auth = new authentication::AuthenticationDialog(appInfo, jm, appObj, trlist);
  auth->init();
}


JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity_cancelClicked
  (JNIEnv *, jobject)
{
   __android_log_print(ANDROID_LOG_WARN, "authjni", "cancelClicked()");
   auth->cancelButtonClicked();
}
  
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity_setPin
  (JNIEnv * env, jobject, jstring jPin)
{
   __android_log_print(ANDROID_LOG_WARN, "authjni", "setPin()");
   std::string stdPin = iviLink::Android::StringConversion::jToStd(jPin, env);
	std::string msg = std::string("setPin(): ")+stdPin;
   __android_log_print(ANDROID_LOG_WARN, "authjni", msg.c_str());
	auth->setPin(stdPin);
}
  
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity_connectClicked
  (JNIEnv *, jobject)
{
   __android_log_print(ANDROID_LOG_WARN, "authjni", "before ok clicked");
   auth->okButtonClicked();
   __android_log_print(ANDROID_LOG_WARN, "authjni", "after ok clicked");
}
