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


#include <cassert>
#include <cerrno>
#include <limits>

#include "CThread.hpp"

#include "CIpc.hpp"
#include "CIpcSocket.hpp"
#include "ICallbackHandler.hpp"

namespace iviLink {
namespace Ipc {

Logger CIpc::logger = Logger::getInstance(LOG4CPLUS_TEXT("utils.ipc.CIpc"));

CIpc::CIpc(Address const& address, ICallbackHandler& callbackHandler) :
   mpClientHandler(&callbackHandler),
   mAddress(address),
   mSocket(NULL),
   mRequestTimeout(10000),
   mResponseTimeout(10000),
   mListenTimeout(10000)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(logger, "CIpc::CIpc() address = '" + address + "'");
}

CIpc::~CIpc()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   disconnect();
}

BaseError CIpc::connect()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   if (isConnected())
      return CIpcError::NoIpcError("Already connected");

   destroySocket();
   mSocket = new CIpcSocket(*this, false);

   BaseError err = mSocket->connect();

   return err;
}

BaseError CIpc::waitForConnection()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   if (isConnected())
      return CIpcError::NoIpcError("Already connected");

   destroySocket();

   mSocket = new CIpcSocket(*this, true);
   BaseError err = mSocket->connect();

   if (!err.isNoError())
   {
      destroySocket();
      return err;
   }

   mListenSem.waitTimeout(mListenTimeout);

   if (!mSocket->isConnected())
   {
      err = CIpcError(CIpcError::ERROR_LISTEN_TIMEOUT);
      destroySocket();
   }
   else
   {
      err = CIpcError::NoIpcError();
   }

   return err;
}

BaseError CIpc::beginWaitForConnection()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   if (isConnected())
      return CIpcError::NoIpcError("Already connected");

   destroySocket();

   mSocket = new CIpcSocket(*this, true);

   BaseError err = mSocket->setContinuousListen();
   if (!err.isNoError())
   {
      destroySocket();
      return err;
   }

   err = mSocket->connect();
   if (!err.isNoError())
   {
      destroySocket();
      return err;
   }

   return err;
}

BaseError CIpc::endWaitForConnection()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   assert(mSocket);

   BaseError err = mSocket->stopContinuousListen();

   return err;
}

void CIpc::disconnect()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   destroySocket();
}

bool CIpc::isConnected() const
{
   return mSocket && mSocket->isConnected();
}

bool CIpc::checkThread() const
{
   return !mSocket || mSocket->checkThread();
}

Address const& CIpc::getAddress() const
{
   return mAddress;
}

BaseError CIpc::request(MsgID id, 
   UInt8 const* pPayload, UInt32 payloadSize, 
   UInt8* const pResponseBuffer, UInt32& bufferSize,
   DirectionID const* const pDirId/* = NULL*/)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   if (!isConnected())
      return CIpcError(CIpcError::ERROR_COMMUNICATION, "request() error - not connected");

   if (!checkThread())
   {
      LOG4CPLUS_ERROR(logger, "request() from wrong thread");
      return CIpcError(CIpcError::ERROR_WRONG_THREAD, "This is the thread performing onRequest()");
   }

   int mutexErr = mRequestMutex.timedLock(mRequestTimeout);
   switch (mutexErr)
   {
   case 0:
      break;
   case ETIMEDOUT:
      return CIpcError(CIpcError::ERROR_REQUEST_TIMEOUT, "Cannot begin request before timeout");
   default:
      return CIpcError(CIpcError::ERROR_OTHER, strerror(errno));
   }

   tSharedSem recvSem(new CSignalSemaphore);
   if (!mRequestDataContainer.insert(id, pResponseBuffer, bufferSize, recvSem, pDirId))
   {
      mRequestMutex.unlock();
      return CIpcError(CIpcError::ERROR_INVALID_PARAMS, "MsgID already exists");
   }

   {
      BaseError err = mSocket->send(id, REQUEST, pPayload, payloadSize, pDirId);
      if (!err.isNoError())
      {
         mRequestDataContainer.erase(id);
         mRequestMutex.unlock();
         return err;
      }
   }

   LOG4CPLUS_INFO(logger, "Ipc request sent addr='" + mAddress + 
      "' MsgId="+convertIntegerToString(id) + 
      " size="+convertIntegerToString(payloadSize)+
      " dirId = " + convertIntegerToString(pDirId ? *pDirId : -1));

   if (-1 == recvSem->waitTimeout(mResponseTimeout))
   {
      BaseError err = BaseError::NoError();
      switch (errno)
      {
      case ETIMEDOUT:
         err = CIpcError(CIpcError::ERROR_RESPONSE_TIMEOUT, "Response timed out");
         break;
      default:
         err = CIpcError(CIpcError::ERROR_OTHER, strerror(errno));
         break;
      }

      LOG4CPLUS_INFO(logger, "Ipc request failed addr='" + mAddress + "' MsgId="+convertIntegerToString(id));

      mRequestDataContainer.erase(id);
      mRequestMutex.unlock();
      return err;
   }

   // success

   {
      RequestData data;
      bool res = mRequestDataContainer.getData(id, data);
      if (!res)
      {
         bufferSize = 0;
         mRequestDataContainer.erase(id);
         LOG4CPLUS_WARN(logger, "Connection was lost during request to = '" + mAddress + "' MsgId=" + convertIntegerToString(id));
         mRequestMutex.unlock();
         return CIpcError(CIpcError::ERROR_CONNECTION_LOST, "Connection lost during request");
      }

      bufferSize = data.bufferSize;
   }

   LOG4CPLUS_INFO(logger, "Ipc request done addr='" + mAddress + "' MsgId="+convertIntegerToString(id) + " size="+convertIntegerToString(bufferSize));

   mRequestDataContainer.erase(id);
   mRequestMutex.unlock();
   return CIpcError::NoIpcError();
}

