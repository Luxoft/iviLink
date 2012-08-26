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
#include <cassert>
#include <utility>
#include <algorithm>
#include <unistd.h>
/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "utils/ipc/CIpc.hpp"
#include "CSourceAgent.hpp"
#include "CTargetAgent.hpp"
#include "framework/components/ConnectivityAgent/generic/common/Buffer.hpp"
#include "framework/components/ConnectivityAgent/generic/common/CDataAccessor.hpp"
#include "framework/components/ConnectivityAgent/generic/HAL/CConnectivityAgent.hpp"
 /********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "L1InterfaceStub.hpp"
/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "CChannelAllocator.hpp"
#include "utils/misc/Logger.hpp"
#include "utils/misc/byteOrder.hpp"
#include "utils/threads/CTimeoutManager.hpp"
#include "framework/components/ConnectivityAgent/generic/HAL/CCarrierAdapter.hpp"


using namespace iviLink::ConnectivityAgent::L1;

using iviLink::ConnectivityAgent::CDataAccessor;
using iviLink::ConnectivityAgent::HAL::CConnectivityAgent;

CMutex L1InterfaceStub::mSingletonMutex;
L1InterfaceStub* L1InterfaceStub::mSelf = 0;

static const char* systemControllerSocket = "SysCtr_ConAgnt";

Logger L1InterfaceStub::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L1.L1InterfaceStub"));

L1InterfaceStub::L1InterfaceStub():
   mpAgent(NULL),
   mSCProtocol(this,systemControllerSocket ),
   bShutDownFlag(false)
{
   fillCallbacks();
   CError err = mSCProtocol.connect();
   LOG4CPLUS_INFO(logger, static_cast<std::string>(err));
}

L1InterfaceStub::~L1InterfaceStub()
{
   mClientDirections.clear();
   delete mpIpc;

   if(mpAgent)
      delete mpAgent;
}

void L1InterfaceStub::start(bool isServer, const char* sockPath)
{
   mpAgent = new CConnectivityAgent(isServer);
   mpAgent->start();
   mpIpc = new iviLink::Ipc::CIpc(sockPath, *this);
   CError err = mpIpc->beginWaitForConnection();
   assert(err.isNoError());
}

L1InterfaceStub* L1InterfaceStub::getInstance()
{
   mSingletonMutex.lock();
   if(mSelf == NULL)
   {
      mSelf = new L1InterfaceStub();
   }
   mSingletonMutex.unlock();
   return mSelf;
}

void L1InterfaceStub::deleteInstance()
{
   delete this;
   mSelf = NULL;
}

void L1InterfaceStub::OnConnection(DirectionID dirId)
{
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::OnConnection dirId = "
      + convertIntegerToString(dirId));
   mClientDirections.push_back(dirId);
}

void L1InterfaceStub::OnConnectionLost(DirectionID dirId)
{
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::onConnectionLost dirId = " 
      + convertIntegerToString(dirId));
   std::remove(mClientDirections.begin(), mClientDirections.end(), dirId);
}

ERROR_CODE L1InterfaceStub::tryAllocateChannel(TChannelPriority prio, UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(logger, "L1InterfaceStub::tryAllocateChannel(" + convertIntegerToString(channel_id) +")");
   ERROR_CODE ret = ERR_UNKNOWN;

   mRequestedMapMutex.lock();
   mRegistryMutex.lock();
   if (mRegistry.find(channel_id) == mRegistry.end()) //if channel wasn't opened yet
   {
      
      tRequestedChannelsMap::iterator iter = mRequestedMap.find(channel_id);
      if (iter == mRequestedMap.end()) // if channel wasn't requested yet ->request channel
      {
         tRequestedChannelInfo info;
         info.mType =  prio;
         info.mSender = E_CLIENT;
         mRequestedMap.insert(std::pair<UInt32,tRequestedChannelInfo >(channel_id, info));
         ret = ERR_OK;
      }
      else
      {
         if (iter->second.mSender  == E_AGENT) //if channel was requested by other side Agent ->allocation complete
         {
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::tryAllocateChannel() =>Allocation complete ");
            mRequestedMap.erase(channel_id);
            ret = ERR_REASSIGN;
         }
         else  //channel was requested by Client Application not the first time
         {
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::tryAllocateChannel() => ERROR: allocation is in progress! ");
            ret = ERR_IN_PROGRESS;

         }
      }
   }
   else //channel already opened
   {
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::tryAllocateChannel() => ERROR: channel already exists! ");
      ret = ERR_NUMBER_BUSY;
   }
   mRegistryMutex.unlock();
   mRequestedMapMutex.unlock();
   
   return ret;
}

ERROR_CODE L1InterfaceStub::allocateChannel(TChannelPriority prio, UInt32 channel_id)
{
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::allocateChannel(type = "
         + convertIntegerToString((int)prio) + ", id = "
         + convertIntegerToString(channel_id) + " ), this = " + convertIntegerToString((intptr_t)this));
   ERROR_CODE ret = ERR_FAIL;
   mRegistryMutex.lock();
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::allocateChannel(type = "
         + convertIntegerToString((int)prio) + ", id = "
         + convertIntegerToString(channel_id) + " ), this = " + convertIntegerToString((intptr_t)this));
   if (mRegistry.find(channel_id) == mRegistry.end())
   {
      ///>FIXME Set up correct thresholds
      tChannelInfo info;
      info.mType = prio;
      info.mLowerThresholdTime = 0;
      info.mUpperThresholdTime = 0xFFFFFFFF;
      info.mState = E_TRANSITION;
      ret = CChannelAllocator::getInstance()->allocateChannel(prio,channel_id, info);
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::allocateChannel():  CChannelAllocator ret = "
            + convertIntegerToString((int)ret));
      mRegistry.insert(std::pair<UInt32,tChannelInfo >(channel_id, info));
      ret = ERR_OK;
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "L1InterfaceStub::allocateChannel() => ERROR: channel already exists! ");
      ret = ERR_NUMBER_BUSY;
   }
   mRegistryMutex.unlock();

   return ret;
}

ERROR_CODE L1InterfaceStub::deallocateChannel(UInt32 channel_id)
{
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::deallocateChannel(chID = " + convertIntegerToString(channel_id) + ")");
   ERROR_CODE ret = ERR_NOTFOUND;
   mRegistryMutex.lock();

   if (mRegistry.find(channel_id) != mRegistry.end())
   {
      ret = CChannelAllocator::getInstance()->deallocateChannel(channel_id);
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "L1InterfaceStub::deallocateChannel(chID = "
            + convertIntegerToString(channel_id) + ") => ERROR: Channel not found! ");
   }
   mRegistryMutex.unlock();

   return ret;
}


ERROR_CODE L1InterfaceStub::sendData(CDataAccessor & accessor)
{
   LOG4CPLUS_TRACE(logger, "L1InterfaceStub::sendData()");
   ERROR_CODE ret = ERR_NOTFOUND;
   mRegistryMutex.lock();
   tChannelsRegistryMap::iterator iter = mRegistry.find(accessor.getChannelID());

   if (iter!=mRegistry.end())
   {
      bool exit = false;
      Buffer buf;

      UInt32 size = accessor.getObjectSize();

      LOG4CPLUS_INFO(logger, "L1InterfaceStub::sendData() => Overall data bytes " + convertIntegerToString(size));
      do
      {
         if (size > buf.getMaxSize())
         {
            buf.reserveSize(buf.getMaxSize());
            buf.getFilledSize() = buf.getMaxSize();
            size -= buf.getMaxSize();
         }
         else
         {
            buf.reserveSize(size);
            buf.getFilledSize() = size;
            exit = true;
         }

         if (accessor.getDataSize()>0)
         {
            UInt8 *pData = accessor.getData();
            memcpy(buf.getData(), pData, buf.getFilledSize());
            pData+= buf.getFilledSize();
         }
         LOG4CPLUS_INFO(logger, "L1InterfaceStub::sendData() => sending "
               + convertIntegerToString(buf.getFilledSize()) + " bytes");
         ret = iter->second.mpSourceAgent->fillBuffer(buf);

      } while (!exit);
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "L1InterfaceStub::sendData() => channel not found!");
   }
   mRegistryMutex.unlock();
   return ret;
}

ERROR_CODE L1InterfaceStub::receiveData(Buffer & buffer, UInt32 channel_id)
{
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::receiveData() => channel " + convertIntegerToString(channel_id));

   UInt32 offset = 0;
   CDataAccessor accessor;
   ERROR_CODE ret = ERR_UNKNOWN;

      assert(buffer.getFilledSize() <= buffer.getMaxSize());

      LOG4CPLUS_INFO(logger, "L1InterfaceStub::receiveData() =>  buf size "
           + convertIntegerToString(buffer.getFilledSize()));
      if(channel_id == SERVICE_CHANNEL_NUMBER)
      {
         //> If  data received from service channel - process service request
         do
         {

            UInt32 begin = offset;

            UInt16 buf_length = ByteOrder::ntoh16(*reinterpret_cast<UInt16*> (buffer.getData() + offset));
            offset += 2;
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::receiveData() =>  buf_length " + convertIntegerToString(buf_length));
            assert(begin + buf_length <= buffer.getFilledSize());
            UInt32 operation = ByteOrder::ntoh32(*reinterpret_cast<UInt32*> (buffer.getData() + offset));
            offset += 4;
            accessor.setOpCode(operation);
            UInt32 channel_id = ByteOrder::ntoh32(*reinterpret_cast<UInt32*> (buffer.getData() + offset));
            offset += 4;
            accessor.setChannelID(channel_id);
            UInt32 data_size = ByteOrder::ntoh32(*reinterpret_cast<UInt32*> (buffer.getData() + offset));
            offset += 4;
            accessor.setData(buffer.getData() + offset, data_size);
            offset += data_size;
            accessor.printContent();
            ///>Process service request
            std::map<tOpCode, process_func>::iterator iter = mServiceCallbacksMap.find(static_cast<tOpCode>(operation));

            if (iter !=mServiceCallbacksMap.end())
            {
               ///> This funny language structure is just callback invocation. Not obvious, huh? =)
               (this->*iter->second)(accessor);
            }
            else
            {
               LOG4CPLUS_WARN(logger, "L1InterfaceStub::receiveData() => UNKNOWN SERVICE REQUEST = "
                     + convertIntegerToString(operation) + "!!!");
            }

         } while (offset < buffer.getFilledSize());

         LOG4CPLUS_INFO(logger, "L1InterfaceStub::receiveData() => offset "
               + convertIntegerToString(offset) + "!!!");
         assert(buffer.getFilledSize() == offset);
      }
      else
      {
         //> Or pack to accessor and send to client
         offset = 3*sizeof(UInt32);
         accessor.setOpCode(E_RECEIVE_DATA_NTF);
         accessor.setChannelID(channel_id);
         accessor.setData(buffer.getData(),buffer.getFilledSize() -offset);
         accessor.printContent();
         UInt8* buf = new UInt8[accessor.getObjectSize()];
         accessor.copyToRawArray(buf);

         UInt32 responseSize = 0;
         mpIpc->request(mMsgIdGen.next(), 
            buf, accessor.getObjectSize(), 
            NULL, responseSize,
            &mRegistry[channel_id].mClientDir);

         delete [] buf;
      }
      ret = ERR_OK;

   return ret;
}

void L1InterfaceStub::sendRequest( CDataAccessor & accessor)
{
   Buffer buf;
   UInt32 offset = 0;

   buf.getFilledSize() = accessor.getObjectSize()+sizeof(UInt16);
   buf.reserveSize(buf.getFilledSize());

   *reinterpret_cast<UInt16*> (buf.getData() + offset) = ByteOrder::hton16((UInt16)buf.getFilledSize());
   offset += 2;

   *reinterpret_cast<UInt32*> (buf.getData() + offset) = ByteOrder::hton32(accessor.getOpCode());
   offset += 4;

   *reinterpret_cast<UInt32*> (buf.getData() + offset) = ByteOrder::hton32(accessor.getChannelID());
   offset += 4;

   *reinterpret_cast<UInt32*> (buf.getData() + offset) = ByteOrder::hton32(accessor.getDataSize());
   offset += 4;

   if (accessor.getDataSize())
   {
      memcpy(buf.getData() + offset,accessor.getData(), accessor.getDataSize() );
   }

   mRegistry[SERVICE_CHANNEL_NUMBER].mpSourceAgent->fillBuffer(buf);

   buf.forgetData();

}

void L1InterfaceStub::OnConnected()
{
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::OnConnected(): Connectivity Agents connected, create service channel ....");
   allocateChannel(eSafetyCritical, SERVICE_CHANNEL_NUMBER);
   mSCProtocol.sendConnectionEstablished(mpAgent->getCurrentGender());

}

void L1InterfaceStub::OnDisconnected()
{
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::OnDisconnected(): Connectivity Agents disconnected=>sending notfications to all clients");
   
   mSCProtocol.sendConnectionLost();

   CDataAccessor responceDA;
   responceDA.setOpCode(E_CONNECTION_LOST_NTF);
   for (tClientDirections::iterator iter = mClientDirections.begin(); iter != mClientDirections.end(); ++iter)
   {
      UInt8* buf = new UInt8[responceDA.getObjectSize()];
      responceDA.copyToRawArray(buf);

      UInt32 responseSize = 0;
      mpIpc->request(mMsgIdGen.next(), 
         buf, responceDA.getObjectSize(), 
         NULL, responseSize,
         &(*iter));

      delete [] buf;
   }
}

void L1InterfaceStub::fillCallbacks()
{

   mClientsCallbacksMap.insert(std::pair<tOpCode  ,process_func >(E_ALLOCATE_CHANNEL, &L1InterfaceStub::processClientAllocateRequest));
   mClientsCallbacksMap.insert(std::pair<tOpCode  ,process_func >(E_DEALLOCATE_CHANNEL, &L1InterfaceStub::processClientDeallocateRequest));
   mClientsCallbacksMap.insert(std::pair<tOpCode  ,process_func >(E_SEND_DATA, &L1InterfaceStub::processClientSendRequest));

   mServiceCallbacksMap.insert(std::pair<tOpCode  ,process_func >(E_ALLOCATE_CHANNEL, &L1InterfaceStub::processServiceAllocateRequest));
   mServiceCallbacksMap.insert(std::pair<tOpCode  ,process_func >(E_DEALLOCATE_CHANNEL, &L1InterfaceStub::processServiceDeallocateRequest));
   mServiceCallbacksMap.insert(std::pair<tOpCode  ,process_func >(E_ALLOCATE_CHANNEL_RESP, &L1InterfaceStub::processServiceAllocateResponse));
   mServiceCallbacksMap.insert(std::pair<tOpCode  ,process_func >(E_DEALLOCATE_CHANNEL_RESP, &L1InterfaceStub::processServiceDeallocateResponse));

}

void L1InterfaceStub::OnRequest(iviLink::Ipc::MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      UInt8* const pResponseBuffer, UInt32& bufferSize,
      DirectionID dirId)
{
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::OnRequest(): received " + convertIntegerToString(payloadSize) + " bytes");

   UInt32 savedBufferSize = bufferSize;
   // no response by default
   bufferSize = 0;

   CDataAccessor pReq(pPayload, payloadSize);
   pReq.printContent();
   UInt32 channel_id = pReq.getChannelID();

   std::map<tOpCode, process_func>::iterator callback_iter = mClientsCallbacksMap.find(static_cast<tOpCode>(pReq.getOpCode()));

   if(callback_iter != mClientsCallbacksMap.end())
   {
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::Receive()");
      (this->*callback_iter->second)(pReq);

      if (E_ALLOCATE_CHANNEL == pReq.getOpCode())
      {
         mRequestedMapMutex.lock();
         tRequestedChannelsMap::iterator iter =  mRequestedMap.find(channel_id);
         if (iter!=mRequestedMap.end())
         {
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::Receive() => set client for requested "
                  + convertIntegerToString(channel_id) + " channel !");
            iter->second.mClientDir = dirId;
         }
         else
         {
            //Accessor with error should be set in ClientAllocateCallback

            /// @todo: fix when size of response buffer will be changeable
            LOG4CPLUS_WARN(logger, "@todo: fix when size of response buffer will be changeable");
            assert(savedBufferSize >= pReq.getObjectSize());
            if (savedBufferSize >= pReq.getObjectSize())
            {
               bufferSize = pReq.getObjectSize();
               pReq.copyToRawArray(pResponseBuffer);
            }

         }
         mRequestedMapMutex.unlock();
      }
      else if(E_ALLOCATE_CHANNEL_RESP == pReq.getOpCode())
      {
         mRegistryMutex.lock();
         tChannelsRegistryMap::iterator iter =  mRegistry.find(channel_id);
         if(iter!=mRegistry.end())
         {
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::Receive() => set client for allocated "
                   + convertIntegerToString(channel_id) + " channel !");
            iter->second.mClientDir = dirId;
         }

         /// @todo: fix when size of response buffer will be changeable
         LOG4CPLUS_WARN(logger, "@todo: fix when size of response buffer will be changeable");
         assert(savedBufferSize >= pReq.getObjectSize());
         if (savedBufferSize >= pReq.getObjectSize())
         {
            bufferSize = pReq.getObjectSize();
            pReq.copyToRawArray(pResponseBuffer);
         }

         mRegistryMutex.unlock();
      }
   }
   else
   {
      LOG4CPLUS_WARN(logger, "L1InterfaceStub::Receive() => UNKNOWN REQUEST!");
   }

}

ERROR_CODE L1InterfaceStub::processClientAllocateRequest(CDataAccessor & accessor)
{
   ERROR_CODE ret = ERR_UNKNOWN;
   UInt32 channel_id = accessor.getChannelID();
   if(SERVICE_CHANNEL_NUMBER !=channel_id)
   {
      UInt32 prio = 0;
      memcpy(&prio, accessor.getData(), accessor.getDataSize());
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientAllocateRequest() => Allocate Channel Request: ChID = "
            + convertIntegerToString(channel_id) + ", prio = " + convertIntegerToString(prio));
      ret = tryAllocateChannel(static_cast<TChannelPriority>(prio), channel_id);

      if (ret == ERR_OK) //>if this is newly requested channel
      {
         LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientAllocateRequest() => this is newly requested channel!" );
         sendRequest(accessor);
      }
      else
      {
         if (ret == ERR_REASSIGN) /// or this is channel waiting for client request
         {
            ret = allocateChannel(static_cast<TChannelPriority>(prio), channel_id);
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientAllocateRequest() => this is channel waited for client request!" );
         }
         accessor.setOpCode(E_ALLOCATE_CHANNEL_RESP);
         accessor.setChannelID(channel_id);
         UInt32 data = ret;
         accessor.setData(reinterpret_cast<UInt8* >(&data), sizeof(UInt32));
         sendRequest(accessor);
      }
   }
   return ret;
}

ERROR_CODE L1InterfaceStub::processClientDeallocateRequest(CDataAccessor & accessor)
{
   ERROR_CODE ret = ERR_UNKNOWN;
   UInt32 channel_id = accessor.getChannelID();
   if(SERVICE_CHANNEL_NUMBER !=channel_id)
   {
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientDeallocateRequest() => Deallocate Channel Request: ChID = " + convertIntegerToString(channel_id));
      mRegistryMutex.lock();
      tChannelsRegistryMap::iterator iter =  mRegistry.find(channel_id);
      if (iter!=mRegistry.end())
      {
         iter->second.mState = E_TRANSITION;
         sendRequest(accessor);
         new CRequestTimeout(100000,static_cast<tOpCode>(accessor.getOpCode()),channel_id,this );
         ret = ERR_OK;
      }
      else
      {
         LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientDeallocateRequest() => Deallocate Channel Request: ChID = " + convertIntegerToString(channel_id) + " NOT FOUND!");
         ret = ERR_NOTFOUND;
      }
      mRegistryMutex.unlock();
   }

   return ret;
}
ERROR_CODE L1InterfaceStub::processClientSendRequest(CDataAccessor & accessor)
{

   ERROR_CODE ret = ERR_UNKNOWN;
   UInt32 channel_id = accessor.getChannelID();
   if (SERVICE_CHANNEL_NUMBER !=channel_id)
   {
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientSendRequest() => Send Data Request: ChID = " + convertIntegerToString(channel_id));
      ret = sendData(accessor);
   }
   return ret;
}

ERROR_CODE L1InterfaceStub::processServiceAllocateRequest(CDataAccessor & accessor)
{
   ERROR_CODE ret = ERR_UNKNOWN;
   UInt32 prio = 0;
   memcpy(&prio, accessor.getData(), sizeof(UInt32));
   UInt32 channel_id = accessor.getChannelID();
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateRequest()=>Allocate channel prio = "
         + convertIntegerToString(prio) + " id =" + convertIntegerToString(accessor.getChannelID()));

   bool found = false;
   iviLink::Ipc::DirectionID client_dir;

   mRequestedMapMutex.lock();
   {
      tRequestedChannelsMap::iterator iter = mRequestedMap.find(channel_id);
      found = (iter != mRequestedMap.end());
  
      if (found)
      {
         ///>if channel was already requested allocate channel
         client_dir = iter->second.mClientDir;
         mRequestedMap.erase(iter);
      }
      else
      {
         ///> if channel wasn't requested yet ->set channel to wait and trigger timeout
         LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateRequest()=>channel wasn't requested yet ->set channel to wait and trigger timeout");
         tRequestedChannelInfo info;
         info.mSender = E_AGENT;
         info.mType = static_cast<TChannelPriority>(prio);
         info.mClientDir = -1;
         mRequestedMap.insert(std::pair<UInt32,tRequestedChannelInfo >(channel_id, info));
         new CRequestTimeout(100000,E_ALLOCATE_CHANNEL,channel_id, this);
         ret = ERR_OK;
      }
   }
   mRequestedMapMutex.unlock();

   if (found)
   {
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateRequest()=>channel was already requested -> allocate channel");
      allocateChannel(static_cast<TChannelPriority>(prio),channel_id);

      mRegistryMutex.lock();
      mRegistry[channel_id].mClientDir = client_dir;
      mRegistry[channel_id].mState = E_OPERATIONAL;
      mRegistryMutex.unlock();

      ret = ERR_REASSIGN;
      accessor.setOpCode(E_ALLOCATE_CHANNEL_RESP);
      UInt32 data = ERR_OK;
      accessor.setData(reinterpret_cast<UInt8 *>(&data), sizeof(UInt32));
      sendRequest(accessor);
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateRequest()=> after allocation send response to application!");
      UInt8* buf = new UInt8[accessor.getObjectSize()];
      accessor.copyToRawArray(buf);

      UInt32 respSize = 0;
      CError err = mpIpc->request(mMsgIdGen.next(), buf, accessor.getObjectSize(), NULL, respSize, &client_dir);
      if (!err.isNoError())
      {
         LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
      }

      delete[] buf;
   }
   
   return ret;
}

ERROR_CODE L1InterfaceStub::processServiceDeallocateRequest(CDataAccessor & accessor)
{
   UInt32 channel_id = accessor.getChannelID();
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceDeallocateRequest()=>Channel deallocate request id = " + convertIntegerToString(channel_id));
   ERROR_CODE ret = deallocateChannel(channel_id );
   accessor.setOpCode(E_DEALLOCATE_CHANNEL_RESP);
   UInt32 data = ret;
   accessor.setData(reinterpret_cast<UInt8 *>(&data), sizeof(UInt32));
   sendRequest(accessor);
   if (ret == ERR_OK)
   {
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceDeallocateRequest() => channel successfully deallocated. "
         "Notifying client");

      iviLink::Ipc::DirectionID dirId;
      bool found = false;

      mRegistryMutex.lock();
      tChannelsRegistryMap::iterator iter = mRegistry.find(accessor.getChannelID());
      if (mRegistry.end() != iter)
      {
         dirId = iter->second.mClientDir;
         found = true;

         LOG4CPLUS_TRACE(logger, "L1InterfaceStub::processServiceDeallocateRequest()=>channel found in registry - removing");
         mRegistry.erase(accessor.getChannelID());
      }
      mRegistryMutex.unlock();

      if (found)
      {
         accessor.setOpCode(E_DEALLOCATE_CHANNEL_NTF);
         UInt8* buf = new UInt8[accessor.getObjectSize()];
         accessor.copyToRawArray(buf);
         UInt32 respSize = 0;
         CError err = mpIpc->request(mMsgIdGen.next(),
            buf, accessor.getObjectSize(),
            NULL, respSize,
            &dirId);

         if (!err.isNoError())
         {
            LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
         }

         delete[] buf;
      }
   } // ret == OK
   return ret;
}

ERROR_CODE L1InterfaceStub::processServiceAllocateResponse(CDataAccessor & accessor)
{
   ERROR_CODE ret = ERR_UNKNOWN;
   UInt32 channel_id = accessor.getChannelID();
   UInt32 res = 0;
   memcpy(&res, accessor.getData(), sizeof(UInt32));
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateResponse()=>Channel allocated responce id = "
         + convertIntegerToString(channel_id) + ", res = " + convertIntegerToString(res));

   bool found = false;
   tRequestedChannelInfo requestedChannelInfo;

   mRequestedMapMutex.lock();
   {
      tRequestedChannelsMap::iterator iter = mRequestedMap.find(channel_id);
      found = (iter != mRequestedMap.end());

      if (found)
      {
         requestedChannelInfo = iter->second;
         mRequestedMap.erase(iter);
      }

   }
   mRequestedMapMutex.unlock();

   if (found)
   {
      if(static_cast<ERROR_CODE>(res) == ERR_OK)
      {
         ret = allocateChannel(requestedChannelInfo.mType, channel_id);
         if (ret == ERR_OK)
         {
            mRegistryMutex.lock();
            mRegistry[channel_id].mClientDir = requestedChannelInfo.mClientDir;
            mRegistry[channel_id].mState = E_OPERATIONAL;
            mRegistryMutex.unlock();
         }

         UInt8* buf = new UInt8[accessor.getObjectSize()];
         accessor.copyToRawArray(buf);
         
         UInt32 respSize = 0;
         CError err = mpIpc->request(mMsgIdGen.next(), 
            buf, accessor.getObjectSize(), 
            NULL, respSize, 
            &requestedChannelInfo.mClientDir);
         if (!err.isNoError())
         {
            LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
         }

         delete[] buf;     

      }

   }
   else
   {
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateResponse()=>Channel not found in requested map");
   }
   
   return ret;
}

ERROR_CODE L1InterfaceStub::processServiceDeallocateResponse(CDataAccessor & accessor)
{
   UInt32 channel_id = accessor.getChannelID();
   UInt32 res = 0;
   memcpy(&res, accessor.getData(), sizeof(UInt32));

   ERROR_CODE ret = static_cast<ERROR_CODE>(res);
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceDeallocateResponse()=>Channel deallocated responce id = "
      + convertIntegerToString(channel_id) + " err = " + convertIntegerToString((int)ret));
   if (ret == ERR_OK)
   {
      ret = deallocateChannel(channel_id);
      if (ret == ERR_OK)
      {
         bool found = false;
         iviLink::Ipc::DirectionID dirId;

         mRegistryMutex.lock();
         {
            tChannelsRegistryMap::iterator iter = mRegistry.find(channel_id);
            if (iter != mRegistry.end())
            {
               dirId = iter->second.mClientDir;
               found = true;
               mRegistry.erase(iter);
            }
         }
         mRegistryMutex.unlock();

         if (found)
         {
            UInt8* buf = new UInt8[accessor.getObjectSize()];
            accessor.copyToRawArray(buf);

            UInt32 respSize = 0;
            CError err = mpIpc->request(mMsgIdGen.next(),
               buf, accessor.getObjectSize(),
               NULL, respSize,
               &dirId);

            if (!err.isNoError())
            {
               LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
            }

            delete [] buf;
         }
         else
         {
            LOG4CPLUS_INFO(logger, "Channel " + convertIntegerToString(channel_id) +
               " is not found. Probably, already deleted");
         }

      }
   }
   return ret;
}

L1InterfaceStub::CRequestTimeout::CRequestTimeout(UInt32 timeout, tOpCode req, UInt32 channel_id, L1InterfaceStub* pOwner ,bool enabled):
   mRequest(req), mChannelID(channel_id), mpOwner(pOwner), mEnabled(enabled)
{
   CTimeoutManager::getInstance()->addSubscriber(this,timeout);
}

void L1InterfaceStub::CRequestTimeout::onTimeout()
{
   CDataAccessor resp;
   UInt32 data = ERR_TIMEOUT;
   if (mEnabled)
   {
      switch (mRequest)
      {
         case E_ALLOCATE_CHANNEL:
         {
            mpOwner->mRegistryMutex.lock();
            bool found = (mpOwner->mRegistry.find(mChannelID) != mpOwner->mRegistry.end());
            mpOwner->mRegistryMutex.unlock();

            if (found)
            {
               LOG4CPLUS_INFO(logger, "L1InterfaceStub::CRequestTimeout::onTimeout()=> channel "
                      + convertIntegerToString(mChannelID) + " allocated!");
            }
            else
            {
               LOG4CPLUS_WARN(logger, "L1InterfaceStub::CRequestTimeout::onTimeout()=> Channel "
                     + convertIntegerToString(mChannelID) + " allocation timeout!");
               resp.setChannelID(mChannelID);
               resp.setOpCode(E_ALLOCATE_CHANNEL_RESP);
               resp.setData(reinterpret_cast<UInt8 *>(&data), sizeof(UInt32));
               mpOwner->mRequestedMapMutex.lock();
               mpOwner->mRequestedMap.erase(mChannelID);
               mpOwner->mRequestedMapMutex.unlock();
               mpOwner->sendRequest(resp);
            }
            
            break;
         }
         case E_DEALLOCATE_CHANNEL:
         {
            iviLink::Ipc::DirectionID client_dir;
            bool found = false;
            mpOwner->mRegistryMutex.lock();
            {
               tChannelsRegistryMap::iterator iter = mpOwner->mRegistry.find(mChannelID);
               found = (iter != mpOwner->mRegistry.end());
               if (found)
               {
                  client_dir = iter->second.mClientDir;
               }
            }
            mpOwner->mRegistryMutex.unlock();

            if (found)
            {
               LOG4CPLUS_INFO(logger, "L1InterfaceStub::CRequestTimeout::onTimeout()=> Channel deallocation timeout!");
               resp.setChannelID(mChannelID);
               resp.setOpCode(E_DEALLOCATE_CHANNEL_RESP);
               resp.setData(reinterpret_cast<UInt8 *>(&data), sizeof(UInt32));
               UInt8* buf = new UInt8[resp.getObjectSize()];
               resp.copyToRawArray(buf);

               UInt32 respSize = 0;
               CError err = mpOwner->mpIpc->request(mpOwner->mMsgIdGen.next(), 
                  buf, resp.getObjectSize(), 
                  NULL, respSize, 
                  &client_dir);
               if (!err.isNoError())
               {
                  LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
               }

               delete [] buf;
            }
            else
            {
               LOG4CPLUS_INFO(logger, "L1InterfaceStub::CRequestTimeout::onTimeout()=> channel Deallocated!");
            }
            
            break;
         }
         default:
         {
            break;
         }
      }
   }

}

