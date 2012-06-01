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

/***************************************************************************
* Project           AXIS
* (c) copyright     2012
* Company           LUXOFT
* @file             CConnectivityAgentProxy.hpp
* @brief            The CConnectivityAgentProxy class header file
* @author           KSkidanov
***************************************************************************/
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
#include"utils/misc/include/Types.hpp"
#include"utils/ipc/include/ICallbackHandler.hpp"
#include"framework/components/ConnectivityAgent/src/generic/HAL/include/Buffer.hpp"
#include"utils/threads/include/CMutex.hpp"
#include"utils/threads/include/CSignalSemaphore.hpp"
#include"utils/threads/include/CRWMutex.hpp"
#include"framework/components/ConnectivityAgent/src/generic/L0/include/CDataAccessor.hpp"
#include"utils/threads/include/CCondVar.hpp"
#include"utils/threads/include/CThread.hpp"

namespace AXIS{
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
  public AXIS::Ipc::ICallbackHandler
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
      TChannelPriority              mType;
      IChannelObserver*             mpClient;
      Buffer                        mChannelBuffer;
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
   ERROR_CODE sendData(UInt32 channel_id, UInt8* data, UInt32 size);
   /**
    * Receives the raw data stream from channel buffer
    * @param channel_id channel number
    * @param data pointer to byte stream to store the data
    * @param received_size actual received data size
    * @param max_size allocated  size of data buffer
    * @return ERR_OK if it's allright, ERR_NOTFOUND or ERR_FAIL if it's not
    */
   ERROR_CODE receiveData(UInt32 channel_id, UInt8* data, UInt32 & received_size, UInt32 max_size);

public:
   // From AXIS::Ipc::ICallbackHandler

   virtual void OnConnection(AXIS::Ipc::DirectionID dirId);

   virtual void OnConnectionLost(AXIS::Ipc::DirectionID dirId);

   virtual void OnRequest(AXIS::Ipc::MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      UInt8* const pResponseBuffer, UInt32& bufferSize,
      AXIS::Ipc::DirectionID dirId);

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
   void receiveDataNotification(CDataAccessor & accessor);

   /**
    * Receives the notification about closed channel from Connectivity Agent.
    * If channel is not deleted from this side - notifies the observer about
    * closed channel and removes it from registry.
    * If channel is being deallocated from this side - notifies the
    * deallocateChannel() about it.
    * If channel is already deleted - does nothing.
    * @param accessor data accessor that contains info about deleted channel
    */
   void channelDeletedNotification(CDataAccessor & accessor);

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
   CRWMutex                               mRegistryMutex;
   AXIS::Ipc::CIpc*                       mpIpc;

   typedef std::set<UInt32>               tChannelOnDeallocSet;
   tChannelOnDeallocSet                   mChannelOnDeallocSet;


   typedef std::map<UInt32, CDataAccessor> tRequestResultMap;
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
      AXIS::Ipc::MsgID    mRequestId;
      CMutex              mRequestIdMutex;
   public:
      MsgIdGen() : 
         mRequestId(0)
      { }

      AXIS::Ipc::MsgID next() 
      {
         mRequestIdMutex.lock();
         AXIS::Ipc::MsgID id = mRequestId;
         mRequestId += 2;
         mRequestIdMutex.unlock();
         return id;
      }
   };
   
   MsgIdGen                               mMsgIdGen;
};
#endif
