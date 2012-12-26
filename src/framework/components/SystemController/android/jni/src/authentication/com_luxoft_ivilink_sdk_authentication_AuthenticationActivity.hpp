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
 

#include <jni.h>
/* Header for class com_luxoft_ivilink_sdk_CApplicationManagerWrapper */

#ifndef _Included_com_luxoft_ivilink_sdk_auth_authact
#define _Included_com_luxoft_ivilink_sdk_auth_authact
#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity_start
  (JNIEnv *, jobject, jstring, jstring, jstring, jstring);
  
  
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity_cancelClicked
  (JNIEnv *, jobject);
  
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity_setPin
  (JNIEnv *, jobject, jstring);
  
JNIEXPORT void JNICALL Java_com_luxoft_ivilink_sdk_authentication_AuthenticationActivity_connectClicked
  (JNIEnv *, jobject);


#ifdef __cplusplus
}
#endif
#endif
