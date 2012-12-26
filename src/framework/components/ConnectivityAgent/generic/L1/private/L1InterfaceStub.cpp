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
#include "CIpc.hpp"
#include "CSourceAgent.hpp"
#include "CTargetAgent.hpp"
#include "Buffer.hpp"
#include "CDataAccessor.hpp"
#include "CConnectivityAgent.hpp"
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
#include "Logger.hpp"
#include "byteOrder.hpp"
#include "CTimeoutManager.hpp"
#include "CCarrierAdapter.hpp"
#include "UnstableAPI.hpp"


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
   LOG4CPLUS_INFO(logger, "mSCProtocol.connect() returned: " + static_cast<std::string>(err));
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
   LOG4CPLUS_TRACE(logger, "L1InterfaceStub::OnConnection dirId = "
      + convertIntegerToString(dirId));
   mClientDirections.push_back(dirId);
}

void L1InterfaceStub::OnConnectionLost(DirectionID dirId)
{
   LOG4CPLUS_TRACE(logger, "L1InterfaceStub::onConnectionLost dirId = " 
      + convertIntegerToString(dirId));
   std::remove(mClientDirections.begin(), mClientDirections.end(), dirId);
}

ERROR_CODE L1InterfaceStub::beginAllocateChannel(const TChannelPriority prio, const UInt32 channel_id, const bool client_side, DirectionID& dirId)
{
   LOG4CPLUS_TRACE_METHOD(logger, "L1InterfaceStub::beginAllocateChannel(" + convertIntegerToString(channel_id) + ", "
      "client_side" + std::string(client_side ? "true" : "false") +")");
   ERROR_CODE ret = ERR_UNKNOWN;

   if (mL1ChannelRegistry.find(channel_id) == mL1ChannelRegistry.end()) //if channel wasn't opened yet
   {
      tRequestedChannelsMap::iterator iter = mRequestedMap.find(channel_id);
      if (iter == mRequestedMap.end()) // if channel wasn't requested yet ->request channel
      {
         // First call on SEQ_A and SEQ_B or SEQ_A DEFERRED

         tRequestedChannelInfo info;
         info.mType =  prio;
         info.mState = (client_side ? E_TRANSITION_CLIENT : E_TRANSITION_AGENT);
         info.mClientDir = dirId;

         assert((client_side && dirId != -1) || (!client_side && dirId == -1));

         LOG4CPLUS_INFO(logger, "L1InterfaceStub::beginAllocateChannel() =>insert request"
            " channel_id " + convertIntegerToString(channel_id) +
            " mState " + std::string(info.mState == E_TRANSITION_CLIENT ? "E_TRANSITION_CLIENT" : "E_TRANSITION_AGENT"));

         mRequestedMap.insert(std::pair<UInt32, tRequestedChannelInfo>(channel_id, info));

         ret = (client_side ? ERR_OK : ERR_DEFERRED);
      }
      else
      {
         tRequestedChannelInfo & info = iter->second;
         if (!client_side || (client_side && info.mState == E_TRANSITION_AGENT)) //if channel was requested by other side
         {
            // SEQ_A DEFERRED or call from other side

            LOG4CPLUS_INFO(logger, "L1InterfaceStub::beginAllocateChannel() =>Opening channel locally");

            if (client_side)
               info.mClientDir = dirId;
            else
               dirId = info.mClientDir;

            ret = allocateChannelLocally(channel_id, info);

            assert(ret != ERR_DEFERRED);
            
            mRequestedMap.erase(channel_id);
         }
         else  //channel was requested by Client Application not the first time
         {
            // SEQ_E_1

            LOG4CPLUS_ERROR(logger, "L1InterfaceStub::beginAllocateChannel() => ERROR: allocation is in progress! ");
            ret = ERR_IN_PROGRESS;
         }
      }
   }
   else //channel already opened
   {
      // SEQ_E_2

      LOG4CPLUS_ERROR(logger, "L1InterfaceStub::beginAllocateChannel() => ERROR: channel already exists! ");
      ret = ERR_NUMBER_BUSY;
   }
   
   return ret;
}

