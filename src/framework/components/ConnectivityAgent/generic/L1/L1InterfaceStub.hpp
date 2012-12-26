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
 

#ifndef L1INTERFACE_STUB_HPP
#define L1INTERFACE_STUB_HPP
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
#include "ICallbackHandler.hpp"
#include "Types.hpp"
#include "ITimeoutSubscriber.hpp"
#include "CMutex.hpp"
#include "API.hpp"
#include "CSystemControllerProtocol.hpp"

namespace iviLink
{
   namespace Ipc
   {
      class CIpc;
   }  // namespace Ipc

   namespace ConnectivityAgent
   {
      class Buffer;
      class CDataAccessor;

      namespace HAL
      {
         class CConnectivityAgent;
      }  // namespace HAL

      namespace L1
      {
          class CSourceAgent;
          class CTargetAgent;
          

          ///L1InterfaceStub class
         /**
          * The L1InterfaceStub implemented as Singleton. It's task is to manage the channels allocation and deallocation
          * and data flow requests from Client application. The only exclusion is creating of service channel which is
          * initiated by HAL when physical connection is established.
          * It uses CIpc class to connect to client application and uses DataAccessors to pack data flow
          */
  
         class  L1InterfaceStub : 
            public iviLink::Ipc::ICallbackHandler
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
               E_UNDEFINED,         //!< eUNDEFINED
               E_TRANSITION_CLIENT, //!< eTRANSITION_CLIENT - channel requested from this side
               E_TRANSITION_AGENT,  //!< eTRANSITION_AGENT  - channel requested from other side
               E_OPERATIONAL        //!< eOPERATIONAL - channel operational
            };

            /**
             * Type for storing allocated channel config
             */
            struct tL1ChannelInfo
            {
               UInt64               mUpperThresholdTime;
               UInt64               mLowerThresholdTime;
               tChannelState        mState;
               TChannelPriority     mType;
               CSourceAgent*          mpSourceAgent;
               CTargetAgent*          mpTargetAgent;
               iviLink::Ipc::DirectionID mClientDir;
            };
            /**
             * Type for storing requested channel config
             */
            struct tRequestedChannelInfo
            {
               TChannelPriority          mType;
               tChannelState             mState;
               iviLink::Ipc::DirectionID mClientDir;
            };
            /**
             * Allocated Channels map type
             */
            typedef std::map<UInt32,tL1ChannelInfo> tChannelsRegistryMap;

            /**
             * Requested Channels map type
             */
            typedef std::map<UInt32,tRequestedChannelInfo> tRequestedChannelsMap;

            /**
             * Client request processor callback type
             * @retval true - accessor has been changed and need to be send back
             * @retval false - nothing to be send in response, make it empty
             */
            typedef bool (L1InterfaceStub::* process_client_func)(CDataAccessor&, iviLink::Ipc::DirectionID);

            /**
             * Service request processor callback type
             */
            typedef void (L1InterfaceStub::* process_service_func)(CDataAccessor&);

            /**
             * Type for map of callbacks for client requests received from CIpc
             */
            typedef std::map<tOpCode, process_client_func> tClientsCallbacksMap;

            /**
             * Type for map of callbacks for service requests received from CA
             * on other side by SERVICE_CHANNEL.
             */
            typedef std::map<tOpCode, process_service_func> tServiceCallbacksMap;

            // Methods section

            /**
             * Interface function to provide sigleton behavior
             * @return pointer to existing instance of singleton and creates it if not exists
             */
            static L1InterfaceStub* getInstance();

            /**
             * Interface function to destroy the singleton instance
             */
            void deleteInstance();

         public:
            // From ICallbackHandler

            virtual void OnConnection(iviLink::Ipc::DirectionID dirId);

            virtual void OnConnectionLost(iviLink::Ipc::DirectionID dirId);

