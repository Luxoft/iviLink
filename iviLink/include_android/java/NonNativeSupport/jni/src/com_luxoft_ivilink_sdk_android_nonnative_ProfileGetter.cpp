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