ERROR_CODE L1InterfaceStub::allocateChannelLocally(const UInt32 channel_id, tRequestedChannelInfo const& req_info)
{
   LOG4CPLUS_TRACE_METHOD(logger, "L1InterfaceStub::allocateChannelLocally"
      " channel_id" + convertIntegerToString(channel_id));

   ERROR_CODE ret = ERR_FAIL;

   assert(mL1ChannelRegistry.find(channel_id) == mL1ChannelRegistry.end());

   assert(req_info.mState == E_TRANSITION_AGENT || req_info.mState == E_TRANSITION_CLIENT);

   ///>FIXME Set up correct thresholds
   tL1ChannelInfo info;
   info.mType                 = req_info.mType;
   info.mClientDir            = req_info.mClientDir;
   info.mState                = req_info.mState;
   info.mLowerThresholdTime   = 0;
   info.mUpperThresholdTime   = 0xFFFFFFFF;

   ret = CChannelAllocator::getInstance()->allocateChannel(info.mType, channel_id, info);

   LOG4CPLUS_INFO(logger, "L1InterfaceStub::allocateChannelLocally():  CChannelAllocator ret = "
         + convertIntegerToString((int)ret));

   /// @todo correct processing of error code and return values. PIlin, 31.08.12
   ret = ERR_OK;

   if (ERR_OK == ret)
   {
      mL1ChannelRegistry.insert(std::pair<UInt32,tL1ChannelInfo >(channel_id, info));
   }

   return ret;
}

ERROR_CODE L1InterfaceStub::endAllocateChannel(const UInt32 channel_id, DirectionID& dirId)
{
   ERROR_CODE err = ERR_OK;
   mRegistryMutex.lock();
   {
      tChannelsRegistryMap::iterator iter = mL1ChannelRegistry.find(channel_id);
      if (iter != mL1ChannelRegistry.end())
      {
         tL1ChannelInfo & info = iter->second;

         if (E_TRANSITION_AGENT == info.mState)
            err = ERR_DEFERRED;
         
         LOG4CPLUS_INFO(logger, "L1InterfaceStub::endAllocateChannel(): E_OPERATIONAL");
         info.mState = E_OPERATIONAL;

         dirId = info.mClientDir;
      }
      else
      {
         // SEQ_E_4
         err = ERR_NOTFOUND;
         LOG4CPLUS_ERROR(logger, "L1InterfaceStub::endAllocateChannel(): ERR_NOTFOUND");
      }
   }
   mRegistryMutex.unlock();

   return err;
}

ERROR_CODE L1InterfaceStub::failAllocateChannel(const UInt32 channel_id, DirectionID& dirId)
{
   mRequestedMapMutex.lock();
   mRegistryMutex.lock();
   {
      {
         tRequestedChannelsMap::iterator iter = mRequestedMap.find(channel_id);
         if (iter != mRequestedMap.end())
         {
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::failAllocateChannel(): remove requset");

            tRequestedChannelInfo & info = iter->second;
            dirId = info.mClientDir;

            mRequestedMap.erase(iter);
         }
      }

      {
         tChannelsRegistryMap::iterator iter = mL1ChannelRegistry.find(channel_id);

         if (iter != mL1ChannelRegistry.end())
         {
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::failAllocateChannel(): remove registry entry and deallocate channel");

            /*ret = */CChannelAllocator::getInstance()->deallocateChannel(channel_id);
            tL1ChannelInfo & info = iter->second;
            dirId = info.mClientDir;

            mL1ChannelRegistry.erase(iter);
         }
      }
   }
   mRegistryMutex.unlock();
   mRequestedMapMutex.unlock();

   return ERR_OK;
}

ERROR_CODE L1InterfaceStub::deallocateChannel(UInt32 channel_id)
{
   LOG4CPLUS_TRACE(logger, "L1InterfaceStub::deallocateChannel(chID = " + convertIntegerToString(channel_id) + ")");
   ERROR_CODE ret = ERR_NOTFOUND;
   mRegistryMutex.lock();

   if (mL1ChannelRegistry.find(channel_id) != mL1ChannelRegistry.end())
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
   tChannelsRegistryMap::iterator iter = mL1ChannelRegistry.find(accessor.getChannelID());

   if (iter != mL1ChannelRegistry.end())
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
   LOG4CPLUS_TRACE(logger, "L1InterfaceStub::receiveData() => channel " + convertIntegerToString(channel_id));

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
            
            UInt32 error_code = ByteOrder::ntoh32(*reinterpret_cast<UInt32*> (buffer.getData() + offset));
            offset += 4;
            accessor.setErrorCode(error_code);

            accessor.setData(buffer.getData() + offset, data_size);
            offset += data_size;
            
            accessor.printContent();
            ///>Process service request
            tServiceCallbacksMap::iterator iter = mServiceCallbacksMap.find(static_cast<tOpCode>(operation));

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
         offset = 4*sizeof(UInt32);
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
            &mL1ChannelRegistry[channel_id].mClientDir);

         delete [] buf;
      }
      ret = ERR_OK;

   return ret;
}