            virtual void OnRequest(iviLink::Ipc::MsgID id, 
               UInt8 const* pPayload, UInt32 payloadSize, 
               UInt8* const pResponseBuffer, UInt32& bufferSize,
               iviLink::Ipc::DirectionID dirId);
            virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, 
               UInt8 const* pPayload, UInt32 payloadSize, 
               iviLink::Ipc::DirectionID dirId);

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
               CRequestTimeout( UInt32 timeout, tOpCode req, UInt32 channel_id,L1InterfaceStub* pOwner, bool enabled = true );

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
               L1InterfaceStub*             mpOwner;
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
             * This methods processes channel allocation request.
             * Updates mRequestedMap.
             *
             * Method <b>MUST BE CALLED</b> inside critical section of mutexes
             * mRequestedMapMutex and mRegistryMutex (in that order). 
             *
             * @param prio priority of requested channel.
             * @param channel_id number of requested channel
             * @param client_side if called from processClientAllocateRequest()
             *    must be set to @c true
             * @param[in,out] dirId used for setting/getting direction id.
             *    If @c client_side == true, it sets direction id.
             *    If @c client_side == false, it returns direction id.
             *
             * @retval ERR_OK all ok, see diagram SEQ_A and SEQ_B
             * @retval ERR_DEFERRED all ok, need to wait channel allocation 
             *    request from one of clients on this side. See diagram SEQ_A
             * @retval ERR_NUMBER_BUSY there is channel with specified 
             *    @c channel_id in the mL1ChannelRegistry
             * @reval ERR_IN_PROGRESS attempt to make the second request for the
             *    same channel, while the first request is not ended yet. 
             *    Please, be patient.
             * @return there can be other errors from allocateChannelLocally()
             */
            ERROR_CODE beginAllocateChannel(const TChannelPriority prio, const UInt32 channel_id, const bool client_side, iviLink::Ipc::DirectionID& dirId);

            /**
             * Performs channel allocation using CChannelAllocator.
             * Updates mL1ChannelRegistry.
             *
             * Method <b>MUST BE CALLED</b> inside critical section of mutexes
             * mRequestedMapMutex and mRegistryMutex (in that order). 
             *
             * @param channel_id number of requested channel
             * @param req_info information about requested channel
             *
             * @return result of channel allocation
             * @retval ERR_OK channel successfully allocated
             */
            ERROR_CODE allocateChannelLocally(const UInt32 channel_id, tRequestedChannelInfo const& req_info);

            /**
             * This method finishes channel allocation.
             * Updates mL1ChannelRegistry.
             *
             * @param channel_id number of requested channel
             * @param[out] dirId returns directionId of client which requested 
             *    the channel.
             *
             * @retval ERR_OK all ok, channel successfully allocated
             * @retval ERR_DEFERRED all ok, channel allocated, but need to send
             *    message to the other side
             * @retval ERR_NOTFOUND channel is not found in mL1ChannelRegistry. This is 
             *    legit in case when L1interfaceStub is being destroyed. Or it 
             *    can be some bug.
             */
            ERROR_CODE endAllocateChannel(const UInt32 channel_id, iviLink::Ipc::DirectionID& dirId);

            /**
             * Called in case of failed channel allocation.
             * Can be called because of error received from other side or because
             * of error on our side.
             * Erases channel data from mRequestedMap and mL1ChannelRegistry. Deallocates
             * the channel using CChannelAllocator.
             * 
             * @param channel_id number of requested channel
             * @param[out] dirId returns directionId of client which requested 
             *    the channel.
             *
             * @retval ERR_OK
             *
             * @todo see message in processServiceAllocateResponse(). PIlin, 31.08.12
             */
            ERROR_CODE failAllocateChannel(const UInt32 channel_id, iviLink::Ipc::DirectionID& dirId);


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
            ERROR_CODE sendRequest(CDataAccessor & accessor);

            /**
             * Service Allocate request processing function
             * @param accessor refernce to data accessor contained request related data
             */
            void processServiceAllocateRequest(CDataAccessor & accessor);

            /**
             * Service Deallocate request processing function
             * @param accessor reference to data accessor contained request related data
             */
            void processServiceDeallocateRequest(CDataAccessor & accessor);

