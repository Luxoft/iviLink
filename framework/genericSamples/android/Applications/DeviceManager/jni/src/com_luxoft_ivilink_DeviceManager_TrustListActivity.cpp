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

#include <android/log.h>

#include "JniDeviceManagerDialog.hpp"
#include "com_luxoft_ivilink_DeviceManager_TrustListActivity.hpp"
#include "AppInfo.hpp"
#include "StringConversion.hpp"
#include "DeviceManager.hpp"

using namespace iviLink::DeviceManager;
static JavaVM * jm;

DeviceManager * deviceManager;
JniDeviceManagerDialog * deviceManagerDialog;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    __android_log_print(ANDROID_LOG_INFO, "Java_com_luxoft_ivilink_TrustListActivity", "JNI_OnLoad");

    jm = jvm;
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_DeviceManager_TrustListActivity_start
  (JNIEnv * env, jobject callbacks, jstring trustListPath)
{
  deviceManager = new DeviceManager(iviLink::Android::StringConversion::jToStd(trustListPath, env));
  deviceManagerDialog = new JniDeviceManagerDialog(jm, callbacks);

  deviceManagerDialog->setDeviceManager(deviceManager);
  deviceManager->setGUI(deviceManagerDialog);

  deviceManager->start();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_DeviceManager_TrustListActivity_removeDeviceFromList
  (JNIEnv *, jobject, jint index)
{
    __android_log_print(ANDROID_LOG_INFO, "Java_com_luxoft_ivilink_TrustListActivity", "removeDeviceFromList");
    deviceManager->removeDeviceFromList(index);
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_DeviceManager_TrustListActivity_trustListClear
  (JNIEnv *, jobject)
{
    __android_log_print(ANDROID_LOG_INFO, "Java_com_luxoft_ivilink_TrustListActivity", "trustListClear");
    deviceManager->clearTrustList();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_DeviceManager_TrustListActivity_disconnectDevice
  (JNIEnv *, jobject, jint index)
{
    __android_log_print(ANDROID_LOG_INFO, "Java_com_luxoft_ivilink_TrustListActivity", "disconnectDevice");
    deviceManager->disconnectDevice(index);
}

JNIEXPORT bool JNICALL Java_com_luxoft_ivilink_sdk_DeviceManager_TrustListActivity_isConnected
  (JNIEnv *, jobject)
{
    __android_log_print(ANDROID_LOG_INFO, "Java_com_luxoft_ivilink_TrustListActivity", "isConnected");
    return deviceManager->isConnected();
}

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_DeviceManager_TrustListActivity_trustDbReload
  (JNIEnv *, jobject)
{
    __android_log_print(ANDROID_LOG_INFO, "Java_com_luxoft_ivilink_TrustListActivity", "trustDbReload");
    deviceManager->start();
}
