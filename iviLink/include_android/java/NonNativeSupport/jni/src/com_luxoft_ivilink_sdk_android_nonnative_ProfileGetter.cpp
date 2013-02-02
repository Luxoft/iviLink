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
#include "com_luxoft_ivilink_sdk_android_nonnative_ProfileGetter.h"
#include "Logger.hpp"
#include "AppInfo.hpp"
#include "StringConversion.hpp"
#include "NativeProfileImplementation.hpp"
#include "CServiceManager.hpp"
/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_ProfileGetter
 * Method:    getProfile
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_ProfileGetter_getProfileNative(
        JNIEnv * env, jclass claz, jstring serviceName, jstring profileAPI)
{
    Logger log = Logger::getInstance(
            "Java_com_luxoft_ivilink_sdk_android_nonnative_ProfileGetter_getProfile");
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    iviLink::Android::AppInfo appInfo;
    std::string service = iviLink::Android::StringConversion::jToStd(serviceName, env);
    std::string api = iviLink::Android::StringConversion::jToStd(profileAPI, env);
    iviLink::BaseProfileApi * nativeProfile = iviLink::Service::CServiceManager::getInstance(
            appInfo)->getProfile(service, api);
    if (nativeProfile == NULL)
    {
        LOG4CPLUS_FATAL(log, "getProfile returned NULL");
        return NULL;
    }
    if (dynamic_cast<NativeProfileImplementation*>(nativeProfile) != NULL)
    {
        NativeProfileImplementation * nativeProfileImplementation =
                (NativeProfileImplementation*) (nativeProfile);
        return nativeProfileImplementation->getExternalProfile();
    } else
    {
        LOG4CPLUS_FATAL(log, "getProfile returned not an instance of NativeProfileImplementation");
        iviLink::Service::CServiceManager::getInstance(appInfo)->releaseProfile(service, api);
        return NULL;
    }
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_ProfileGetter
 * Method:    releaseProfile
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_ProfileGetter_releaseProfileNative
(JNIEnv * env, jclass claz, jstring serviceName, jstring profileAPI)
{
    Logger log = Logger::getInstance("Java_com_luxoft_ivilink_sdk_android_nonnative_ProfileGetter_getProfile");
    LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
    iviLink::Android::AppInfo appInfo;
    std::string service = iviLink::Android::StringConversion::jToStd(serviceName, env);
    std::string api = iviLink::Android::StringConversion::jToStd(profileAPI, env);
    iviLink::Service::CServiceManager::getInstance(appInfo)->releaseProfile(service, api);
}