            /**
             * Service Allocate response processing function
             * @param accessor reference to data accessor contained request related data
             */
            void processServiceAllocateResponse(CDataAccessor & accessor);

            /**
             * Service Deallocate response processing function
             * @param accessor reference to data accessor contained request related data
             */
            void processServiceDeallocateResponse(CDataAccessor & accessor);

            /**
             * CLient Allocate request processing function
             * @param accessor reference to data accessor contained request related data
             */
            bool processClientAllocateRequest(CDataAccessor & accessor, const iviLink::Ipc::DirectionID dirId);

            /**
             * Client Deallocate request processing function
             * @param accessor reference to data accessor contained request related data
             */
            bool processClientDeallocateRequest(CDataAccessor & accessor, const iviLink::Ipc::DirectionID dirId);

            /**
             * Client Send request processing function
             * @param accessor reference to data accessor contained request related data
             */
            bool processClientSendRequest(CDataAccessor & accessor, const iviLink::Ipc::DirectionID dirId);


            bool processClientGetConnectionAddrRequest(CDataAccessor & accessor, const iviLink::Ipc::DirectionID dirId);

            /**
             * Fills callback maps for service and clients requests processing.
             * Should be called at startup
             */
            void fillCallbacks();

            void sendIpcNotification(CDataAccessor & accessor, iviLink::Ipc::DirectionID dirId);

            /**
             * Constructor is private to provide sigleton behavior
             */
            L1InterfaceStub();

            /**
             * Destructor
             */
            ~L1InterfaceStub();

            /**
             * Copying constructor
             *
             * Unimplemented to avoid missusing
             *
             * @param classRef The reference to the object the class is constructing on
             */
            L1InterfaceStub(const L1InterfaceStub&);

            /**
             * Assigning operator
             *
             * Unimplemented to avoid missusing
             *
             * @param classRef The reference to the object the class is assiging from
             */

            L1InterfaceStub& operator=(const L1InterfaceStub&);

            // Members section

            //> Map of callbacks that serves requests from client applications
            tClientsCallbacksMap                               mClientsCallbacksMap;

            //> Map of callbacks that serves requests from other side Connectivity agent
            tServiceCallbacksMap                               mServiceCallbacksMap;

            //> Map of allocated channels
            tChannelsRegistryMap                               mL1ChannelRegistry;

            /**
             * Mutex for map of allocated channels.
             * Be sure to preserve the order of mRequestedMapMutex and mRegistryMutex
             * locking, if they are both used. mRequestedMapMutex must be the first one.
             */
            CMutex                                             mRegistryMutex;

            //> Map of requested but not yet allocated channels
            tRequestedChannelsMap                              mRequestedMap;

            /**
             * Mutex for map of requested channels.
             * Be sure to preserve the order of mRequestedMapMutex and mRegistryMutex
             * locking, if they are both used. mRequestedMapMutex must be the first one.
             */
            CMutex                                             mRequestedMapMutex;

            //> server ipc for connection with Proxies on clients sides
            iviLink::Ipc::CIpc*                                   mpIpc;

            //> pointer to CConnectivityAgent class that starts up HAL infrastructure
            iviLink::ConnectivityAgent::HAL::CConnectivityAgent*  mpAgent;


            //> type for mClientDirections
            typedef std::vector<iviLink::Ipc::DirectionID>        tClientDirections;
            //> ipc direction id for data transfer with Proxies on clients sides
            std::vector<iviLink::Ipc::DirectionID>                mClientDirections;

            //> singleton mutex in case of concurrent calls
            static CMutex                                      mSingletonMutex;

            //> singleton instance
            static L1InterfaceStub*                            mSelf;
            
            CSystemConrollerProtocol                           mSCProtocol;
            bool                                               bShutDownFlag;

            class MsgIdGen 
            {
               iviLink::Ipc::MsgID    mRequestId;
               CMutex              mRequestIdMutex;
            public:
               MsgIdGen() :
                  mRequestId(1)
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
            
            MsgIdGen                                           mMsgIdGen;
         };

      }
   }
}
#endif
