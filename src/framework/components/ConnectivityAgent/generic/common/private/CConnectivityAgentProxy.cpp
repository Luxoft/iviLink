/* 
 * 
 * iviLINK SDK, version 1.0.1
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
 * 
 */








/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <assert.h>
#include <utility>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "utils/ipc/CIpc.hpp"
 /********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CConnectivityAgentProxy.hpp"
/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "utils/misc/Logger.hpp"
#include "utils/misc/byteOrder.hpp"
#include "API.hpp"
#include "CallbackWrappers.hpp"

#include "framework/components/ConnectivityAgent/linux/config.hpp"

using iviLink::Ipc::CIpc;
using iviLink::ConnectivityAgent::Buffer;
using iviLink::ConnectivityAgent::CDataAccessor;

CMutex CConnectivityAgentProxy::mSingletonMutex;
CConnectivityAgentProxy* CConnectivityAgentProxy::mSelf = 0;

/// @todo for testing
char const * gpCA_UNIX_SOCK_PATH = 0;

Logger CConnectivityAgentProxy::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.common.CConnectivityAgentProxy"));


CConnectivityAgentProxy::CConnectivityAgentProxy(): 
   CThread("CallbackTread"),
   mpIpc(NULL)
{
   static char const* const arr[] = 
   {
      gpCA_UNIX_SOCK_PATH, first_lsn_path, second_lsn_path
   };

   for (size_t i = 0; i < sizeof(arr)/sizeof(arr[0]); ++i)
   {
      if (arr[i])
      {
         LOG4CPLUS_INFO(logger, "Trying to connect to connectivity agent using address " + std::string(arr[i]));
         CIpc* pipc = new CIpc(arr[i], *this);
         CError err = pipc->connect();
         if (err.isNoError())
         {
            mpIpc = pipc;
            break;
         }
         else
         {
            LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
            delete pipc;
         }
      }
   }

   if (!mpIpc)
   {
      LOG4CPLUS_ERROR(logger, "Unable to connect to connectivity agent");
   }
   
   start();
}

CConnectivityAgentProxy::~CConnectivityAgentProxy()
{
   setStopFlag();
   delete mpIpc;
}


CConnectivityAgentProxy* CConnectivityAgentProxy::getInstance()
{
   mSingletonMutex.lock();
   if(mSelf == NULL)
   {
      mSelf = new CConnectivityAgentProxy();
   }
   mSingletonMutex.unlock();
   return mSelf;
}

void CConnectivityAgentProxy::deleteInstance()
{
   delete this;
   mSelf = NULL;
}

void CConnectivityAgentProxy::OnConnection(iviLink::Ipc::DirectionID dirId)
{
   LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::OnConnection dirId = "
      + convertIntegerToString(dirId));
}

void CConnectivityAgentProxy::OnConnectionLost(iviLink::Ipc::DirectionID dirId)
{
   LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::onConnectionLost dirId = "
      + convertIntegerToString(dirId));
}

ERROR_CODE CConnectivityAgentProxy::allocateChannel(TChannelPriority prio, UInt32 channel_id, IChannelObserver* observer)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::allocateChannel(type = "
         + convertIntegerToString((int)prio) + ", id = "
         + convertIntegerToString(channel_id) + " ), this = " + convertIntegerToString((intptr_t)this));

   ERROR_CODE ret = ERR_FAIL;
   if((channel_id>0)&&(channel_id <=0xFFFF)&&(NULL != observer))
   {
      mRegistryMutex.lockWrite();
      bool channel_unknown = mRegistry.find(channel_id) == mRegistry.end();
      mRegistryMutex.unlock();
      if (channel_unknown)
      {
         CDataAccessor requestDA;   
         requestDA.setChannelID(channel_id);
         requestDA.setOpCode(E_ALLOCATE_CHANNEL);
         UInt32 data = prio;
         requestDA.setData(reinterpret_cast<UInt8 *>(&data),sizeof(UInt32) );
         UInt8* buf = new UInt8[requestDA.getObjectSize()];
         requestDA.copyToRawArray(buf);

         UInt8 respBuf[100];
         UInt32 respSize = sizeof(respBuf);
         CError err = mpIpc->request(mMsgIdGen.next(), buf, requestDA.getObjectSize(), respBuf, respSize);
         delete [] buf;
         if (!err.isNoError())
         {
            LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
            ret = ERR_FAIL;
         }
         else
         {
            CDataAccessor responseDA;

            // Answer can be sent right in this response or later as a specific 
            // request
            if (respSize == 0)
            {
               mAllocateRequestResultCond.lock();
               LOG4CPLUS_TRACE(logger, "CConnectivityAgentProxy::allocateChannel waiting for mLastRequestResultDA");
               while (mAllocateRequestResultMap.find(channel_id) == mAllocateRequestResultMap.end())
                  mAllocateRequestResultCond.wait();

               responseDA = mAllocateRequestResultMap[channel_id];
               mAllocateRequestResultMap.erase(channel_id);
               mAllocateRequestResultCond.unlock();
            }
            else
            {
               // Answer is ready
               responseDA = CDataAccessor(respBuf, respSize);
            }

            LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::allocateChannel(type = "
               + convertIntegerToString((int)prio) + ", id = "
               + convertIntegerToString(channel_id) + ") after wait");
            responseDA.printContent();

            if((E_ALLOCATE_CHANNEL_RESP == responseDA.getOpCode())
               && (channel_id == responseDA.getChannelID()))
            {
               //ret  = static_cast<ERROR_CODE> (ByteOrder::ntoh32(*reinterpret_cast<UInt32*> ( mLastRequestResultDA.getData())));
               memcpy(&data, responseDA.getData(),sizeof(UInt32) );
               responseDA.resetAll();
               ret =  static_cast<ERROR_CODE>(data);
            }
            else
            {
               LOG4CPLUS_ERROR(logger, "CConnectivityAgentProxy::allocateChannel() => ERROR: wrong response type from Agent !!! ");
               ret = ERR_WRONG_SEQUENCE;
            }
            LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::allocateChannel(): Unlock last request result...");

            if(ret == ERR_OK)
            {
               mRegistryMutex.lockWrite();
               mRegistry[channel_id].mType = prio;
               mRegistry[channel_id].mpClient = observer;
               mRegistry[channel_id].mChannelBuffer.reserveSize(MAX_SIZE);
               mRegistryMutex.unlock();
            }
         }



      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CConnectivityAgentProxy::allocateChannel() => ERROR: channel already exists! ");
         ret = ERR_NUMBER_BUSY;
      }
      
   }

   return ret;
}

ERROR_CODE CConnectivityAgentProxy::deallocateChannel(UInt32 channel_id)
{
   LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::deallocateChannel(chID = " + convertIntegerToString(channel_id) + ")");
   ERROR_CODE ret = ERR_NOTFOUND;

   mRegistryMutex.lockWrite();
   // Channel info will be saved in info and removed from registry here.
   // In case of some errors, it will be restored in registry.
   tChannelInfo info;
   tChannelsRegistryMap::iterator iter = mRegistry.find(channel_id);
   if (iter == mRegistry.end())
   {
      LOG4CPLUS_ERROR(logger, "CConnectivityAgentProxy::deallocateChannel(chID = "
            + convertIntegerToString(channel_id) + ") => ERROR: Channel not found! ");
   }
   else
   {
      ret = ERR_OK;
      info = iter->second;
      iter->second.mChannelBuffer.forgetData();
      mRegistry.erase(iter);
      mChannelOnDeallocSet.insert(channel_id);
   }
   mRegistryMutex.unlock();

   if (ret != ERR_OK)
      return ret;

   CDataAccessor requestDA;
   requestDA.setChannelID(channel_id);
   requestDA.setOpCode(E_DEALLOCATE_CHANNEL);
   UInt8* buf = new UInt8[requestDA.getObjectSize()];
   requestDA.copyToRawArray(buf);

   // response will be sent later as a separate request
   UInt32 respSize = 0;
   CError err = mpIpc->request(mMsgIdGen.next(), buf, requestDA.getObjectSize(), NULL, respSize);
   delete [] buf;

   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
      ret = ERR_FAIL;
   }
   else
   {
      CDataAccessor responseDA;

      mDeallocateRequestResultCond.lock();
      {
         LOG4CPLUS_TRACE(logger, "CConnectivityAgentProxy::deallocateChannel waiting for mLastRequestResultDA");
         while (mDeallocateRequestResultMap.find(channel_id) == mDeallocateRequestResultMap.end())
         {
            LOG4CPLUS_TRACE(logger, "before wait");
            mDeallocateRequestResultCond.wait();
            LOG4CPLUS_TRACE(logger, "after wait");
         }

         responseDA = mDeallocateRequestResultMap[channel_id];
         mDeallocateRequestResultMap.erase(channel_id);
      }
      mDeallocateRequestResultCond.unlock();

      if (responseDA.getData())
      {
         if((E_DEALLOCATE_CHANNEL_RESP == responseDA.getOpCode())
            && (channel_id == responseDA.getChannelID()))
         {
            UInt32 data = 0;
            memcpy(&data, responseDA.getData(),sizeof(UInt32) );
            responseDA.resetAll();
            ret = static_cast<ERROR_CODE>(data);
         }
         else
         {
            LOG4CPLUS_ERROR(logger, "CConnectivityAgentProxy::deallocateChannel() => ERROR: wrong response type("
                  + convertIntegerToString(responseDA.getOpCode()) + ") from Agent  !!! ");
            ret = ERR_WRONG_SEQUENCE;
         }
      }
      else
      {
         LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::deallocateChannel() => "
            "channel already deleted form other side");
         ret = ERR_OK;
      }

   }

   if(ret != ERR_OK)
   {
      // something gone wrong, we need to restore information in registry
      mRegistryMutex.lockWrite();

      if (mRegistry.find(channel_id) != mRegistry.end())
      {
         LOG4CPLUS_WARN(logger, "CConnectivityAgentProxy::deallocateChannel: "
            "something gone wrong and I'm unable to restore channel info - "
            "there is a channel in registry with such id");
      }
      else
      {
         mRegistry[channel_id] = info;
         info.mChannelBuffer.forgetData();
         LOG4CPLUS_TRACE(logger, "CConnectivityAgentProxy::deallocateChannel:"
            "unable to delete channel " + convertIntegerToString(channel_id) + ", so channel info is restored");
      }

      mRegistryMutex.unlock();
   }

   mRegistryMutex.lockWrite();
   mChannelOnDeallocSet.erase(channel_id);
   mRegistryMutex.unlock();

   return ret;
}

// void CConnectivityAgentProxy::onReceive(CUnixSocket* pSender, const UInt8* pData, UInt32 size)
void CConnectivityAgentProxy::OnRequest(iviLink::Ipc::MsgID id, 
   UInt8 const* pPayload, UInt32 payloadSize, 
   UInt8* const pResponseBuffer, UInt32& bufferSize,
   iviLink::Ipc::DirectionID dirId)
{
   LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::OnRequest(): size " + convertIntegerToString(payloadSize));

   CDataAccessor responceDA(pPayload, payloadSize);
   const UInt32 channel_id = responceDA.getChannelID();
   const tOpCode type = static_cast<tOpCode>(responceDA.getOpCode());
   responceDA.printContent();

   switch(type)
   {
      case E_ALLOCATE_CHANNEL_RESP:
      {
         LOG4CPLUS_INFO(logger, "eAllocateChannelResp");
         mAllocateRequestResultCond.lock();
         mAllocateRequestResultMap[channel_id] = responceDA;
         LOG4CPLUS_TRACE(logger, "broadcasting about channel " + convertIntegerToString(channel_id));
         mAllocateRequestResultCond.broadcast();
         mAllocateRequestResultCond.unlock();
         break;
      }
      case E_DEALLOCATE_CHANNEL_RESP:
      {
         LOG4CPLUS_INFO(logger, "eDeallocateChannelResp");
         mDeallocateRequestResultCond.lock();
         mDeallocateRequestResultMap[channel_id] = responceDA;
         LOG4CPLUS_TRACE(logger, "broadcasting about channel " + convertIntegerToString(channel_id));
         mDeallocateRequestResultCond.broadcast();
         mDeallocateRequestResultCond.unlock();
         break;
      }
      case E_RECEIVE_DATA_NTF:
      {
         receiveDataNotification(responceDA);
         break;
      }
      case E_DEALLOCATE_CHANNEL_NTF:
      {
         LOG4CPLUS_INFO(logger, "eDeallocateChannelNtf");
         channelDeletedNotification(responceDA);
         break;
      }
      case E_CONNECTION_LOST_NTF:
      {
         OnDisconnected();
         break;
      }
      default:
      {
         LOG4CPLUS_WARN(logger, "CConnectivityAgentProxy::OnRequest() => UNKNOWN Response!");
         break;
      }
   }

   bufferSize = 0;
}


ERROR_CODE CConnectivityAgentProxy::receiveData(UInt32 channel_id, UInt8* data, UInt32 & received_size, UInt32 max_size)
{
   LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::receiveData() => channel " + convertIntegerToString(channel_id));

   ERROR_CODE ret = ERR_FAIL;
   if(data !=NULL)
   {
      mRegistryMutex.lockRead();
      tChannelsRegistryMap::iterator iter = mRegistry.find(channel_id);

      if(iter != mRegistry.end())
      {
         Buffer *buf = &(iter->second.mChannelBuffer);
         UInt32 filled_size = buf->getFilledSize();
         if(filled_size == 0)
         {
            LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::receiveData() => No data for channel " + convertIntegerToString(channel_id) + "!");
            ret = ERR_FAIL;
         }
         else
         {
            if(filled_size > max_size)
            {
               memcpy(data,buf->getData(),max_size );
               memmove(buf->getData(),buf->getData()+max_size, filled_size- max_size);
               buf->getFilledSize() =  filled_size- max_size;
               received_size = max_size;
            }
            else
            {
               memcpy(data,buf->getData(),filled_size);
               memset(buf->getData(),0, filled_size);
               buf->getFilledSize() = 0;
               received_size = filled_size;
            }
            ret = ERR_OK;
         }
      }
      else
      {
         ret = ERR_NOTFOUND;
      }
      mRegistryMutex.unlock();
   }
   return ret;
}


void CConnectivityAgentProxy::receiveDataNotification(CDataAccessor & accessor)
{

   UInt32 channel_id = accessor.getChannelID();

   UInt32 data_size = accessor.getDataSize();

   LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::receiveDataNotification() => channel "
         + convertIntegerToString(channel_id) + ", data_size "
         + convertIntegerToString(data_size));

   mRegistryMutex.lockRead();
   tChannelsRegistryMap::iterator iter = mRegistry.find(channel_id);
   if(iter != mRegistry.end())
   {

      Buffer *buf = &(iter->second.mChannelBuffer);

      UInt32 free_size = buf->getAllocatedSize() - buf->getFilledSize();


      if(free_size < data_size)
      {

         mCallbackListMutex.lock();
         CCallbackWrapper* pCallback = new CBufferOverflowCallback(iter->second.mpClient,channel_id);
         mCallbackList.push_back(pCallback);
         mCallbackListMutex.unlock();
         mCallbackSema.signal();
         LOG4CPLUS_WARN(logger, "CConnectivityAgentProxy::receiveDataNotification() => overflow!");
         //copy or not?
      }
      else
      {
         buf->appendData(accessor.getData(), data_size);
         free_size-=data_size;
         mCallbackListMutex.lock();
         CCallbackWrapper* pCallback = new CDataReceivedCallback(iter->second.mpClient,channel_id,data_size);
         mCallbackList.push_back(pCallback);
         mCallbackListMutex.unlock();
         mCallbackSema.signal();
      }
   }
   mRegistryMutex.unlock();
}

void CConnectivityAgentProxy::channelDeletedNotification(CDataAccessor & accessor)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   UInt32 channel_id = accessor.getChannelID();

   mRegistryMutex.lockWrite();
   {
      tChannelsRegistryMap::iterator iter = mRegistry.find(channel_id);
      if (iter == mRegistry.end())
      {
         LOG4CPLUS_DEBUG(logger, "channel " + convertIntegerToString(channel_id) + " is not found");
         if (mChannelOnDeallocSet.find(channel_id) != mChannelOnDeallocSet.end())
         {
            LOG4CPLUS_TRACE(logger, "channel is being deallocated from this side, signaling about it");
            mDeallocateRequestResultCond.lock();

            mDeallocateRequestResultMap[channel_id] = CDataAccessor();

            mDeallocateRequestResultCond.broadcast();
            mDeallocateRequestResultCond.unlock();
         }
      }
      else
      {
         LOG4CPLUS_DEBUG(logger, "channel " + convertIntegerToString(channel_id) + " found");
         mCallbackListMutex.lock();
         {
            CCallbackWrapper* pCallback = new CChannelDeletedCallback(iter->second.mpClient,channel_id);
            mCallbackList.push_back(pCallback);
            mCallbackSema.signal();
         }
         mCallbackListMutex.unlock();

         mRegistry.erase(channel_id);
      }

   }
   mRegistryMutex.unlock();
}

ERROR_CODE CConnectivityAgentProxy::sendData(UInt32 channel_id, UInt8 const* data, UInt32 size)
{
   LOG4CPLUS_INFO(logger, "CConnectivityAgentProxy::sendData() => channel "
         + convertIntegerToString(channel_id) + ", size " + convertIntegerToString(size));
   ERROR_CODE ret = ERR_FAIL;

   if((data !=NULL)&&((Int32)size>0)&&(size<0xFFFF))
   {
      mRegistryMutex.lockRead();
      bool found = (mRegistry.find(channel_id) != mRegistry.end());
      mRegistryMutex.unlock();

      if(found)
      {
         CDataAccessor requestDA;
         requestDA.setChannelID(channel_id);
         requestDA.setOpCode(E_SEND_DATA);
         requestDA.setData(data,size );
         UInt8* buf = new UInt8[requestDA.getObjectSize()];
         requestDA.copyToRawArray(buf);

         UInt32 respSize = 0;
         CError err = mpIpc->request(mMsgIdGen.next(), buf, requestDA.getObjectSize(), NULL, respSize);
         if (err.isNoError())
         {
            ret = ERR_OK;
         }
         else
         {
            LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
         }

         delete[] buf;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CConnectivityAgentProxy::sendData() => ERROR: channel "
               + convertIntegerToString(channel_id) + " is not opened yet!");
         ret = ERR_NOTFOUND;
      }
   }

   return ret;
}

ERROR_CODE CConnectivityAgentProxy::getFreeSize(UInt32 channel_id, UInt32& free_size) const
{
   LOG4CPLUS_TRACE_METHOD(logger, "CConnectivityAgentProxy::getFreeSize "
      + std::string("channel_id") + convertIntegerToString(channel_id));

   ERROR_CODE res = ERR_FAIL;

   mRegistryMutex.lockRead();
   {
      tChannelsRegistryMap::const_iterator iter = mRegistry.find(channel_id);
      if(iter != mRegistry.end())
      {
         Buffer const * const buf = &(iter->second.mChannelBuffer);
         free_size = buf->getAllocatedSize() - buf->getFilledSize();
      }
      else
      {
         res = ERR_NOTFOUND;
      }
   }
   mRegistryMutex.unlock();

   return res;
}

void CConnectivityAgentProxy::OnDisconnected()
{
   LOG4CPLUS_TRACE(logger, "CConnectivityAgentProxy::OnDisconnected() ");

   mRegistryMutex.lockWrite();
   tChannelsRegistryMap::iterator iter = mRegistry.begin();
   mCallbackListMutex.lock();
   while(iter!=mRegistry.end())
   {
      CCallbackWrapper* pCallback = new CConnectionLostCallback(iter->second.mpClient);     
      mCallbackList.push_back(pCallback);
      mCallbackSema.signal();
   }
   mCallbackListMutex.unlock();
   mRegistryMutex.unlock();
}

void CConnectivityAgentProxy::threadFunc()
{
   while (false == getStopFlag())
   {
      mCallbackSema.wait();
      mCallbackListMutex.lock();
      for (std::list<CCallbackWrapper* >::iterator iter = mCallbackList.begin(); iter != mCallbackList.end(); ++iter)
      {

         static_cast<CCallbackWrapper* >(*iter)->execute();
         delete *iter;
      }
      mCallbackList.clear();
      mCallbackListMutex.unlock();
   }
}
