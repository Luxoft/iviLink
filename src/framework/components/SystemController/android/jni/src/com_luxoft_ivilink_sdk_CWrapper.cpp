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

#include "framework/components/SystemController/android/jni/src/com_luxoft_ivilink_sdk_CWrapper.hpp"
#include "framework/components/SystemController/ssm/stateMachine/CSystemStateMachine.hpp"
#include "utils/misc/Logger.hpp"
#include "utils/android/MakeRequest.hpp"

#include <string>

using namespace SystemController;

//static std::string intentFilter;

static JavaVM * jm;
static jobject gLauncher;
static jmethodID gLaunchMethod;

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_CWrapper_initSSM
  (JNIEnv * env, jobject, jobject launcher, jboolean isAuthOn)
{
	//const char *exJString = env->GetStringUTFChars(intents, NULL);
	//intentFilter = std::string(exJString);
	env->GetJavaVM(&jm);
	gLauncher  = env->NewGlobalRef(launcher);
	jclass launcherCls = env->GetObjectClass(gLauncher);
	gLaunchMethod = env->GetMethodID(launcherCls, "answerRequest", "(I)V");
	iviLink::Android::setup(jm, gLauncher, gLaunchMethod);
	CSystemStateMachine* ssm = new CSystemStateMachine();
	bool authStatus = (bool)isAuthOn;
	ssm->init(true); //TODO implement auth
	while(true)
	{
		sleep(100);
	}
}

