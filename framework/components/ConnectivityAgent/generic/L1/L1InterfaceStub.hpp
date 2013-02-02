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


#ifndef L1INTERFACE_STUB_HPP
#define L1INTERFACE_STUB_HPP

#include <map>
#include <vector>

#include "ICallbackHandler.hpp"
#include "Types.hpp"
#include "ITimeoutSubscriber.hpp"
#include "CMutex.hpp"
#include "API.hpp"
#include "CSystemControllerProtocol.hpp"
#include "ConnectivityAgentError.hpp"

namespace iviLink
{
namespace Utils
{
class TimeoutManager;
}
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
* The L1InterfaceStub implemented as Singleton. It's task is to manage the channels allocation
* and deallocationand data flow requests from Client application. The only exclusion is
* creating of service channel which is initiated by HAL when physical connection is established.
* It uses CIpc class to connect to client application and uses DataAccessors to pack data flow
*/

class  L1InterfaceStub
    : public iviLink::Ipc::ICallbackHandler
{
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
        UInt64                      mUpperThresholdTime;
        UInt64                      mLowerThresholdTime;
        tChannelState               mState;
        TChannelPriority            mType;
        CSourceAgent*               mpSourceAgent;
        CTargetAgent*               mpTargetAgent;
        iviLink::Ipc::DirectionID   mClientDir;
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
    typedef bool (L1InterfaceStub::* process_client_func)(CDataAccessor&,
            iviLink::Ipc::DirectionID);

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


    /**
     * Function called by Target agents to receive data from specified channel
     * @param buf data buffer
     * @param channel_id channel number data received from
     * @return IVILINK_NO_ERROR in case of success
     * ERROR_INCONSISTENT_DATA if the buffer is malformed
     * ERROR_REQUEST_FAILED could not reach the client via IPC
     */
    ConnectivityAgentError receiveData(Buffer & buffer, UInt32 channel_id);

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
     * Class RequestTimeout is used to invoke action if the request timeout passed
     */

    class RequestTimeout
        : public Utils::ITimeoutSubscriber
    {
    public:

        /**
         * Constructor
         * @param req type of request to choose the timeout action
         * @param channel_id  channel number which request belongs to
         */
        RequestTimeout(tOpCode req, UInt32 channel_id,L1InterfaceStub* pOwner, bool enabled = true);

        //from ITimeoutSubscriber

        /**
         * Callback for TimeoutManager which invokes when timeout passed
         */
        virtual void onTimeout();

    private:

        /**
        * Not implemented to avoid missusing
        */
        RequestTimeout(const RequestTimeout&);
        RequestTimeout & operator = (const RequestTimeout&);

        tOpCode                      mRequest;
        UInt32                       mChannelID;
        L1InterfaceStub*             mpOwner;
        bool                         mEnabled;
    };

    friend class RequestTimeout;


    class MsgIdGen
    {
    public:
        MsgIdGen()
            : mRequestId(1)
        {
        }

        iviLink::Ipc::MsgID next()
        {
            mRequestIdMutex.lock();
            iviLink::Ipc::MsgID id = mRequestId;
            mRequestId += 2;
            mRequestIdMutex.unlock();
            return id;
        }

    private:
        iviLink::Ipc::MsgID     mRequestId;
        CMutex                  mRequestIdMutex;
    };

    // Methods section
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
     * @retval IVILINK_NO_ERROR all ok, see diagram SEQ_A and SEQ_B
     * @retval ERROR_DEFERRED all ok, need to wait channel allocation
     *    request from one of clients on this side. See diagram SEQ_A
     * @retval ERROR_CHANNEL_BUSY there is channel with specified
     *    @c channel_id in the mL1ChannelRegistry
     * @reval ERROR_ALLOCATION_IN_PROGRESS attempt to make the second request for the
     *    same channel, while the first request is not ended yet.
     *    Please, be patient.
     * @return there can be other errors from allocateChannelLocally()
     */
    ConnectivityAgentError beginAllocateChannel(const TChannelPriority prio,
            const UInt32 channel_id, const bool client_side, iviLink::Ipc::DirectionID& dirId);

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
     * @return IVILINK_NO_ERROR in case of success
     * ERROR_CHANNEL_BUSY if a channel with the same ID has been allocated already
     */
    ConnectivityAgentError allocateChannelLocally(const UInt32 channel_id,
            tRequestedChannelInfo const& req_info);

    /**
     * This method finishes channel allocation.
     * Updates mL1ChannelRegistry.
     *
     * @param channel_id number of requested channel
     * @param[out] dirId returns directionId of client which requested
     *    the channel.
     *
     * @retval IVILINK_NO_ERROR all ok, channel successfully allocated
     * @retval ERROR_DEFERRED all ok, channel allocated, but need to send
     *    message to the other side
     * @retval ERROR_NOT_FOUND channel is not found in mL1ChannelRegistry. This is
     *    legit in case when L1interfaceStub is being destroyed. Or it
     *    can be some bug.
     */
    ConnectivityAgentError endAllocateChannel(const UInt32 channel_id,
            iviLink::Ipc::DirectionID& dirId);

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
     * @retval IVILINK_NO_ERROR
     *
     * @todo see message in processServiceAllocateResponse(). PIlin, 31.08.12
     */
    ConnectivityAgentError failAllocateChannel(const UInt32 channel_id,
            iviLink::Ipc::DirectionID& dirId);


    /**
     * Deallocates duplex channel with given number and sends the allocation request to
     * the other side Connectivity Agent process
     * @param channel_id channel number
     * @return IVILINK_NO_ERROR in case of success, ERROR_NOT_FOUND if no channel with this ID was found
     */
    ConnectivityAgentError deallocateChannel(UInt32 channel_id);

    /**
     * Sends data received from Client App through specified channel
     * @param Accessor client's data accessor contained channel number and data itself
     * @return IVILINK_NO_ERROR in case of success, ERROR_NOT_FOUND if no channel with this ID was found
     * ERROR_NO_FREE_BUFFERS_LEFT if there are no free buffers to enqueue data
     * contained in this acessor
     */
    ConnectivityAgentError sendData(CDataAccessor & Accessor);

    /**
     * Sends service requests for channel allocation/deallocation to the other side
     * Connectivity Agent process
     * @param Accessor data accessor contained request
     * @return IVILINK_NO_ERROR in case of success
     * ERROR_NO_FREE_BUFFERS_LEFT if there are no free buffers to enqueue data contained
     * in this acessor
     */
    ConnectivityAgentError sendRequest(CDataAccessor & accessor);

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
     * @return true if the accessor should be sent back to client
     */
    bool processClientAllocateRequest(CDataAccessor & accessor,
            const iviLink::Ipc::DirectionID dirId);


    bool processClientDeallocateByWD(CDataAccessor & accessor,
            const iviLink::Ipc::DirectionID dirId);

    /**
     * Client Deallocate request processing function
     * @param accessor reference to data accessor contained request related data
     * @return true if the accessor should be sent back to client
     */
    bool processClientDeallocateRequest(CDataAccessor & accessor,
            const iviLink::Ipc::DirectionID dirId);

    /**
     * Client Send request processing function
     * @param accessor reference to data accessor contained request related data
     * @return true if the accessor should be sent back to client
     */
    bool processClientSendRequest(CDataAccessor & accessor, const iviLink::Ipc::DirectionID dirId);

    bool processClientGetConnectionAddrRequest(CDataAccessor & accessor,
            const iviLink::Ipc::DirectionID dirId);

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
     * Unimplemented to avoid missusing
     * @param classRef The reference to the object the class is constructing on
     */
    L1InterfaceStub(const L1InterfaceStub&);

    /**
     * Assigning operator
     * Unimplemented to avoid missusing
     * @param classRef The reference to the object the class is assiging from
     */
    L1InterfaceStub& operator=(const L1InterfaceStub&);

    // Members section

    //> Map of callbacks that serves requests from client applications
    tClientsCallbacksMap                                    mClientsCallbacksMap;

    //> Map of callbacks that serves requests from other side Connectivity agent
    tServiceCallbacksMap                                    mServiceCallbacksMap;

    //> Map of allocated channels
    tChannelsRegistryMap                                    mL1ChannelRegistry;

    /**
    * Mutex for map of allocated channels.
    * Be sure to preserve the order of mRequestedMapMutex and mRegistryMutex
    * locking, if they are both used. mRequestedMapMutex must be the first one.
    */
    CMutex                                                  mRegistryMutex;

    //> Map of requested but not yet allocated channels
    tRequestedChannelsMap                                   mRequestedMap;

    /**
    * Mutex for map of requested channels.
    * Be sure to preserve the order of mRequestedMapMutex and mRegistryMutex
    * locking, if they are both used. mRequestedMapMutex must be the first one.
    */
    CMutex                                                  mRequestedMapMutex;

    //> server ipc for connection with Proxies on clients sides
    iviLink::Ipc::CIpc*                                     mpIpc;

    //> pointer to CConnectivityAgent class that starts up HAL infrastructure
    iviLink::ConnectivityAgent::HAL::CConnectivityAgent*    mpAgent;


    //> type for mClientDirections
    typedef std::vector<iviLink::Ipc::DirectionID>          tClientDirections;
    //> ipc direction id for data transfer with Proxies on clients sides
    std::vector<iviLink::Ipc::DirectionID>                  mClientDirections;

    //> singleton mutex in case of concurrent calls
    static CMutex                                           mSingletonMutex;

    //> singleton instance
    static L1InterfaceStub*                                 mSelf;

    CSystemConrollerProtocol                                mSCProtocol;
    bool                                                    bShutDownFlag;
    MsgIdGen                                                mMsgIdGen;
    Utils::TimeoutManager *                                 mTimeoutManager;
    static Logger logger;
};

}   // namespace L1
}   // namespace ConnectivityAgent
}   // namespace iviLink
#endif
