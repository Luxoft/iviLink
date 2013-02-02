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
#include "com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile.h"

#include "Logger.hpp"
#include "ProfileHolder.hpp"
#include "NativeProfileImplementation.hpp"
#include "StringConversion.hpp"

#define CHANNEL_ID_SEPARATOR "#"

Logger apLogger = Logger::getInstance(
        "com_luxoft_ivilink_sdk_android_nonnative_NativeProfileImplementation JNI");

static void setLong(JNIEnv *env, jobject target, const char* fieldName, jlong value)
{
    LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(apLogger, "SetLong: " + convertIntegerToString(value));
    jclass cls;
    jfieldID fieldId;
    cls = env->GetObjectClass(target);
    fieldId = env->GetFieldID(cls, fieldName, "J");
    if (fieldId == NULL)
    {
        LOG4CPLUS_ERROR(apLogger, "Could not find field: " + std::string(fieldName));
        return;
    }
    env->SetLongField(target, fieldId, value);
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    createNativeProfile
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_createNativeProfile
(JNIEnv * env, jobject profileExternal, jstring profileIUID, jstring serviceID, jlong pointerToCallbacks)
{
    LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
    JavaVM * jm;
    env->GetJavaVM(&jm);
    jobject callerGlobalRef = env->NewGlobalRef(profileExternal);
    iviLink::Profile::IProfileCallbackProxy * appCallbacks = (iviLink::Profile::IProfileCallbackProxy*)(intptr_t) pointerToCallbacks;
    // type of appCallbacks has been checked in profileFactory
    NativeProfileImplementation * profileInternal = new NativeProfileImplementation(jm, callerGlobalRef, appCallbacks);
    iviLink::Profile::ProfileHolder *profileHolder = new iviLink::Profile::ProfileHolder(iviLink::Android::StringConversion::jToStd(profileIUID, env),
            iviLink::Android::StringConversion::jToStd(serviceID, env),
            profileInternal);
    setLong(env, profileExternal, "mNativeProfileInstance", (jlong)(intptr_t) profileInternal);
    setLong(env, profileExternal, "mNativeProfileHolder", (jlong)(intptr_t) profileHolder);
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    getProfileCallbacksNative
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_getProfileApiCallbacks(
        JNIEnv *, jobject, jlong nativeProfileInstance)
{
    LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(apLogger,
            "NativeProfileInstance: " + convertIntegerToString(nativeProfileInstance));
    NativeProfileImplementation * profile =
            (NativeProfileImplementation*) (intptr_t) nativeProfileInstance;
    return profile->getProfileCallbacks();
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    allocateChannel
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jstring JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_allocateChannelNative(
        JNIEnv * env, jobject, jlong nativeProfileInstance, jstring channelTag, jint priority)
{
    LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
    std::string stdTag = iviLink::Android::StringConversion::jToStd(channelTag, env);
    UInt32 channelID = 0;
    NativeProfileImplementation * profile =
            (NativeProfileImplementation*) (intptr_t) nativeProfileInstance;
    iviLink::Error result = profile->allocateChannel(stdTag, channelID, priority);
    std::string ret = convertIntegerToString(channelID) + CHANNEL_ID_SEPARATOR
            + result.serializeForJNI();
    return iviLink::Android::StringConversion::stdToJ(ret, env);
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    allocateChannelAsServer
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jstring JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_allocateChannelAsServerNative(
        JNIEnv * env, jobject, jlong nativeProfileInstance, jstring channelTag, jint priority)
{
    LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
    std::string stdTag = iviLink::Android::StringConversion::jToStd(channelTag, env);
    UInt32 channelID = 0;
    NativeProfileImplementation * profile =
            (NativeProfileImplementation*) (intptr_t) nativeProfileInstance;
    iviLink::Error result = profile->allocateChannelAsServer(stdTag, channelID, priority);
    std::string ret = convertIntegerToString(channelID) + CHANNEL_ID_SEPARATOR
            + result.serializeForJNI();
    return iviLink::Android::StringConversion::stdToJ(ret, env);
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    allocateChannelAsClient
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jstring JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_allocateChannelAsClientNative(
        JNIEnv * env, jobject, jlong nativeProfileInstance, jstring channelTag, jint priority)
{
    LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
    std::string stdTag = iviLink::Android::StringConversion::jToStd(channelTag, env);
    UInt32 channelID = 0;
    NativeProfileImplementation * profile =
            (NativeProfileImplementation*) (intptr_t) nativeProfileInstance;
    iviLink::Error result = profile->allocateChannelAsClient(stdTag, channelID, priority);
    std::string ret = convertIntegerToString(channelID) + CHANNEL_ID_SEPARATOR
            + result.serializeForJNI();
    return iviLink::Android::StringConversion::stdToJ(ret, env);
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    deallocateChannel
 * Signature: (I)V
 */
JNIEXPORT jstring JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_deallocateChannelNative(
        JNIEnv * env, jobject, jlong nativeProfileInstance, jint channelID)
{
    LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
    NativeProfileImplementation * profile =
            (NativeProfileImplementation*) (intptr_t) nativeProfileInstance;
    iviLink::Error deallocResult = profile->deallocateChannel(static_cast<UInt32>(channelID));
    return iviLink::Android::StringConversion::stdToJ(deallocResult.serializeForJNI(), env);
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    sendData
 * Signature: ([BI)V
 */
JNIEXPORT jstring JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_sendDataNative(
        JNIEnv * env, jobject, jlong nativeProfileInstance, jbyteArray toSend, jint channelID)
{
    LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
    int size = env->GetArrayLength(toSend);
    LOG4CPLUS_INFO(apLogger, "size = " + convertIntegerToString(size));
    char nativeArray[size];
    env->GetByteArrayRegion(toSend, 0, size, (jbyte*) (nativeArray));
    NativeProfileImplementation * profile =
            (NativeProfileImplementation*) (intptr_t) nativeProfileInstance;
    iviLink::Error err = profile->sendData(nativeArray, size, static_cast<UInt32>(channelID));
    return iviLink::Android::StringConversion::stdToJ(err.serializeForJNI(), env);
}
