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

#include "framework/components/SystemController/android/jni/src/com_luxoft_ivilink_sdk_CApplicationManagerWrapper.hpp"
#include "framework/components/AppMan/controller/CController.hpp"
#include "framework/components/AppMan/process/SystemControllerMsg.hpp"
#include "framework/libraries/AppMan/AmpForPmp/CPmpController.hpp"

#include "utils/misc/Logger.hpp"

#include <string>

JavaVM * jm;
jclass jLauncher;
jmethodID launchApp;

using iviLink::AppMan::Ipc::CPmpController;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) {
	jm = jvm;
	return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_CApplicationManagerWrapper_startAM
(JNIEnv * env, jobject obj, jstring pathToDatabase, jobject launcher)
{
	Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.main"));
	LOG4CPLUS_INFO(logger, "AMP started...");

	const char *filename = env->GetStringUTFChars(pathToDatabase, NULL);
	LOG4CPLUS_INFO(logger, "DataBase is in: "+std::string(filename));
	jLauncher=env->GetObjectClass(launcher);
	jLauncher = (jclass)env->NewGlobalRef(jLauncher);
	launchApp = env->GetStaticMethodID(jLauncher, "startApplication","(Ljava/lang/String;)I");

	CSignalSemaphore sem;
	iviLink::AMP::CController ampController;
	ampController.init(std::string(filename), jm, jLauncher, launchApp);
	iviLink::AMP::SystemControllerMsg sysCtrl(&sem);

	bool connEstablished = false;
	bool needExit = false;
	while (true)
	{
		CPmpController* pmpctrl = CPmpController::instance();
		if (!pmpctrl)
		{
			LOG4CPLUS_INFO(logger, "!pmpctrl");
			needExit = true;
			break;
		}
		if (pmpctrl->checkConnection())
		{
			LOG4CPLUS_INFO(logger, "conn established");
			connEstablished = true;
			break;
		}
		if (0 == sem.tryWait())
		{
			LOG4CPLUS_INFO(logger, "semtrywait() == 0");
			needExit = true;
			break;
		}

		// waiting for app man connection
		sleep(1);
	}

	if (connEstablished) sysCtrl.requestConnected();

	/**
	 * waiting for exit by signal of system controller
	 */
	if (!needExit) {
		LOG4CPLUS_INFO(logger, "needexit == false");
		sem.wait();
	}
	ampController.uninit();
}