ERROR_CODE L1InterfaceStub::sendRequest( CDataAccessor & accessor)
{
   Buffer buf;
   UInt32 offset = 0;

   accessor.printContent();

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

   *reinterpret_cast<UInt32*> (buf.getData() + offset) = ByteOrder::hton32(accessor.getErrorCode());
   offset += 4;

   if (accessor.getDataSize())
   {
      memcpy(buf.getData() + offset,accessor.getData(), accessor.getDataSize() );
   }

   ERROR_CODE ret = mL1ChannelRegistry[SERVICE_CHANNEL_NUMBER].mpSourceAgent->fillBuffer(buf);

   buf.forgetData();

   return ret;
}

void L1InterfaceStub::OnConnected()
{
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::OnConnected(): Connectivity Agents connected, create service channel ....");

   tRequestedChannelInfo info;
   info.mType = eSafetyCritical;
   info.mClientDir = -1;
   info.mState = E_TRANSITION_CLIENT;

   mRegistryMutex.lock();
   allocateChannelLocally(SERVICE_CHANNEL_NUMBER, info);
   mRegistryMutex.unlock();

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

   mClientsCallbacksMap.insert(std::make_pair(E_ALLOCATE_CHANNEL,    &L1InterfaceStub::processClientAllocateRequest));
   mClientsCallbacksMap.insert(std::make_pair(E_DEALLOCATE_CHANNEL,  &L1InterfaceStub::processClientDeallocateRequest));
   mClientsCallbacksMap.insert(std::make_pair(E_SEND_DATA,           &L1InterfaceStub::processClientSendRequest));
   mClientsCallbacksMap.insert(std::make_pair(E_GET_CONNECTION_ADDR, &L1InterfaceStub::processClientGetConnectionAddrRequest));

   mServiceCallbacksMap.insert(std::make_pair(E_ALLOCATE_CHANNEL,        &L1InterfaceStub::processServiceAllocateRequest));
   mServiceCallbacksMap.insert(std::make_pair(E_DEALLOCATE_CHANNEL,      &L1InterfaceStub::processServiceDeallocateRequest));
   mServiceCallbacksMap.insert(std::make_pair(E_ALLOCATE_CHANNEL_RESP,   &L1InterfaceStub::processServiceAllocateResponse));
   mServiceCallbacksMap.insert(std::make_pair(E_DEALLOCATE_CHANNEL_RESP, &L1InterfaceStub::processServiceDeallocateResponse));

}

void L1InterfaceStub::OnRequest(iviLink::Ipc::MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      UInt8* const pResponseBuffer, UInt32& bufferSize,
      DirectionID dirId)
{
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::OnRequest(): received " + convertIntegerToString(payloadSize) + " bytes");

   const UInt32 savedBufferSize = bufferSize;
   // no response by default
   bufferSize = 0;

   CDataAccessor pReq(pPayload, payloadSize);
   pReq.printContent();

   bool send_result = false;

   tClientsCallbacksMap::const_iterator callback_iter = mClientsCallbacksMap.find(static_cast<tOpCode>(pReq.getOpCode()));
   if (callback_iter != mClientsCallbacksMap.end())
   {
      send_result = (this->*callback_iter->second)(pReq, dirId);
   }
   else
   {
      LOG4CPLUS_WARN(logger, "L1InterfaceStub::OnRequest() => UNKNOWN REQUEST!");

      pReq.resetData();
      pReq.setErrorCode(ERR_UNKNOWN);
      send_result = true;
   }

   /*Here was a "<" condition, getConnectionAddr did not work */
   if (send_result && (savedBufferSize >= pReq.getObjectSize() ) )
   {
      bufferSize = pReq.getObjectSize();
      pReq.copyToRawArray(pResponseBuffer);
   }
}

void L1InterfaceStub::OnAsyncRequest(iviLink::Ipc::MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      DirectionID dirId)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
}

