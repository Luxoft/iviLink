/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */

#include <cassert>
#include <cerrno>
#include <limits>

#include "utils/threads/include/CThread.hpp"

#include "../include/CIpc.hpp"
#include "../include/CIpcSocket.hpp"
#include "../include/ICallbackHandler.hpp"

namespace AXIS {
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
   LOG4CPLUS_INFO(logger, "CIpc::CIpc() address = '" + address + "'");
}

CIpc::~CIpc()
{
   disconnect();
}

CError CIpc::connect()
{
   if (isConnected())
      return CIpcError::NoIpcError("Already connected");

   destroySocket();
   mSocket = new CIpcSocket(*this, false);

   CError err = mSocket->connect();

   return err;
}

CError CIpc::waitForConnection()
{
   if (isConnected())
      return CIpcError::NoIpcError("Already connected");

   destroySocket();

   mSocket = new CIpcSocket(*this, true);
   CError err = mSocket->connect();

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

CError CIpc::beginWaitForConnection()
{
   if (isConnected())
      return CIpcError::NoIpcError("Already connected");

   destroySocket();

   mSocket = new CIpcSocket(*this, true);

   CError err = mSocket->setContinuousListen();
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

CError CIpc::endWaitForConnection()
{
   assert(mSocket);

   CError err = mSocket->stopContinuousListen();

   return err;
}

void CIpc::disconnect()
{
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

CError CIpc::request(MsgID id, 
   UInt8 const* pPayload, UInt32 payloadSize, 
   UInt8* const pResponseBuffer, UInt32& bufferSize,
   DirectionID const* const pDirId/* = NULL*/)
{
   LOG4CPLUS_TRACE(logger, "request()");

   if (!isConnected())
      return CIpcError(CIpcError::ERROR_COMMUNICATION);

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
   if (!mRequestDataContainer.insert(id, pResponseBuffer, bufferSize, recvSem))
   {
      mRequestMutex.unlock();
      return CIpcError(CIpcError::ERROR_INVALID_PARAMS, "MsgID already exists");
   }

   {
      CError err = mSocket->send(id, true, pPayload, payloadSize, pDirId);
      if (!err.isNoError())
      {
         mRequestDataContainer.erase(id);
         mRequestMutex.unlock();
         return err;
      }
   }

   LOG4CPLUS_INFO(logger, "Ipc request sent addr='" + mAddress + "' MsgId="+convertIntegerToString(id) + " size="+convertIntegerToString(payloadSize));

   if (-1 == recvSem->waitTimeout(mResponseTimeout))
   {
      CError err = CError::NoError();
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
      assert(res);

      bufferSize = data.bufferSize;
   }

   LOG4CPLUS_INFO(logger, "Ipc request done addr='" + mAddress + "' MsgId="+convertIntegerToString(id) + " size="+convertIntegerToString(bufferSize));

   mRequestDataContainer.erase(id);
   mRequestMutex.unlock();
   return CIpcError::NoIpcError();
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
   LOG4CPLUS_TRACE(logger, "destroySocket");
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
}

void CIpc::incomingRequest(MsgID id, UInt8 const* pPayload, UInt32 payloadSize, DirectionID dirId)
{
   LOG4CPLUS_TRACE(logger, "incomingRequest()");

   if (!mpClientHandler)
      return;

   mRespBufferMutex.lock();
   {
      UInt32 responseSize = RESP_BUFFER_SIZE;
      LOG4CPLUS_INFO(logger, "Ipc got request addr='" + mAddress + "' MsgId=" + convertIntegerToString(id) + " size=" + convertIntegerToString(payloadSize));
      mpClientHandler->OnRequest(id, pPayload, payloadSize, mRespBuffer, responseSize, dirId);

      CError err = mSocket->send(id, false, mRespBuffer, responseSize, &dirId);
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

//

bool CIpc::CRequestDataContainer::insert(MsgID id, UInt8* const pResponseBuffer, UInt32 bufferSize, tSharedSem recvSem)
{
   LOG4CPLUS_TRACE(logger, "insert()");

   mRequestDataMapMutex.lock();
   {
      if (mRequestDataMap.find(id) != mRequestDataMap.end())
      {
         mRequestDataMapMutex.unlock();
         return false;
      }
      RequestData data;
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
      LOG4CPLUS_TRACE(logger, "id " + convertIntegerToString(id) + "not found");
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
   LOG4CPLUS_TRACE(logger, "lockData()");

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
   LOG4CPLUS_TRACE(logger, "nlockData(" + convertIntegerToString(id) + ")");
   // Currently unused
   (void)id;
   mRequestDataMapMutex.unlock();
}

}  // namespace Ipc
}  // namespace AXIS
