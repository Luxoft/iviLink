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
#include "GlobalJavaVmGetter.h"
#include "BluetoothObject.h"

#include <jni.h>
#include <cassert>
#include <android/log.h>

jobject gBlueHelper;
jobject getBluetoothHelperInstance()
{
    __android_log_print(ANDROID_LOG_INFO, "iviLink.ConnectivityAgent.Android.BluetoothStorage", "BluetoothHelper requested!");
    return gBlueHelper;
};

void storeObject(jobject obj)
{
   __android_log_print(ANDROID_LOG_INFO, "iviLink.ConnectivityAgent.Android.BluetoothStorage", "ENTER: storeObject");
   gBlueHelper = obj;  
   __android_log_print(ANDROID_LOG_INFO, "iviLink.globalJavaVMGetter", "BluetoothHelper object stored successfully!");
   __android_log_print(ANDROID_LOG_INFO, "iviLink.globalJavaVMGetter", "EXIT: storeObject");
}


JavaVM * gJM;
JavaVM * globalGetJavaVM()
{
    __android_log_print(ANDROID_LOG_INFO, "iviLink.globalJavaVMGetter", "gJM requested!");
    return gJM;
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved) 
{
    __android_log_print(ANDROID_LOG_INFO, "iviLink.globalJavaVMGetter", "ENTER: JNI_OnLoad");
    gJM = jvm;  
    assert(gJM);
    __android_log_print(ANDROID_LOG_INFO, "iviLink.globalJavaVMGetter", "gJM cached successfully!");
    __android_log_print(ANDROID_LOG_INFO, "iviLink.globalJavaVMGetter", "EXIT: JNI_OnLoad");
	return JNI_VERSION_1_6;
}