bool L1InterfaceStub::processClientAllocateRequest(CDataAccessor & accessor, const iviLink::Ipc::DirectionID dirId)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   const UInt32 channel_id = accessor.getChannelID();
   
   if(SERVICE_CHANNEL_NUMBER == channel_id)
   {
      // SEQ_E_5

      LOG4CPLUS_WARN(logger, "Channel SERVICE_CHANNEL_NUMBER is not allowed to be open");

      accessor.resetData();
      accessor.setErrorCode(ERR_NUMBER_BUSY);

      return true;
   }

   UInt32 prio = 0;
   memcpy(&prio, accessor.getData(), accessor.getDataSize());
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientAllocateRequest() => Allocate Channel Request: ChID = "
      + convertIntegerToString(channel_id) + ", prio = " + convertIntegerToString(prio));

   iviLink::Ipc::DirectionID tmpDirId = dirId;
   ERROR_CODE err = ERR_FAIL;

   mRequestedMapMutex.lock();
   mRegistryMutex.lock();
   {
      err = beginAllocateChannel(static_cast<TChannelPriority>(prio), channel_id, true, tmpDirId);

      accessor.setErrorCode(err);

      if (ERR_OK == err)
      {
         LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientAllocateRequest() => all ok, sending request to other side" );
         err = sendRequest(accessor);
      }
   }
   mRegistryMutex.unlock();
   mRequestedMapMutex.unlock();

   if (ERR_OK == err) //> all ok
   {
      return false;
   }
   else
   {
      // something wrong, need message about error
      // SEQ_E_5

      assert(err != ERR_DEFERRED);

      accessor.resetData();
      accessor.setErrorCode(err);

      return true;
   }
}

bool L1InterfaceStub::processClientDeallocateRequest(CDataAccessor & accessor, const iviLink::Ipc::DirectionID dirId)
{
   UInt32 channel_id = accessor.getChannelID();
   if(SERVICE_CHANNEL_NUMBER !=channel_id)
   {
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientDeallocateRequest() => Deallocate Channel Request: ChID = " + convertIntegerToString(channel_id));
      mRegistryMutex.lock();
      tChannelsRegistryMap::iterator iter =  mL1ChannelRegistry.find(channel_id);
      if (iter != mL1ChannelRegistry.end())
      {
         iter->second.mState = E_TRANSITION_CLIENT;
         sendRequest(accessor);
         new CRequestTimeout(250000,static_cast<tOpCode>(accessor.getOpCode()),channel_id,this );
         // ret = ERR_OK;
      }
      else
      {
         LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientDeallocateRequest() => Deallocate Channel Request: ChID = " + convertIntegerToString(channel_id) + " NOT FOUND!");
         // ret = ERR_NOTFOUND;
      }
      mRegistryMutex.unlock();
   }

   return false;
}

bool L1InterfaceStub::processClientSendRequest(CDataAccessor & accessor, const iviLink::Ipc::DirectionID dirId)
{
   UInt32 channel_id = accessor.getChannelID();
   if (SERVICE_CHANNEL_NUMBER !=channel_id)
   {
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientSendRequest() => Send Data Request: ChID = " + convertIntegerToString(channel_id));
      sendData(accessor);
   }
   return false;
}

bool L1InterfaceStub::processClientGetConnectionAddrRequest(CDataAccessor & accessor, const iviLink::Ipc::DirectionID dirId)
{
   bool result = false;
   if (this->mpAgent)
   {
      iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pca = mpAgent->getCurrentCarrierAdapter();
      if (pca)
      {
         ConnectionInformation address(pca->getLocalAddress(), pca->getRemoteAddress(), pca->getTypeName());
         UInt8 * serializedData = address.serialize();
         accessor.setData(serializedData, address.getSerializedSize());
         delete[] serializedData;
         result = true;
      }
   }

   if (!result)
   {
      UInt32 tmp = accessor.getOpCode();
      accessor.resetAll();
      accessor.setOpCode(tmp);
   }

   return result;
}

void L1InterfaceStub::processServiceAllocateRequest(CDataAccessor & accessor)
{
   const UInt32 channel_id = accessor.getChannelID();
   const UInt32 prio = *reinterpret_cast<UInt32*>(accessor.getData());
   ERROR_CODE err = static_cast<ERROR_CODE>(accessor.getErrorCode());
   
   LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateRequest()=>Allocate channel prio = "
         + convertIntegerToString(prio) + " id =" + convertIntegerToString(accessor.getChannelID()));

   iviLink::Ipc::DirectionID dirId = -1;

   accessor.setOpCode(E_ALLOCATE_CHANNEL_RESP);
   accessor.resetData();

   if (ERR_OK != err)
   {
      // SEQ_E_3

      failAllocateChannel(channel_id, dirId);

      accessor.setErrorCode(err);
   }
   else
   {
      mRequestedMapMutex.lock();
      mRegistryMutex.lock();
      {
         err = beginAllocateChannel(static_cast<TChannelPriority>(prio), channel_id, false, dirId);

         if (ERR_DEFERRED != err)
         {
            // sending the result to other side
            // SEQ_E_4 or ok
            accessor.setErrorCode(err);  
            sendRequest(accessor);
         }
      }
      mRegistryMutex.unlock();
      mRequestedMapMutex.unlock();

      if (ERR_DEFERRED == err)
      {
         // all ok, we will wait for the client requesting this channel
         // setting timeout

         LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateRequest()=>channel wasn't requested yet ->set channel to wait and trigger timeout");

         new CRequestTimeout(250000,E_ALLOCATE_CHANNEL,channel_id, this);

         return;
      }      
   }

   if (ERR_OK != err)
   {
      // messagign about failed channel allocation
      // SEQ_E_4

      if (dirId != -1)
         sendIpcNotification(accessor, dirId);
   }
}