BaseError CIpc::asyncRequest(MsgID id, 
   UInt8 const* pPayload, UInt32 payloadSize, 
   DirectionID const* const pDirId/* = NULL*/)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   if (!isConnected())
      return CIpcError(CIpcError::ERROR_COMMUNICATION, "request() error - not connected");

   if (!checkThread())
   {
      LOG4CPLUS_ERROR(logger, "request() from wrong thread");
      return CIpcError(CIpcError::ERROR_WRONG_THREAD, "This is the thread performing onRequest()");
   }

   int mutexErr = mRequestMutex.timedLock(mRequestTimeout);
   switch (mutexErr)
   {
   case 0:
      break;
   case ETIMEDOUT:
      return CIpcError(CIpcError::ERROR_REQUEST_TIMEOUT, "Cannot begin request before timeout");
   default:
      return CIpcError(CIpcError::ERROR_OTHER, strerror(errno));
   }

   BaseError err = mSocket->send(id, TRIGGER, pPayload, payloadSize, pDirId);
   mRequestMutex.unlock();
   return err;
}

time_t CIpc::getRequestTimeout() const
{
   return mRequestTimeout;
}

void CIpc::setRequestTimeout(time_t timeout)
{
   mRequestTimeout = timeout;
}

time_t CIpc::getResponseTimeout() const
{
   return mResponseTimeout;
}

void CIpc::setResponseTimeout(time_t timeout)
{
   mResponseTimeout = timeout;
}

time_t CIpc::getListenTimeout() const
{
   return mListenTimeout;
}

void CIpc::setListenTimeout(time_t timeout)
{
   mListenTimeout = timeout;
}

void CIpc::destroySocket()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   if (mSocket)
   {
      mSocket->stopOperations();
      delete mSocket;
      mSocket = NULL;
   }
}

void CIpc::onListenSuccess(DirectionID dirId)
{
   LOG4CPLUS_TRACE(logger, "onListenSuccess(" + convertIntegerToString(dirId) + ")");

   assert(mpClientHandler);

   mpClientHandler->OnConnection(dirId);

   mListenSem.signal();
}

void CIpc::onConnectSuccess(DirectionID dirId)
{
   LOG4CPLUS_TRACE(logger, "onConnectSuccess(" + convertIntegerToString(dirId) + ")");

   assert(mpClientHandler);

   mpClientHandler->OnConnection(dirId);
}

void CIpc::onConnectionLost(DirectionID dirId)
{
   LOG4CPLUS_TRACE(logger, "onConnectionLost(" + convertIntegerToString(dirId) + ")");

   assert(mpClientHandler);

   mpClientHandler->OnConnectionLost(dirId);
   mRequestDataContainer.invalidateRequestsForDirection(dirId);
}

void CIpc::incomingRequest(MsgID id, UInt8 const* pPayload, UInt32 payloadSize, DirectionID dirId)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   if (!mpClientHandler) 
   {
      LOG4CPLUS_FATAL(logger, "No mpClientHandler!");
      return;
   }

   mRespBufferMutex.lock();
   {
      UInt32 responseSize = RESP_BUFFER_SIZE;
      LOG4CPLUS_INFO(logger, "Ipc got request addr='" + mAddress + 
         "' MsgId=" + convertIntegerToString(id) + 
         " size=" + convertIntegerToString(payloadSize) + 
         " dirId = " + convertIntegerToString(dirId));
      
      mpClientHandler->OnRequest(id, pPayload, payloadSize, mRespBuffer, responseSize, dirId);

      BaseError err = mSocket->send(id, RESPONSE, mRespBuffer, responseSize, &dirId);
      if (err.isNoError())
      {
         LOG4CPLUS_INFO(logger, "Ipc response sent addr='" + mAddress + "' MsgId=" + convertIntegerToString(id) + " size=" + convertIntegerToString(responseSize));   
      }
      else
      {
         /// @todo error handling, if we can do something here
         LOG4CPLUS_ERROR(logger, ((std::string)err).c_str());
      }
      
   }
   mRespBufferMutex.unlock();
}

