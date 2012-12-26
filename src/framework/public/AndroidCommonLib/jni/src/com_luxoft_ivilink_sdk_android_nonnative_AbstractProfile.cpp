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
 
#include "com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile.h"

#include "Logger.hpp"
#include "CProfile.hpp"
#include "CProfileImplementation.hpp"
#include "StringConversion.hpp"

Logger apLogger = Logger::getInstance("com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile JNI");
iviLink::Profile::CProfile * profileInternalWrapper;
CProfileImplementation * profileInternal;
/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    createNativeProfile
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_createNativeProfile
  (JNIEnv * env, jobject profileExternal, jstring profileIUID, jstring serviceID, jbyteArray pointerToCallbacks)
{
	LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
	JavaVM * jm;
	env->GetJavaVM(&jm);
	jobject callerGlobalRef = env->NewGlobalRef(profileExternal);
	int size = env->GetArrayLength(pointerToCallbacks);
	char nativeArray[size];
	env->GetByteArrayRegion(pointerToCallbacks, 0, size, (jbyte*)(nativeArray));
	iviLink::Profile::IProfileCallbackProxy * appCallbacks;
	memcpy(&appCallbacks, nativeArray, sizeof(sizeof(iviLink::Profile::IProfileCallbackProxy*)));
	// type of appCallbacks has been checked in profileFactory
	profileInternal = new CProfileImplementation(jm, callerGlobalRef, appCallbacks);
	profileInternalWrapper = new iviLink::Profile::CProfile(iviLink::Android::StringConversion::jToStd(profileIUID, env),
										  iviLink::Android::StringConversion::jToStd(serviceID, env),
										  profileInternal);
}


/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    getNativeCProfileInstance
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_getNativeCProfileInstance
  (JNIEnv * env, jobject)
{
	LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
	jbyteArray retval = env->NewByteArray(sizeof(iviLink::Profile::CProfile*)); 
	char serializedPointer[sizeof(iviLink::Profile::CProfile*)];
	memcpy(serializedPointer, &profileInternalWrapper, sizeof(iviLink::Profile::CProfile*));
    env->SetByteArrayRegion(retval, 0, sizeof(iviLink::Profile::CProfile*), (jbyte*)serializedPointer);
    return retval;
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    getProfileCallbacksNative
 * Signature: (Ljava/lang/String;)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_getProfileCallbacks
  (JNIEnv *, jobject)
{
	LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
	return profileInternal->getProfileCallbacks();
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    allocateChannel
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_allocateChannel
  (JNIEnv * env, jobject, jstring channelTag)
{
	LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
	std::string stdTag = iviLink::Android::StringConversion::jToStd(channelTag, env);
	LOG4CPLUS_INFO(apLogger, "will allocate channel: " + stdTag);
	return static_cast<jint>(profileInternal->allocateChannel(stdTag));
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    allocateChannelAsServer
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_allocateChannelAsServer
  (JNIEnv * env, jobject, jstring channelTag)
{
	LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
	std::string stdTag = iviLink::Android::StringConversion::jToStd(channelTag, env);
	LOG4CPLUS_INFO(apLogger, "will allocate channel: " + stdTag);
	return static_cast<jint>(profileInternal->allocateChannelAsServer(stdTag));
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    allocateChannelAsClient
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_allocateChannelAsClient
  (JNIEnv * env, jobject, jstring channelTag)
{
	LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
	std::string stdTag = iviLink::Android::StringConversion::jToStd(channelTag, env);
	LOG4CPLUS_INFO(apLogger, "will allocate channel: " + stdTag);
	return static_cast<jint>(profileInternal->allocateChannelAsClient(stdTag));
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    deallocateChannel
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_deallocateChannel
  (JNIEnv *, jobject, jint channelID)
{
	LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
	profileInternal->deallocateChannel(static_cast<unsigned int>(channelID));
}

/*
 * Class:     com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile
 * Method:    sendData
 * Signature: ([BI)V
 */
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_android_nonnative_AbstractProfile_sendData
  (JNIEnv * env, jobject, jbyteArray toSend, jint channelID)
{
	LOG4CPLUS_TRACE_METHOD(apLogger, __PRETTY_FUNCTION__);
	int size = env->GetArrayLength(toSend);
	LOG4CPLUS_INFO(apLogger, "size = " + convertIntegerToString(size));
	char nativeArray[size];
	env->GetByteArrayRegion(toSend, 0, size, (jbyte*)(nativeArray));
	profileInternal->sendData(nativeArray, size, static_cast<unsigned int>(channelID));
}
