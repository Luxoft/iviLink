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
#ifndef ANDROID
#else

#include <cassert>

#include "CAndroidBluetoothAdapter.hpp"
#include "GlobalJavaVmGetter.h"
#include "BluetoothObject.h"

using namespace iviLink::ConnectivityAgent::HAL;
using namespace iviLink::ConnectivityAgent;
Logger CAndroidBluetoothAdapter::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.HAL.CAndroidBluetoothCarrierAdapter"));

CAndroidBluetoothAdapter::~CAndroidBluetoothAdapter()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   // TODO release global references
}

bool CAndroidBluetoothAdapter::isBroken()  const
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   iviLink::Android::JniThreadHelper jth(mpJM);
   JNIEnv * env = jth.getEnv();
   return (bool)env->CallBooleanMethod(jImplementation, midIsBroken);
}

ConnectivityAgentError  CAndroidBluetoothAdapter::makeHandshake()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    // TODO when Ubuntu's bluetooth is done
    if(mConnectionInfo.isServer){
      mGenderType = eServerGender;
  	} else {
      mGenderType = eClientGender;
	  }
    return ConnectivityAgentError::NoError();
}
 
const char* CAndroidBluetoothAdapter::getRemoteAddress()  const
{
  LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	return NULL; // TODO get name or MAC from Java code
}

const char* CAndroidBluetoothAdapter::getLocalAddress()  const
{
  LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	return NULL; // TODO get name or MAC from Java code
}

const char* CAndroidBluetoothAdapter::getTypeName()  const
{
  LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
	return "Bluetooth";
}

ConnectivityAgentError CAndroidBluetoothAdapter::sendPreparedArray(const UInt8* pArray, const UInt32 size)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   if (!pArray || (size <= 0) )
   {
	   LOG4CPLUS_ERROR(logger, "sendPreparedArray called with incorrect pArray or size");
	   return ConnectivityAgentError(ConnectivityAgentError::ERROR_INCONSISTENT_DATA);
   }
   iviLink::Android::JniThreadHelper jth(mpJM);
   JNIEnv * env = jth.getEnv();
   jbyteArray param = env->NewByteArray(size); 
   LOG4CPLUS_INFO(logger, "send:: created byte array");
   env->SetByteArrayRegion(param, 0, size, (jbyte*)pArray);
   LOG4CPLUS_INFO(logger, "send:: set byte array region");
   bool result = (bool)env->CallBooleanMethod(jImplementation, send, param);
   LOG4CPLUS_INFO(logger, "send:: called java send method");
   if(result) return ConnectivityAgentError::NoError();
   return ConnectivityAgentError(ConnectivityAgentError::ERROR_BROKEN_PIPE);
}

ConnectivityAgentError CAndroidBluetoothAdapter::receiveRawArray(UInt8* pArray, UInt32 &size)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   iviLink::Android::JniThreadHelper jth(mpJM);
   JNIEnv * env = jth.getEnv();
   LOG4CPLUS_INFO(logger, "about to call the receive method");
   jbyteArray arr = static_cast<jbyteArray>(env->CallObjectMethod(jImplementation, receive, size));
   if (!arr || (size <= 0) )
   {
	   LOG4CPLUS_ERROR(logger, "receiveRawArray: incorrect array or size");
     return ConnectivityAgentError(ConnectivityAgentError::ERROR_BROKEN_PIPE);
   }
   LOG4CPLUS_INFO(logger, "called the receive method");
   size = env->GetArrayLength(arr);
   env->GetByteArrayRegion(arr, 0, size,(jbyte*)pArray);
   return ConnectivityAgentError::NoError();
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

CAndroidBluetoothAdapter::CAndroidBluetoothAdapter(const CBluetoothInfo & connectionInfo) :
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
