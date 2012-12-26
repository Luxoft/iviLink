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
 
#ifndef ANDROID
#else

#include <cassert>

#include "CAndroidBluetoothAdapter.hpp"
#include "GlobalJavaVmGetter.h"
#include "BluetoothObject.h"

using namespace iviLink::ConnectivityAgent::HAL;
Logger CAndroidBluetoothAdapter::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CAndroidBluetoothCarrierAdapter"));

CAndroidBluetoothAdapter::~CAndroidBluetoothAdapter()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
}

bool CAndroidBluetoothAdapter::isBroken()  const
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   iviLink::Android::JniThreadHelper jth(mpJM);
   JNIEnv * env = jth.getEnv();

   return (bool)env->CallBooleanMethod(jImplementation, midIsBroken);
}

ERROR_CODE CAndroidBluetoothAdapter::makeHandshake()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    ERROR_CODE ret = ERR_OK;
    // TODO some stuff like in tcp adapter
    if(mConnectionInfo.isServer){
      mGenderType = eServerGender;
  	} else {
      mGenderType = eClientGender;
	  }
    return ERR_OK;
}
 
const char* CAndroidBluetoothAdapter::getRemoteAddress()  const
{
  LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	return NULL; // TODO get names from Java code???
}

const char* CAndroidBluetoothAdapter::getLocalAddress()  const
{
  LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	return NULL; // TODO get names from Java code???
}

const char* CAndroidBluetoothAdapter::getTypeName()  const
{
  LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	return "Bluetooth/Android";
}

ERROR_CODE CAndroidBluetoothAdapter::sendPrepearedArray(const UInt8* pArray, UInt32 size)
{
   if (!pArray || (size <= 0) )
   {
	   LOG4CPLUS_ERROR(logger, "sendPrepearedArray called with incorrect pArray or size");
	   return ERR_FAIL;
   }
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   iviLink::Android::JniThreadHelper jth(mpJM);
   JNIEnv * env = jth.getEnv();
   jbyteArray param = env->NewByteArray(size); 
   LOG4CPLUS_INFO(logger, "created byte array ok");
   env->SetByteArrayRegion(param, 0, size, (jbyte*)pArray);
   LOG4CPLUS_INFO(logger, "set byte array region");
   bool result = (bool)env->CallBooleanMethod(jImplementation, send, param);
   LOG4CPLUS_INFO(logger, "called send method");
   if(result) return ERR_OK;
   return ERR_FAIL;
}

ERROR_CODE CAndroidBluetoothAdapter::receiveRawArray(UInt8* pArray, UInt32 &size)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   iviLink::Android::JniThreadHelper jth(mpJM);
   JNIEnv * env = jth.getEnv();
   LOG4CPLUS_INFO(logger, "about to call the receive method");
   jbyteArray arr = static_cast<jbyteArray>(env->CallObjectMethod(jImplementation, receive, size));
   if (!arr || (size <= 0) )
   {
	   LOG4CPLUS_ERROR(logger, "receiveRawArray: incorrect array or size");
	   return ERR_FAIL;
   }
   LOG4CPLUS_INFO(logger, "called the receive method");
   size = env->GetArrayLength(arr);
   env->GetByteArrayRegion(arr, 0, size,(jbyte*)pArray);

   return ERR_OK;
}

bool CAndroidBluetoothAdapter::start()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   return CThread::start();
}

bool CAndroidBluetoothAdapter::stop()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   return CThread::stop();
}

void CAndroidBluetoothAdapter::threadFunc()
{
   	LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    // TODO ???
   	while(true){
   		haveData();
   	}
}

CAndroidBluetoothAdapter::CAndroidBluetoothAdapter(const CAndroidBluetoothInfo & connectionInfo) :
   CThread("CAndroidBluetoothCarrierAdapter", true),
   mConnectionInfo(connectionInfo)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   jImplementation = getBluetoothHelperInstance();
   mpJM = globalGetJavaVM();
   iviLink::Android::JniThreadHelper jth(mpJM);
   JNIEnv * env = jth.getEnv();
   jclass claz = env->GetObjectClass(jImplementation);
   receive = env->GetMethodID(claz, "readData", "(I)[B"); // returns read byte array
   assert(receive>=0);
   send = env->GetMethodID(claz, "sendData", "([B)Z"); // returns true if ok
   assert(send>=0);

   midIsBroken = env->GetMethodID(claz, "isBroken", "()Z"); // returns true if ok
   start();
}
#endif //ANDROID
