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
 
#include <list>

#include "com_luxoft_ivilink_sdk_android_nonnative_CApp.h"
#include "AppInfo.hpp"
#include "StringConversion.hpp"
#include "Logger.hpp"
#include "CAppImplementation.hpp"
#include "UID.hpp"
#include "NonnativeCallbacksWrapper.hpp"

Logger log = Logger::getInstance("com_luxoft_ivilink_sdk_android_nonnative_CApp JNI");
CAppImplementation * appInternal;

JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_CApp_createAppNative
  (JNIEnv * env, jobject appExternal, jstring servicePath, jstring internalPath, jstring launchInfo, jobject serviceUIDs, jclass profileCreator)
{
  LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
  iviLink::Android::AppInfo appInfo;
  appInfo.serviceRepoPath = iviLink::Android::StringConversion::jToStd(servicePath, env);
  appInfo.privateDirPath = iviLink::Android::StringConversion::jToStd(internalPath, env);
  appInfo.launchInfo = iviLink::Android::StringConversion::jToStd(launchInfo, env);
  appInfo.isValid = true;
  JavaVM * jvm;
  env->GetJavaVM(&jvm);
  appInfo.javaVM = jvm;
  appInfo.profileCreatorClass = (jclass) env->NewGlobalRef(profileCreator);
  std::list<iviLink::Service::Uid> serviceUIDsNative;
  if (serviceUIDs != NULL) {
    jclass claz = env->GetObjectClass(serviceUIDs);
    jmethodID getSize = env->GetMethodID(claz, "size", "()I");
    int listSize = static_cast<int>(env->CallIntMethod(serviceUIDs, getSize));
    LOG4CPLUS_INFO(log, "list size is: "+ convertIntegerToString(listSize));
    jmethodID getItem = env->GetMethodID(claz, "get", "(I)Ljava/lang/Object;");
    for (int i = 0; i< listSize ; i++) {
      jstring item = static_cast<jstring> (env->CallObjectMethod(serviceUIDs, getItem, static_cast<jint>(i)));
      iviLink::Service::Uid service(iviLink::Android::StringConversion::jToStd(item, env));
      LOG4CPLUS_INFO(log, "service: " + service.value());
      serviceUIDsNative.push_back(service);
    }
  }
  jobject globalRefAppExternal = env->NewGlobalRef(appExternal);
  appInternal = new CAppImplementation(appInfo, serviceUIDsNative, globalRefAppExternal);
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_CApp
 * Method:    useServiceNative
 * Signature: (Ljava/lang/String;Z)V
 */
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_CApp_useServiceNative
  (JNIEnv * env, jobject, jstring serviceUID, jboolean enabled)
{
  LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
  iviLink::Service::Uid service(iviLink::Android::StringConversion::jToStd(serviceUID, env));
  appInternal->setEnabled(service, static_cast<bool>(enabled));
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_CApp
 * Method:    registerProfileCallbacksNative
 * Signature: (Ljava/lang/String;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_CApp_registerProfileCallbacksNative
  (JNIEnv * env, jobject, jstring profileAPI, jobject callbacks)
{
  LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
  // this ref is never freed actually - implement when unregistering mechanism is provided
  jobject callbacksRef = env->NewGlobalRef(callbacks);
  std::string profileApiStd = iviLink::Android::StringConversion::jToStd(profileAPI, env);
  NonnativeCallbacksWrapper * fakeCallbacks = new NonnativeCallbacksWrapper(callbacksRef);
  appInternal->registerProfileCallbacks(iviLink::Profile::ApiUid(profileApiStd), fakeCallbacks);
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_CApp
 * Method:    initInIviLinkNative
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_CApp_initInIviLinkNative
  (JNIEnv *, jobject)
{
  LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
  appInternal->initInIVILink();
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_CApp
 * Method:    getLaunchInfoNative
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_CApp_getLaunchInfoNative
  (JNIEnv *, jobject)
{
  LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
  iviLink::ELaunchInfo launcher = appInternal->getLaunchInfo();
  return static_cast<jboolean>(launcher ==  iviLink::LAUNCHED_BY_USER);
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_CApp
 * Method:    loadServiceNative
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_CApp_loadServiceNative
  (JNIEnv * env, jobject, jstring serviceToLoad)
{
  LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
  iviLink::Service::Uid service(iviLink::Android::StringConversion::jToStd(serviceToLoad, env));
  return static_cast<jboolean>(appInternal->loadService(service));
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_CApp
 * Method:    unloadServiceNative
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_CApp_unloadServiceNative
  (JNIEnv * env, jobject, jstring serviceToUnload)
{
  LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
  iviLink::Service::Uid service(iviLink::Android::StringConversion::jToStd(serviceToUnload, env));
  appInternal->unloadService(service);
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_CApp
 * Method:    getActiveServicesListSizeNative
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_CApp_getActiveServicesListSizeNative
  (JNIEnv *, jobject)
{
  LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
  iviLink::Service::ListOfUids activeServices = appInternal->getActiveServices();
  return static_cast<jint>(activeServices.size());
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_CApp
 * Method:    getActiveServiceByListPositionNative
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_CApp_getActiveServiceByListPositionNative
  (JNIEnv * env, jobject, jint position)
{
  LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
  iviLink::Service::ListOfUids activeServices = appInternal->getActiveServices();
  if (position < activeServices.size()) 
  {
    iviLink::Service::ListOfUids::iterator it = activeServices.begin();
    std::advance(it, position);
    return iviLink::Android::StringConversion::stdToJ(it->value(), env);
  }
  else
  {
    return NULL;
  }
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_CApp
 * Method:    isServiceActiveNative
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_CApp_isServiceActiveNative
  (JNIEnv * env, jobject, jstring serviceToCheck)
{
  LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
  iviLink::Service::Uid service(iviLink::Android::StringConversion::jToStd(serviceToCheck, env));
  return static_cast<jboolean>(appInternal->isActive(service));
}

JNIEXPORT jboolean JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_CApp_isLinkAliveNative
  (JNIEnv * env, jobject)
{
  LOG4CPLUS_TRACE_METHOD(log, __PRETTY_FUNCTION__);
  return static_cast<jboolean>(appInternal->isLinkAlive());
}
