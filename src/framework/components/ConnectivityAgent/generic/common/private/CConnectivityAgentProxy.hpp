/* 
 * 
 * iviLINK SDK, version 1.0
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






#ifndef CONNECTIVITY_AGENT_PROXY_HPP
#define CONNECTIVITY_AGENT_PROXY_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <map>
#include <vector>
#include <list>
#include <set>
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/ipc/ICallbackHandler.hpp"
#include "utils/misc/Types.hpp"
#include "utils/threads/CCondVar.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/threads/CRWMutex.hpp"
#include "utils/threads/CSignalSemaphore.hpp"
#include "utils/threads/CThread.hpp"

#include "Buffer.hpp"
#include "framework/components/ConnectivityAgent/generic/common/CDataAccessor.hpp"

namespace iviLink{
namespace Ipc{
class CIpc;
} // namespace Ipc
} // namespace AXIS

class IChannelObserver;
class CCallbackWrapper;

///CConnectivityAgentProxy class
/**
 * The CConnectivityAgentProxy implemented as Singleton. It's task is to translate API Calls from Client App
 * to requests and send them to Connectivity Agent process, and also provide the callbacks to CLient when needed
 * It uses UNIX Sockets to connect to client application and uses DataAccessors to pack data flow.
 */

class  CConnectivityAgentProxy : 
  public CThread,
  public iviLink::Ipc::ICallbackHandler
{
private:
   static Logger logger;

public:

  // Types section

  /**
    * Type for storing channel config
    */
   struct tChannelInfo
   {
      TChannelPriority                       mType;
      IChannelObserver*                      mpClient;
      iviLink::ConnectivityAgent::Buffer        mChannelBuffer;
   };

   /**
    * Channels map type
    */
   typedef std::map<UInt32,tChannelInfo> tChannelsRegistryMap;

   // Methods section

   /**
    * Interface function to provide sigleton behavior
    * @return pointer to existing instance of singleton and creates it if not exists
    */
   static CConnectivityAgentProxy* getInstance();

   /**
    * Interface function to destroy the singleton instance
    */
   void deleteInstance();

   /**
    * Allocates duplex channel with given number and priority
    * @param prio channel priority
    * @param channel_id channel number
    * @param observer client observer of the channel
    * @return ERR_OK if it's allright, ERR_NUMBER_BUSY or ERR_FAIL if it's not
    */
   ERROR_CODE allocateChannel(TChannelPriority prio, UInt32 channel_id, IChannelObserver* observer);

   /**
    * Deallocates duplex channel with given number and sends the allocation request to the other side Connectivity Agent process
    * @param channel_id channel number
    * @return ERR_OK if it's allright, ERR_NOTFOUND or ERR_FAIL if it's not
    */
   ERROR_CODE deallocateChannel(UInt32 channel_id);
   /**
    * Sends the raw data stream through the channel
    * @param channel_id channel number
    * @param data pointer to byte stream
    * @param size data size for sending
    * @return ERR_OK if it's allright, ERR_NOTFOUND or ERR_FAIL if it's not
    */
   ERROR_CODE sendData(UInt32 channel_id, UInt8 const* data, UInt32 size);
   /**
    * Receives the raw data stream from channel buffer
    * @param channel_id channel number
    * @param data pointer to byte stream to store the data
    * @param received_size actual received data size
    * @param max_size allocated  size of data buffer
    * @return ERR_OK if it's allright, ERR_NOTFOUND or ERR_FAIL if it's not
    */
   ERROR_CODE receiveData(UInt32 channel_id, UInt8* data, UInt32 & received_size, UInt32 max_size);

   /**
    * Retrieve free size left in channel
    * @param channel_id channel number
    * @param[out] free_size free size left in channel. In case of error, value of
    *    param is undefined.
    * @retval ERR_OK operation successful, result in @c free_size param
    * @retval ERR_NOTFOUND channel with specified @c channel_id is not found
    * @retval ERR_FAIL unknown error
    */
   ERROR_CODE getFreeSize(UInt32 channel_id, UInt32& free_size) const;

public:
   // From AXIS::Ipc::ICallbackHandler

   virtual void OnConnection(iviLink::Ipc::DirectionID dirId);

   virtual void OnConnectionLost(iviLink::Ipc::DirectionID dirId);

   virtual void OnRequest(iviLink::Ipc::MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      UInt8* const pResponseBuffer, UInt32& bufferSize,
      iviLink::Ipc::DirectionID dirId);

public:
   // From CThread

   virtual void threadFunc();

private:

   // Methods section

   /**
    * Receives the nofication about arrived data from Connectivity Agent and adds it to channel buffer.
    * If there are not enough free space in buffer - sends buffer overflow callback
    * @param accessor data accessor that caontains data and info
    */
   void receiveDataNotification(iviLink::ConnectivityAgent::CDataAccessor & accessor);

   /**
    * Receives the notification about closed channel from Connectivity Agent.
    * If channel is not deleted from this side - notifies the observer about
    * closed channel and removes it from registry.
    * If channel is being deallocated from this side - notifies the
    * deallocateChannel() about it.
    * If channel is already deleted - does nothing.
    * @param accessor data accessor that contains info about deleted channel
    */
   void channelDeletedNotification(iviLink::ConnectivityAgent::CDataAccessor & accessor);

   /**
    * Callback invoked by Connectivity Agent when physical connection is broken
    */
   void OnDisconnected();

   /**
    * Constructor is private to provide sigleton behavior
    */
   CConnectivityAgentProxy();

   /**
    * Destructor
    */
   ~CConnectivityAgentProxy();

   /**
    * Copying constructor
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is constructing on
    */
   CConnectivityAgentProxy(const CConnectivityAgentProxy&);

   /**
    * Assigning operator
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is assiging from
    */

   CConnectivityAgentProxy& operator=(const CConnectivityAgentProxy&);

   // Members section

   tChannelsRegistryMap                   mRegistry;
   mutable CRWMutex                       mRegistryMutex;
   iviLink::Ipc::CIpc*                       mpIpc;

   typedef std::set<UInt32>               tChannelOnDeallocSet;
   tChannelOnDeallocSet                   mChannelOnDeallocSet;


   typedef std::map<UInt32, iviLink::ConnectivityAgent::CDataAccessor> tRequestResultMap;
   CCondVar                               mAllocateRequestResultCond;
   tRequestResultMap                      mAllocateRequestResultMap;

   CCondVar                               mDeallocateRequestResultCond;
   tRequestResultMap                      mDeallocateRequestResultMap;

   CSignalSemaphore                       mCallbackSema;
   std::list<CCallbackWrapper* >          mCallbackList;
   CMutex                                 mCallbackListMutex;
   static CMutex                          mSingletonMutex;
   static CConnectivityAgentProxy*        mSelf;

   class MsgIdGen 
   {
      iviLink::Ipc::MsgID    mRequestId;
      CMutex              mRequestIdMutex;
   public:
      MsgIdGen() : 
         mRequestId(0)
      { }

      iviLink::Ipc::MsgID next() 
      {
         mRequestIdMutex.lock();
         iviLink::Ipc::MsgID id = mRequestId;
         mRequestId += 2;
         mRequestIdMutex.unlock();
         return id;
      }
   };
   
   MsgIdGen                               mMsgIdGen;
};
#endif
