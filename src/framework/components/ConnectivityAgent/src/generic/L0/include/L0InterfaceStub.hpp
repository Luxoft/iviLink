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




#ifndef L0INTERFACE_STUB_HPP
#define L0INTERFACE_STUB_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <map>
#include <vector>
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/ipc/include/ICallbackHandler.hpp"
#include "utils/misc/include/Types.hpp"
#include "utils/threads/include/ITimeoutSubscriber.hpp"
#include "utils/threads/include/CMutex.hpp"
#include "framework/components/ConnectivityAgent/src/generic/common/include/API.hpp"
#include "framework/components/ConnectivityAgent/src/generic/L0/include/CSystemControllerProtocol.hpp"

class CSourceAgent;
class CTargetAgent;
class CDataAccessor;
class CConnectivityAgent;
struct Buffer;

///L0InterfaceStub class
/**
 * The L0InterfaceStub implemented as Singleton. It's task is to manage the channels allocation and deallocation
 * and data flow requests from Client application. The only exclusion is creating of service channel which is
 * initiated by HAL when physical connection is established.
 * It uses UNIX Sockets to connect to client application and uses DataAccessors to pack data flow
 */

class  L0InterfaceStub : 
   public AXIS::Ipc::ICallbackHandler
{
private:
   static Logger logger;

public:

  // Types section
   /**
    * Channel state
    */
   enum tChannelState
   {
      E_UNDEFINED,  //!< eUNDEFINED
      E_TRANSITION, //!< eTRANSITION
      E_OPERATIONAL //!< eOPERATIONAL
   };
   /**
    * Message Sender
    */
   enum tMessageSender
   {
      E_CLIENT,
      E_AGENT
   };

   /**
    * Type for storing allocated channel config
    */
   struct tChannelInfo
   {
      UInt64               mUpperThresholdTime;
      UInt64               mLowerThresholdTime;
      tChannelState        mState;
      TChannelPriority     mType;
      CSourceAgent*          mpSourceAgent;
      CTargetAgent*          mpTargetAgent;
      AXIS::Ipc::DirectionID mClientDir;
   };
   /**
    * Type for storing requested channel config
    */
   struct tRequestedChannelInfo
   {
      TChannelPriority       mType;
      tMessageSender         mSender;
      AXIS::Ipc::DirectionID mClientDir;
   };
   /**
    * Allocated Channels map type
    */
   typedef std::map<UInt32,tChannelInfo> tChannelsRegistryMap;

   /**
    * Requested Channels map type
    */
   typedef std::map<UInt32,tRequestedChannelInfo> tRequestedChannelsMap;

   /**
    * Request processor callback type
   */
   typedef ERROR_CODE (L0InterfaceStub::* process_func)(CDataAccessor& );

   // Methods section

   /**
    * Interface function to provide sigleton behavior
    * @return pointer to existing instance of singleton and creates it if not exists
    */
   static L0InterfaceStub* getInstance();

   /**
    * Interface function to destroy the singleton instance
    */
   void deleteInstance();

public:
   // From ICallbackHandler

   virtual void OnConnection(DirectionID dirId);

   virtual void OnConnectionLost(DirectionID dirId);

   virtual void OnRequest(AXIS::Ipc::MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      UInt8* const pResponseBuffer, UInt32& bufferSize,
      DirectionID dirId);

public:
   /**
    * Function called by Target agents to receive data from specified channel
    * @param buf data buffer
    * @param channel_id channel number data received from
    * @return ERR_OK
    */
    ERROR_CODE receiveData(Buffer & buffer, UInt32 channel_id);

   /**
    * Function starts the system
    * @param isServer are we a server
    * @param sockPath path to Unix Socket  file descriptor
    */
   void start(bool isServer, const char* sockPath);

   /**
    * Callback from HAL that physical connection established, and Agents found each other
    */
   void OnConnected();

   /**
    * Callback from HAL that physical connection is broken
    */
   void OnDisconnected();

   void onShutDown()
   {
      //bShutDownFlag = true;
      mSCProtocol.sendConnectionLost();
   }

   bool getShutdownState()
   {
      return bShutDownFlag;
   }

   private:
  // Types section

   /**
    * Class CRequestTimeout is used to invoke action if the request timeout passed
    */

   class CRequestTimeout: public ITimeoutSubscriber
   {
      public:

      // Methods section

      /**
       * Constructor
       * @param timeout timeout value in milliseconds for request processing
       * @param req type of request to choose the timeout action
       * @param channel_id  channel number which request belongs to
       */
      CRequestTimeout( UInt32 timeout, tOpCode req, UInt32 channel_id,L0InterfaceStub* pOwner, bool enabled = true );

      //from ITimeoutSubscriber

      /**
       * Callback for TimeoutManager which invokes when timeout passed
       */
      virtual void onTimeout();

      private:
      // Methods section

      /**
       * Not implemented to avoid missusing
       */
      CRequestTimeout(const CRequestTimeout&);
      CRequestTimeout & operator = (const CRequestTimeout&);

      // Members section

      tOpCode                      mRequest;
      UInt32                       mChannelID;
      L0InterfaceStub*             mpOwner;
      bool                         mEnabled;

   };

   friend class CRequestTimeout;

   // Methods section
   /**
    * Allocates duplex channel with given number and priority and sends the allocation request to the
    * other side Connectivity Agent process
    * @param prio channel priority
    * @param channel_id channel number
    * @return ERR_OK if it's allright, ERR_NUMBER_BUSY or ERR_FAIL if it's not
    */
   ERROR_CODE allocateChannel(TChannelPriority prio, UInt32 channel_id);

   /**
    * Request to allocate duplex channel with given number and priority and sends the allocation request to the
    * other side Connectivity Agent process. Or in case the channel was already requested -set it to reassign
    * @param prio channel priority
    * @param channel_id channel number
    * @return ERR_OK if it's allright, ERR_NUMBER_BUSY or ERR_FAIL if it's not
    */

   ERROR_CODE tryAllocateChannel(TChannelPriority prio, UInt32 channel_id);
   /**
    * Deallocates duplex channel with given number and sends the allocation request to the other side Connectivity Agent process
    * @param channel_id channel number
    * @return ERR_OK if it's allright, ERR_NOTFOUND or ERR_FAIL if it's not
    */
   ERROR_CODE deallocateChannel(UInt32 channel_id);

   /**
    * Sends data received from Client App through specified channel
    * @param Accessor client's data accessor contained channel number and data itself
    * @return ERR_OK if it's allright, ERR_NOTFOUND or ERR_FAIL if it's not
    */
   ERROR_CODE sendData(CDataAccessor & Accessor);

   /**
    * Sends service requests for channel allocation/deallocation to the other side Connectivity Agent process
    * @param Accessor data accessor contained request
    */
   void sendRequest(CDataAccessor & accessor);

   /**
    * Service Allocate request processing function
    * @param accessor refernce to data accessor contained request related data
    */
   ERROR_CODE processServiceAllocateRequest(CDataAccessor & accessor);

   /**
    * Service Deallocate request processing function
    * @param accessor reference to data accessor contained request related data
    */
   ERROR_CODE processServiceDeallocateRequest(CDataAccessor & accessor);

   /**
    * Service Allocate response processing function
    * @param accessor reference to data accessor contained request related data
    */
   ERROR_CODE processServiceAllocateResponse(CDataAccessor & accessor);

   /**
    * Service Deallocate response processing function
    * @param accessor reference to data accessor contained request related data
    */
   ERROR_CODE processServiceDeallocateResponse(CDataAccessor & accessor);

   /**
    * CLient Allocate request processing function
    * @param accessor reference to data accessor contained request related data
    */
   ERROR_CODE processClientAllocateRequest(CDataAccessor & accessor);

   /**
    * Client Deallocate request processing function
    * @param accessor reference to data accessor contained request related data
    */
   ERROR_CODE processClientDeallocateRequest(CDataAccessor & accessor);

   /**
    * Client Send request processing function
    * @param accessor reference to data accessor contained request related data
    */
   ERROR_CODE processClientSendRequest(CDataAccessor & accessor);

   /**
    * Fills callback maps for service and clients requests processing.
    * Should be called at startup
    */
   void fillCallbacks();

   /**
    * Constructor is private to provide sigleton behavior
    */
   L0InterfaceStub();

   /**
    * Destructor
    */
   ~L0InterfaceStub();

   /**
    * Copying constructor
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is constructing on
    */
   L0InterfaceStub(const L0InterfaceStub&);

   /**
    * Assigning operator
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is assiging from
    */

   L0InterfaceStub& operator=(const L0InterfaceStub&);

   // Members section

   //> Map of callbacks that serves requests from client applications
   std::map<tOpCode, process_func>                    mClientsCallbacksMap;

   //> Map of callbacks that serves requests from other side Connectivity agent
   std::map<tOpCode, process_func>                    mServiceCallbacksMap;

   //> Map of allocated channels
   tChannelsRegistryMap                               mRegistry;

   //> mutex for map of allocated channels
   CMutex                                             mRegistryMutex;

   //> Map of requested but not yet allocated channels
   tRequestedChannelsMap                              mRequestedMap;

   //> mutex for map of requested channels
   CMutex                                             mRequestedMapMutex;

   //> server ipc for connection with Proxies on clients sides
   AXIS::Ipc::CIpc*                                   mpIpc;

   //> pointer to CConnectivityAgent class that starts up HAL infrastructure
   CConnectivityAgent*                                mpAgent;


   //> type for mClientDirections
   typedef std::vector<AXIS::Ipc::DirectionID>        tClientDirections;
   //> socket for data transfer with Proxies on clients sides
   std::vector<AXIS::Ipc::DirectionID>                mClientDirections;

   //> singleton mutex in case of concurrent calls
   static CMutex                                      mSingletonMutex;

   //> singleton instance
   static L0InterfaceStub*                            mSelf;
   
   CSystemConrollerProtocol                           mSCProtocol;
   bool                                               bShutDownFlag;

   class MsgIdGen 
   {
      AXIS::Ipc::MsgID    mRequestId;
      CMutex              mRequestIdMutex;
   public:
      MsgIdGen() :
         mRequestId(1)
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
   
   MsgIdGen                                           mMsgIdGen;
};
#endif
