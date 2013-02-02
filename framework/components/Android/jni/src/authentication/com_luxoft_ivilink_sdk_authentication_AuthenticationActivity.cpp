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


#include <android/log.h>

#include "JniAuthenticationDialog.hpp"
#include "com_luxoft_ivilink_sdk_authentication_AuthenticationActivity.hpp"
#include "AppInfo.hpp"
#include "StringConversion.hpp"
#include "WatchdogThread.hpp"
#include "CSignalSemaphore.hpp"
#include "AuthenticationStateMachine.hpp"
#include "Components.hpp"

using namespace iviLink::Authentication;
static JavaVM * jm;

AuthenticationStateMachine * authStateMachine;  
JniAuthenticationDialog * authDialog;

static WatchdogThread watchdogThread(iviLink::SystemController::AUTHENTICATION_APP);
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    jm = jvm;
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity_start
(JNIEnv * env, jobject callbacks, jstring pathToService, jstring jlaunchInfo, jstring internalPath, jstring trlistPath)
{
  watchdogThread.start();
  std:: string privateDirPath = iviLink::Android::StringConversion::jToStd(internalPath, env);
  std::string serviceRepoPath = iviLink::Android::StringConversion::jToStd(pathToService, env);
  std::string launchInfo = iviLink::Android::StringConversion::jToStd(jlaunchInfo, env);
  iviLink::Android::AppInfo appInfo(serviceRepoPath, privateDirPath, launchInfo, NULL, NULL);
	std::string trlist = iviLink::Android::StringConversion::jToStd(trlistPath, env);
  authStateMachine = new AuthenticationStateMachine(trlist, appInfo);
  authDialog = new JniAuthenticationDialog(jm, callbacks);
  authStateMachine->startAuthentication(authDialog);
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity_cancelClicked
(JNIEnv *, jobject)
{
    __android_log_print(ANDROID_LOG_INFO, "Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity", "cancelClicked()");
    authStateMachine->cancelAuthentication();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity_sendPin
(JNIEnv * env, jobject, jint firstDigit, jint secondDigit, jint thirdDigit, jint fourthDigit)
{
    __android_log_print(ANDROID_LOG_INFO, "Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity", "setPin()" );
    authStateMachine->sendPin(firstDigit, secondDigit, thirdDigit, fourthDigit);
}