void CIpc::incomingAsyncRequest(MsgID id, UInt8 const* pPayload, UInt32 payloadSize, DirectionID dirId)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   if (!mpClientHandler) 
   {
      LOG4CPLUS_FATAL(logger, "No mpClientHandler!");
      return;
   }

   mRespBufferMutex.lock();
   {
      LOG4CPLUS_INFO(logger, "Ipc got async request addr='" + mAddress + 
         "' MsgId=" + convertIntegerToString(id) + 
         " size=" + convertIntegerToString(payloadSize) + 
         " dirId = " + convertIntegerToString(dirId));
      
      mpClientHandler->OnAsyncRequest(id, pPayload, payloadSize, dirId);
   }
   mRespBufferMutex.unlock();
}

//

bool CIpc::CRequestDataContainer::insert(MsgID id, UInt8* const pResponseBuffer, UInt32 bufferSize, tSharedSem recvSem, const DirectionID * pDirId)
{
   mRequestDataMapMutex.lock();
   {
      if (mRequestDataMap.find(id) != mRequestDataMap.end())
      {
         mRequestDataMapMutex.unlock();
         return false;
      }
      RequestData data;
      if (pDirId == NULL) 
      {
         data.mDirID = 1;
      }
      else 
      {
         data.mDirID = *pDirId;
      }
      data.bufferSize = bufferSize;
      data.pResponseBuffer = pResponseBuffer;
      data.recvSem = recvSem;
      mRequestDataMap[id] = data;
   }
   mRequestDataMapMutex.unlock();
   return true;
}

bool CIpc::CRequestDataContainer::getData(MsgID id, RequestData & data) const
{
   mRequestDataMapMutex.lock();
   LOG4CPLUS_TRACE(logger, "CRequestDataContainer::getData");
   LOG4CPLUS_TRACE(logger, "id " + convertIntegerToString(id));
   tRequestDataMap::const_iterator it = mRequestDataMap.find(id);
   if (it == mRequestDataMap.end())
   {
      LOG4CPLUS_WARN(logger, "id " + convertIntegerToString(id) + "not found");
      mRequestDataMapMutex.unlock();
      return false;
   }

   data = it->second;
   mRequestDataMapMutex.unlock();
   return true;
}

bool CIpc::CRequestDataContainer::haveData(MsgID id) const
{
   mRequestDataMapMutex.lock();
   tRequestDataMap::const_iterator it = mRequestDataMap.find(id);
   bool res = (it == mRequestDataMap.end());
   mRequestDataMapMutex.unlock();
   return res;
}

void CIpc::CRequestDataContainer::erase(MsgID id)
{
   LOG4CPLUS_TRACE(logger, "erase(" + convertIntegerToString(id) + ")");

   mRequestDataMapMutex.lock();
   tRequestDataMap::iterator it = mRequestDataMap.find(id);
   if (it == mRequestDataMap.end())
   {
      mRequestDataMapMutex.unlock();
      return;
   }

   mRequestDataMap.erase(it);
   mRequestDataMapMutex.unlock();
   return;
}

bool CIpc::CRequestDataContainer::lockData(MsgID id, RequestData*& data)
{
   LOG4CPLUS_TRACE_METHOD(logger, "lockData(" + convertIntegerToString(id) + ")");
   mRequestDataMapMutex.lock();

   tRequestDataMap::iterator it = mRequestDataMap.find(id);
   if (it == mRequestDataMap.end())
   {
      mRequestDataMapMutex.unlock();
      return false;
   }

   data = &(it->second);
   return true;
}

void CIpc::CRequestDataContainer::unlockData(MsgID id)
{
   LOG4CPLUS_TRACE_METHOD(logger, "unlockData(" + convertIntegerToString(id) + ")");
   // Currently unused
   (void)id;
   mRequestDataMapMutex.unlock();
}

void CIpc::CRequestDataContainer::invalidateRequestsForDirection(DirectionID dirId)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   mRequestDataMapMutex.lock();
   tRequestDataMap::iterator it = mRequestDataMap.begin();
   while ( it != mRequestDataMap.end())
   {
      if ((*it).second.mDirID == dirId) 
      {
         LOG4CPLUS_INFO(logger, "Found pending request for direction " + convertIntegerToString(dirId) + ", erasing");
         (*it).second.recvSem->signal();
         mRequestDataMap.erase(it++);
      } 
      else
      {
         it++;
      }
   }
   mRequestDataMapMutex.unlock();
}

}  // namespace Ipc
}  // namespace AXIS