void L1InterfaceStub::processServiceDeallocateRequest(CDataAccessor & accessor)
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
      tChannelsRegistryMap::iterator iter = mL1ChannelRegistry.find(accessor.getChannelID());
      if (mL1ChannelRegistry.end() != iter)
      {
         dirId = iter->second.mClientDir;
         found = true;

         LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceDeallocateRequest()=>channel found in registry - removing");
         mL1ChannelRegistry.erase(accessor.getChannelID());
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
}

void L1InterfaceStub::processServiceAllocateResponse(CDataAccessor & accessor)
{
   const UInt32 channel_id = accessor.getChannelID();
   ERROR_CODE err = static_cast<ERROR_CODE>(accessor.getErrorCode());

   LOG4CPLUS_TRACE_METHOD(logger, "L1InterfaceStub::processServiceAllocateResponse()=>Channel allocated responce id = "
         + convertIntegerToString(channel_id) + ", err = " + convertIntegerToString((int)err));

   iviLink::Ipc::DirectionID dirId = -1;

   /// @todo better processing of error codes. PIlin, 31.08.12
   /// There is the error case with wrong processing:
   /// 1) if response with error, obtain ERR_DEFERRED from failAllocateChannel and 
   ///   begin deallocation procedure.
   /// 2) if there is no known dirId, channel also needs to be deallocated, even
   /// if its allocation was successfull.
   ///
   /// Here we must initialize channel deallocation procedure, because we unable 
   /// to send some message - there is no such in existing protocol.

   if (ERR_OK == err)
   {
      err = endAllocateChannel(channel_id, dirId);

      if (ERR_DEFERRED == err)
      {
         LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateResponse()=>was E_TRANSITION_AGENT, messaging to other side");
         accessor.setErrorCode(ERR_OK);
         err = sendRequest(accessor);
      }
   }

   if (ERR_OK != err || dirId == -1)
   {
      // SEQ_E_4

      LOG4CPLUS_ERROR(logger, "L1InterfaceStub::processServiceAllocateResponse()=> failed channel allocation");

      failAllocateChannel(channel_id, dirId);
      accessor.setErrorCode(err);
   }

   if (dirId != -1)
   {
      LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateResponse()=>message about allocation result");
      sendIpcNotification(accessor, dirId);
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "L1InterfaceStub::processServiceAllocateResponse()=> unknown client, failing channel");
   }
}

void L1InterfaceStub::processServiceDeallocateResponse(CDataAccessor & accessor)
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
            tChannelsRegistryMap::iterator iter = mL1ChannelRegistry.find(channel_id);
            if (iter != mL1ChannelRegistry.end())
            {
               dirId = iter->second.mClientDir;
               found = true;
               mL1ChannelRegistry.erase(iter);
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
}


void L1InterfaceStub::sendIpcNotification(CDataAccessor & accessor, iviLink::Ipc::DirectionID dirId)
{
   assert(dirId != -1);

   UInt8* buf = new UInt8[accessor.getObjectSize()];
   accessor.copyToRawArray(buf);

   UInt32 respSize = 0;
   CError err = mpIpc->request(mMsgIdGen.next(),
      buf, accessor.getObjectSize(),
      NULL, respSize,
      &dirId);

   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(logger, "sendIpcNotification error : " + static_cast<std::string>(err));
   }

   delete [] buf;
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
            bool found = (mpOwner->mL1ChannelRegistry.find(mChannelID) != mpOwner->mL1ChannelRegistry.end());
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
               resp.setErrorCode(ERR_TIMEOUT);
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
               tChannelsRegistryMap::iterator iter = mpOwner->mL1ChannelRegistry.find(mChannelID);
               found = (iter != mpOwner->mL1ChannelRegistry.end());
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
               resp.setErrorCode(ERR_TIMEOUT);
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

