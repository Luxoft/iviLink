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


#include <list>

#include "com_luxoft_ivilink_sdk_ApplicationManagerWrapper.hpp"
#include "StringConversion.hpp"
#include "ApplicationManagerEntryPoint.hpp"
#include "CDatabase.hpp"
#include "AndroidLaunchInfoProcessor.hpp"
#include "AndroidLauncher.hpp"

using namespace iviLink::AMP;
using namespace iviLink::Android;

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_ApplicationManagerWrapper_startApplicationManager
(JNIEnv * env, jobject obj, jlong databaseInstance , jclass launcher)
{
    JavaVM* jm;
    env->GetJavaVM(&jm);
    AndroidLauncher * androidLauncher = new AndroidLauncher(jm, launcher);
    iviLink::AMP::appManProcessEntryPoint((CDatabase*)(intptr_t) databaseInstance, androidLauncher);
}

JNIEXPORT jlong JNICALL Java_com_luxoft_ivilink_sdk_ApplicationManagerWrapper_loadDatabase(
        JNIEnv * env, jobject caller, jstring pathToDatabase, jobject checker)
{
    JavaVM* jm;
    env->GetJavaVM(&jm);
    AndroidLaunchInfoProcessor * androidChecker = new AndroidLaunchInfoProcessor(jm, checker);
    std::string stdPathToDatabase = StringConversion::jToStd(pathToDatabase, env);
    return (jlong)(intptr_t)(new CDatabase(androidChecker, stdPathToDatabase));
}


JNIEXPORT jlong JNICALL Java_com_luxoft_ivilink_sdk_ApplicationManagerWrapper_addApplicationToDatabase
  (JNIEnv * env, jobject caller, jlong databaseInstance, jstring launchInfo, jobject servicesList)
{
    std::string stdLaunchInfo = StringConversion::jToStd(launchInfo, env);
    std::list<iviLink::Service::Uid> servicesListNative;
    if (servicesList != NULL) 
    {
        jclass claz = env->GetObjectClass(servicesList);
        jmethodID getSize = env->GetMethodID(claz, "size", "()I");
        int listSize = static_cast<int>(env->CallIntMethod(servicesList, getSize));
        jmethodID getItem = env->GetMethodID(claz, "get", "(I)Ljava/lang/Object;");
        for (int i = 0; i<listSize ; i++) 
        {
            jstring item = static_cast<jstring> (env->CallObjectMethod(servicesList, getItem, static_cast<jint>(i)));
            iviLink::Service::Uid service(iviLink::Android::StringConversion::jToStd(item, env));
            servicesListNative.push_back(service);
        }
        ((CDatabase*)(intptr_t) databaseInstance)->addApplicationAndServices(stdLaunchInfo, servicesListNative);
    }
}

JNIEXPORT jlong JNICALL Java_com_luxoft_ivilink_sdk_ApplicationManagerWrapper_checkDatabase
  (JNIEnv * env, jobject caller, jlong databaseInstance, jstring partialLaunchInfo)
{
    std::string stdPartLaunchInfo = StringConversion::jToStd(partialLaunchInfo, env);
    ((CDatabase*)(intptr_t) databaseInstance)->deleteByPartialLaunchInfo(stdPartLaunchInfo);
}
