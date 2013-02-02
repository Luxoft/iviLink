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


#include <utility>
#include <algorithm>
#include <unistd.h>

#include "CIpc.hpp"
#include "CSourceAgent.hpp"
#include "CTargetAgent.hpp"
#include "ConnectivityAgentBuffer.hpp"
#include "CDataAccessor.hpp"
#include "CConnectivityAgent.hpp"

#include "L1InterfaceStub.hpp"

#include "CChannelAllocator.hpp"
#include "Logger.hpp"
#include "byteOrder.hpp"
#include "TimeoutManager.hpp"
#include "CCarrierAdapter.hpp"
#include "UnstableAPI.hpp"


using namespace iviLink::ConnectivityAgent::L1;
using namespace iviLink::ConnectivityAgent;

using iviLink::ConnectivityAgent::CDataAccessor;
using iviLink::ConnectivityAgent::HAL::CConnectivityAgent;

CMutex L1InterfaceStub::mSingletonMutex;
L1InterfaceStub* L1InterfaceStub::mSelf = 0;

static const char* systemControllerSocket = "SysCtr_ConAgnt";

Logger L1InterfaceStub::logger = Logger::getInstance(LOG4CPLUS_TEXT
        ("ConnectivityAgent.L1.L1InterfaceStub"));

L1InterfaceStub::L1InterfaceStub()
    : mpAgent(NULL)
    , mSCProtocol(this,systemControllerSocket )
    , bShutDownFlag(false)
    , mTimeoutManager(new Utils::TimeoutManager())
{
    fillCallbacks();
    BaseError err = mSCProtocol.connect();
    LOG4CPLUS_INFO(logger, "mSCProtocol.connect() returned: " + static_cast<std::string>(err));
}

L1InterfaceStub::~L1InterfaceStub()
{
    if (mTimeoutManager)
    {
        mTimeoutManager->finish();
        delete mTimeoutManager;
    }
    mClientDirections.clear();
    delete mpIpc;
    delete mpAgent;
}

void L1InterfaceStub::start(bool isServer, const char* sockPath)
{
    mpAgent = new CConnectivityAgent(isServer);
    mpAgent->start();
    mpIpc = new iviLink::Ipc::CIpc(sockPath, *this);
    BaseError err = mpIpc->beginWaitForConnection();
    assert(err.isNoError());
}

L1InterfaceStub* L1InterfaceStub::getInstance()
{
    mSingletonMutex.lock();
    if (mSelf == NULL)
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

ConnectivityAgentError L1InterfaceStub::beginAllocateChannel(const TChannelPriority prio,
        const UInt32 channel_id, const bool client_side, DirectionID& dirId)
{
    LOG4CPLUS_TRACE_METHOD(logger, "L1InterfaceStub::beginAllocateChannel(" +
            convertIntegerToString(channel_id) + ", "
            "client_side" + std::string(client_side ? "true" : "false") +")");
    ConnectivityAgentError result;

    //if channel wasn't opened yet
    if (mL1ChannelRegistry.find(channel_id) == mL1ChannelRegistry.end())
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
                    " mState " + std::string(info.mState == E_TRANSITION_CLIENT ?
                            "E_TRANSITION_CLIENT" : "E_TRANSITION_AGENT"));

            mRequestedMap.insert(std::pair<UInt32, tRequestedChannelInfo>(channel_id, info));
            
            result = (client_side ? ConnectivityAgentError::NoError() :
                    ConnectivityAgentError(ConnectivityAgentError::ERROR_DEFERRED));
        }
        else
        {
            tRequestedChannelInfo & info = iter->second;

            //if channel was requested by other side
            if (!client_side || (client_side && info.mState == E_TRANSITION_AGENT))
            {
                // SEQ_A DEFERRED or call from other side

                LOG4CPLUS_INFO(logger, "L1InterfaceStub::beginAllocateChannel() =>"
                        "Opening channel locally");

                if (client_side)
                {
                    info.mClientDir = dirId;
                }
                else
                {
                    dirId = info.mClientDir;
                }

                result = allocateChannelLocally(channel_id, info);

                assert(result.getCode() != ConnectivityAgentError::ERROR_DEFERRED);

                mRequestedMap.erase(channel_id);
            }
            else  //channel was requested by Client Application not the first time
            {
                // SEQ_E_1
                LOG4CPLUS_ERROR(logger, "L1InterfaceStub::beginAllocateChannel() => ERROR: "
                        "allocation is in progress! ");
                result.setErrorCode(ConnectivityAgentError::ERROR_ALLOCATION_IN_PROGRESS);
            }
        }
    }
    else //channel already opened
    {
        // SEQ_E_2

        LOG4CPLUS_ERROR(logger, "L1InterfaceStub::beginAllocateChannel() => ERROR: channel "
                "already exists! ");
        result.setErrorCode(ConnectivityAgentError::ERROR_CHANNEL_BUSY);
    }

    return result;
}

ConnectivityAgentError L1InterfaceStub::allocateChannelLocally(const UInt32 channel_id,
        tRequestedChannelInfo const& req_info)
{
    LOG4CPLUS_TRACE_METHOD(logger, "L1InterfaceStub::allocateChannelLocally"
            " channel_id" + convertIntegerToString(channel_id));

    ConnectivityAgentError result;

    assert(mL1ChannelRegistry.find(channel_id) == mL1ChannelRegistry.end());

    assert(req_info.mState == E_TRANSITION_AGENT || req_info.mState == E_TRANSITION_CLIENT);

    ///>FIXME Set up correct thresholds
    tL1ChannelInfo info;
    info.mType                 = req_info.mType;
    info.mClientDir            = req_info.mClientDir;
    info.mState                = req_info.mState;
    info.mLowerThresholdTime   = 0;
    info.mUpperThresholdTime   = 0xFFFFFFFF;

    result = CChannelAllocator::getInstance()->allocateChannel(info.mType, channel_id, info);

    LOG4CPLUS_INFO(logger, "L1InterfaceStub::allocateChannelLocally():  CChannelAllocator ret = "
            + convertIntegerToString((int)result.getCode()));

    /// @todo correct processing of error code and return values. PIlin, 31.08.12
    //ret = ERR_OK;

    if (result.isNoError())
    {
        mL1ChannelRegistry.insert(std::pair<UInt32,tL1ChannelInfo >(channel_id, info));
    }

    return result;
}

ConnectivityAgentError L1InterfaceStub::endAllocateChannel(const UInt32 channel_id,
        DirectionID& dirId)
{
    ConnectivityAgentError result = ConnectivityAgentError::NoError();
    mRegistryMutex.lock();
    {
        tChannelsRegistryMap::iterator iter = mL1ChannelRegistry.find(channel_id);
        if (iter != mL1ChannelRegistry.end())
        {
            tL1ChannelInfo & info = iter->second;

            if (E_TRANSITION_AGENT == info.mState)
            {
                result.setErrorCode(ConnectivityAgentError::ERROR_DEFERRED);
            }

            LOG4CPLUS_INFO(logger, "L1InterfaceStub::endAllocateChannel(): E_OPERATIONAL");
            info.mState = E_OPERATIONAL;

            dirId = info.mClientDir;
        }
        else
        {
            // SEQ_E_4
            result.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
            LOG4CPLUS_ERROR(logger, "L1InterfaceStub::endAllocateChannel(): ERR_NOTFOUND");
        }
    }
    mRegistryMutex.unlock();

    return result;
}

ConnectivityAgentError L1InterfaceStub::failAllocateChannel(const UInt32 channel_id,
        DirectionID& dirId)
{
    mRequestedMapMutex.lock();
    mRegistryMutex.lock();
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
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::failAllocateChannel(): "
                    "remove registry entry and deallocate channel");

            CChannelAllocator::getInstance()->deallocateChannel(channel_id);
            tL1ChannelInfo & info = iter->second;
            dirId = info.mClientDir;

            mL1ChannelRegistry.erase(iter);
        }
    }
    mRegistryMutex.unlock();
    mRequestedMapMutex.unlock();
    return ConnectivityAgentError::NoError();
}

ConnectivityAgentError L1InterfaceStub::deallocateChannel(UInt32 channel_id)
{
    LOG4CPLUS_TRACE(logger, "L1InterfaceStub::deallocateChannel(chID = " +
            convertIntegerToString(channel_id) + ")");
    ConnectivityAgentError result(ConnectivityAgentError::ERROR_NOT_FOUND);
    mRegistryMutex.lock();

    if (mL1ChannelRegistry.find(channel_id) != mL1ChannelRegistry.end())
    {
        result = CChannelAllocator::getInstance()->deallocateChannel(channel_id);
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "L1InterfaceStub::deallocateChannel(chID = "
                + convertIntegerToString(channel_id) + ") => ERROR: Channel not found! ");
    }
    mRegistryMutex.unlock();

    return result;
}


ConnectivityAgentError L1InterfaceStub::sendData(CDataAccessor & accessor)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    ConnectivityAgentError result(ConnectivityAgentError::ERROR_NOT_FOUND);
    mRegistryMutex.lock();
    tChannelsRegistryMap::iterator iter = mL1ChannelRegistry.find(accessor.getChannelID());

    if (iter != mL1ChannelRegistry.end())
    {
        Buffer buf;
        UInt32 size = accessor.getDataSize();
        assert (size <= MAX_SIZE);

        LOG4CPLUS_INFO(logger, "L1InterfaceStub::sendData() => Overall data bytes " +
                convertIntegerToString(size));
        buf.reserveSize(size);
        buf.setFilledSize(size);
        memcpy(buf.getData(), accessor.getData(), accessor.getDataSize());
        result = (iter->second.mpSourceAgent->fillBuffer(buf));
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "L1InterfaceStub::sendData() => channel not found!");
        result.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
    }
    mRegistryMutex.unlock();
    return result;
}

ConnectivityAgentError L1InterfaceStub::receiveData(Buffer & buffer, UInt32 channel_id)
{
    LOG4CPLUS_TRACE(logger, "L1InterfaceStub::receiveData() => channel " +
            convertIntegerToString(channel_id));

    UInt32 offset = 0;
    CDataAccessor accessor;
    ConnectivityAgentError result;
    // TODO ipc errors processing
    // TODO incorrect data processing

    assert(buffer.getFilledSize() <= buffer.getMaxSize());

    LOG4CPLUS_INFO(logger, "L1InterfaceStub::receiveData() =>  buf size "
    + convertIntegerToString(buffer.getFilledSize()));
    if(channel_id == CA_SERVICE_CHANNEL)
    {
        //> If  data received from service channel - process service request
        do
        {

            UInt32 begin = offset;

            UInt16 buf_length = ByteOrder::ntoh16(*reinterpret_cast<UInt16*>
            (buffer.getData() + offset));
            offset += 2;
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::receiveData() =>  buf_length " +
                    convertIntegerToString(buf_length));
            assert(begin + buf_length <= buffer.getFilledSize());
            
            UInt32 operation = ByteOrder::ntoh32(*reinterpret_cast<UInt32*>
                    (buffer.getData() + offset));
            offset += 4;
            accessor.setOpCode(operation);
            
            UInt32 channel_id = ByteOrder::ntoh32(*reinterpret_cast<UInt32*>
                    (buffer.getData() + offset));
            offset += 4;
            accessor.setChannelID(channel_id);
            
            UInt32 data_size = ByteOrder::ntoh32(*reinterpret_cast<UInt32*>
                    (buffer.getData() + offset));
            offset += 4;
            
            UInt32 error_code = ByteOrder::ntoh32(*reinterpret_cast<UInt32*>
                    (buffer.getData() + offset));
            offset += 4;
            accessor.setErrorCode(error_code);

            accessor.setData(buffer.getData() + offset, data_size);
            offset += data_size;
            
            accessor.printContent();
            ///>Process service request
            tServiceCallbacksMap::iterator iter =
                    mServiceCallbacksMap.find(static_cast<tOpCode>(operation));

            if (iter !=mServiceCallbacksMap.end())
            {
                ///> This funny language structure is just callback invocation.
                ///> Not obvious, huh? =)
                (this->*iter->second)(accessor);
                result.setNoError();
            }
            else
            {
                LOG4CPLUS_WARN(logger, "L1InterfaceStub::receiveData() => "
                        "UNKNOWN SERVICE REQUEST = "
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
        accessor.setOpCode(E_RECEIVE_DATA_NTF);
        accessor.setChannelID(channel_id);
        assert (buffer.getFilledSize() > 0);
        accessor.setData(buffer.getData(),buffer.getFilledSize());
        accessor.printContent();
        UInt8* buf = new UInt8[accessor.getObjectSize()];
        accessor.copyToRawArray(buf);

        BaseError ipcError = mpIpc->asyncRequest(mMsgIdGen.next(),
        buf, accessor.getObjectSize(), &mL1ChannelRegistry[channel_id].mClientDir); // TODO use find, and not []

        delete [] buf;
        if (ipcError.isNoError())
        {
            result.setNoError();
        }
        else
        {
            result.setErrorCode(ConnectivityAgentError::ERROR_REQUEST_FAILED);
        }
    }

    return result;
}

ConnectivityAgentError L1InterfaceStub::sendRequest( CDataAccessor & accessor)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    Buffer buf;
    UInt32 offset = 0;

    accessor.printContent();

    buf.getFilledSize() = accessor.getObjectSize() + sizeof(UInt16);
    buf.reserveSize(buf.getFilledSize());

    *reinterpret_cast<UInt16*> (buf.getData() + offset) =
            ByteOrder::hton16((UInt16)buf.getFilledSize());
    offset += 2;

    *reinterpret_cast<UInt32*> (buf.getData() + offset) =
            ByteOrder::hton32(accessor.getOpCode());
    offset += 4;

    *reinterpret_cast<UInt32*> (buf.getData() + offset) =
            ByteOrder::hton32(accessor.getChannelID());
    offset += 4;

    *reinterpret_cast<UInt32*> (buf.getData() + offset) =
            ByteOrder::hton32(accessor.getDataSize());
    offset += 4;

    *reinterpret_cast<UInt32*> (buf.getData() + offset) =
            ByteOrder::hton32(accessor.getErrorCode());
    offset += 4;

    if (accessor.getDataSize())
    {
        memcpy(buf.getData() + offset,accessor.getData(), accessor.getDataSize() );
    }

    ConnectivityAgentError ret = mL1ChannelRegistry[CA_SERVICE_CHANNEL].
            mpSourceAgent->fillBuffer(buf);

    buf.forgetData();

    return ret;
}

void L1InterfaceStub::OnConnected()
{
    LOG4CPLUS_INFO(logger, "L1InterfaceStub::OnConnected(): "
            "Connectivity Agents connected, create service channel ....");

    tRequestedChannelInfo info;
    info.mType = eSafetyCritical;
    info.mClientDir = -1;
    info.mState = E_TRANSITION_CLIENT;

    mRegistryMutex.lock();
    allocateChannelLocally(CA_SERVICE_CHANNEL, info);
    mRegistryMutex.unlock();

    mSCProtocol.sendConnectionEstablished(mpAgent->getCurrentGender());

}

void L1InterfaceStub::OnDisconnected()
{
    LOG4CPLUS_INFO(logger, "L1InterfaceStub::OnDisconnected(): "
            "Connectivity Agents disconnected=>sending notfications to all clients");

    mSCProtocol.sendConnectionLost();

    CDataAccessor responceDA;
    responceDA.setOpCode(E_CONNECTION_LOST_NTF);
    for (tClientDirections::iterator iter = mClientDirections.begin(); iter !=
            mClientDirections.end(); ++iter)
    {
        UInt8* buf = new UInt8[responceDA.getObjectSize()];
        responceDA.copyToRawArray(buf);
        mpIpc->asyncRequest(mMsgIdGen.next(),
        buf, responceDA.getObjectSize(), &(*iter));
        delete [] buf;
    }
}

void L1InterfaceStub::fillCallbacks()
{

    mClientsCallbacksMap.insert(std::make_pair(E_ALLOCATE_CHANNEL,    &L1InterfaceStub::processClientAllocateRequest         ));
    mClientsCallbacksMap.insert(std::make_pair(E_DEALLOCATE_CHANNEL,  &L1InterfaceStub::processClientDeallocateRequest       ));
    mClientsCallbacksMap.insert(std::make_pair(E_SEND_DATA,           &L1InterfaceStub::processClientSendRequest             ));
    mClientsCallbacksMap.insert(std::make_pair(E_GET_CONNECTION_ADDR, &L1InterfaceStub::processClientGetConnectionAddrRequest));
    mClientsCallbacksMap.insert(std::make_pair(E_DEALLOCATE_CHANNEL_WATCHDOG, &L1InterfaceStub::processClientDeallocateByWD  ));

    mServiceCallbacksMap.insert(std::make_pair(E_ALLOCATE_CHANNEL,        &L1InterfaceStub::processServiceAllocateRequest    ));
    mServiceCallbacksMap.insert(std::make_pair(E_DEALLOCATE_CHANNEL,      &L1InterfaceStub::processServiceDeallocateRequest  ));
    mServiceCallbacksMap.insert(std::make_pair(E_ALLOCATE_CHANNEL_RESP,   &L1InterfaceStub::processServiceAllocateResponse   ));
    mServiceCallbacksMap.insert(std::make_pair(E_DEALLOCATE_CHANNEL_RESP, &L1InterfaceStub::processServiceDeallocateResponse ));

}

void L1InterfaceStub::OnRequest(iviLink::Ipc::MsgID id, 
        UInt8 const* pPayload, UInt32 payloadSize,
        UInt8* const pResponseBuffer, UInt32& bufferSize,
        DirectionID dirId)
{
    LOG4CPLUS_INFO(logger, "L1InterfaceStub::OnRequest(): received " +
            convertIntegerToString(payloadSize) + " bytes");

    const UInt32 savedBufferSize = bufferSize;
    // no response by default
    bufferSize = 0;

    CDataAccessor pReq(pPayload, payloadSize);
    pReq.printContent();

    bool send_result = false;

    tClientsCallbacksMap::const_iterator callback_iter =
            mClientsCallbacksMap.find(static_cast<tOpCode>(pReq.getOpCode()));
    if (callback_iter != mClientsCallbacksMap.end())
    {
        LOG4CPLUS_INFO(logger, "OnRequest: found callback, invoking...");
        send_result = (this->*callback_iter->second)(pReq, dirId);
    }
    else
    {
        LOG4CPLUS_WARN(logger, "L1InterfaceStub::OnRequest() => UNKNOWN REQUEST!");
        pReq.resetData();
        pReq.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
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

bool L1InterfaceStub::processClientAllocateRequest(CDataAccessor & accessor,
        const iviLink::Ipc::DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    const UInt32 channel_id = accessor.getChannelID();

    if(CA_SERVICE_CHANNEL == channel_id)
    {
        // SEQ_E_5
        LOG4CPLUS_WARN(logger, "Channel CA_SERVICE_CHANNEL is not allowed to be open");
        accessor.resetData();
        accessor.setErrorCode(ConnectivityAgentError::ERROR_CHANNEL_BUSY);
        return true;
    }

    UInt32 prio = 0;
    memcpy(&prio, accessor.getData(), accessor.getDataSize());
    LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientAllocateRequest() => "
            "Allocate Channel Request: ChID = "
            + convertIntegerToString(channel_id) + ", prio = " + convertIntegerToString(prio));

    iviLink::Ipc::DirectionID tmpDirId = dirId;
    ConnectivityAgentError err = ConnectivityAgentError(ConnectivityAgentError::ERROR_OTHER);

    mRequestedMapMutex.lock();
    mRegistryMutex.lock();
    {
        err = beginAllocateChannel(static_cast<TChannelPriority>(prio), channel_id, true,
                tmpDirId);
        accessor.setErrorCode(err.getCode());

        if (err.isNoError())
        {
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientAllocateRequest() => "
                    "all ok, sending request to other side" );
            err = sendRequest(accessor);
        }
    }
    mRegistryMutex.unlock();
    mRequestedMapMutex.unlock();

    if (err.isNoError()) //> all ok
    {
        return false;
    }
    else
    {
        // something wrong, need message about error
        // SEQ_E_5
        assert(err.getCode() != ConnectivityAgentError::ERROR_DEFERRED);
        accessor.resetData();
        accessor.setErrorCode(err.getCode());
        return true;
    }
}

bool L1InterfaceStub::processClientDeallocateRequest(CDataAccessor & accessor,
        const iviLink::Ipc::DirectionID dirId)
{
    UInt32 channel_id = accessor.getChannelID();
    if(CA_SERVICE_CHANNEL !=channel_id)
    {
        LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientDeallocateRequest() => "
                "Deallocate Channel Request: ChID = " + convertIntegerToString(channel_id));
        mRegistryMutex.lock();
        tChannelsRegistryMap::iterator iter =  mL1ChannelRegistry.find(channel_id);
        if (iter != mL1ChannelRegistry.end())
        {
            iter->second.mState = E_TRANSITION_CLIENT;
            sendRequest(accessor);
            assert(mTimeoutManager);
            mTimeoutManager->addSubscriber(new RequestTimeout(static_cast<tOpCode>
            (accessor.getOpCode()),channel_id,this ), 250000);
            // ret = ERR_OK;
        }
        else
        {
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientDeallocateRequest() "
                    "=> Deallocate Channel Request: ChID = " +
                    convertIntegerToString(channel_id) + " NOT FOUND!");
            // ret = ERR_NOTFOUND;
        }
        mRegistryMutex.unlock();
    }

    return false;
}


bool L1InterfaceStub::processClientDeallocateByWD(CDataAccessor & accessor,
        const iviLink::Ipc::DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    UInt32 channel_id = accessor.getChannelID();
    if(CA_SERVICE_CHANNEL !=channel_id)
    {
        LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientDeallocateByWD() => "
                "Deallocate Channel Request: ChID = " + convertIntegerToString(channel_id));
        mRegistryMutex.lock();
        tChannelsRegistryMap::iterator iter =  mL1ChannelRegistry.find(channel_id);
        if (iter != mL1ChannelRegistry.end())
        {
            iter->second.mClientDir = dirId; // so that Negotiator receives the response
            iter->second.mState = E_TRANSITION_CLIENT;
            accessor.setOpCode(E_DEALLOCATE_CHANNEL);
            sendRequest(accessor);
            assert(mTimeoutManager);
            mTimeoutManager->addSubscriber(new RequestTimeout(static_cast<tOpCode>
            (accessor.getOpCode()),channel_id,this ), 250000);
            accessor.setErrorCode(0);
        }
        else
        {
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientDeallocateByWD() "
                    "=> Deallocate Channel Request: ChID = " +
                    convertIntegerToString(channel_id) + " NOT FOUND!");
            accessor.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
        }
        mRegistryMutex.unlock();
    }
    else
    {
        accessor.setErrorCode(ConnectivityAgentError::ERROR_OTHER);
    }

    return true;
}

bool L1InterfaceStub::processClientSendRequest(CDataAccessor & accessor,
        const iviLink::Ipc::DirectionID dirId)
{
    UInt32 channel_id = accessor.getChannelID();
    if (CA_SERVICE_CHANNEL !=channel_id)
    {
        LOG4CPLUS_INFO(logger, "L1InterfaceStub::processClientSendRequest() => "
                "Send Data Request: ChID = " + convertIntegerToString(channel_id));
        sendData(accessor);
    }
    return false;
}

bool L1InterfaceStub::processClientGetConnectionAddrRequest(CDataAccessor & accessor,
        const iviLink::Ipc::DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    ConnectionInformation fake;
    UInt8 * serializedData = fake.serialize();
    accessor.setData(serializedData, fake.getSerializedSize());
    accessor.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
    delete[] serializedData;

    if (mpAgent)
    {
        iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pca =
                mpAgent->getCurrentCarrierAdapter();
        if (pca)
        {
            LOG4CPLUS_INFO(logger, "Retrieving actual connection information");
            ConnectionInformation address(pca->getLocalAddress(), pca->getRemoteAddress(),
                    pca->getTypeName());
            UInt8 * serializedData = address.serialize();
            accessor.setData(serializedData, address.getSerializedSize());
            accessor.setErrorCode(BaseError::IVILINK_NO_ERROR);
            delete[] serializedData;
        }
    }
    // we try to send response always
    return true;
}

void L1InterfaceStub::processServiceAllocateRequest(CDataAccessor & accessor)
{
    const UInt32 channel_id = accessor.getChannelID();
    const UInt32 prio = *reinterpret_cast<UInt32*>(accessor.getData());
    ConnectivityAgentError err(static_cast<ConnectivityAgentError::AgentErrorCodes>
            (accessor.getErrorCode()));
    LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateRequest()=>"
            "Allocate channel prio = " + convertIntegerToString(prio) +
            " id =" + convertIntegerToString(accessor.getChannelID()));

    iviLink::Ipc::DirectionID dirId = -1;

    accessor.setOpCode(E_ALLOCATE_CHANNEL_RESP);
    accessor.resetData();

    if (!err.isNoError())
    {
        // SEQ_E_3
        failAllocateChannel(channel_id, dirId);

        accessor.setErrorCode(err.getCode());
    }
    else
    {
        mRequestedMapMutex.lock();
        mRegistryMutex.lock();
        {
            err = beginAllocateChannel(static_cast<TChannelPriority>(prio), channel_id,
                    false, dirId);

            if (err.getCode() != ConnectivityAgentError::ERROR_DEFERRED)
            {
                // sending the result to other side
                // SEQ_E_4 or ok
                accessor.setErrorCode(err.getCode());
                sendRequest(accessor);
            }
        }
        mRegistryMutex.unlock();
        mRequestedMapMutex.unlock();

        if (err.getCode() == ConnectivityAgentError::ERROR_DEFERRED)
        {
            // all ok, we will wait for the client requesting this channel
            // setting timeout

            LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateRequest() => "
                    "channel wasn't requested yet ->set channel to wait and trigger timeout");

            assert(mTimeoutManager);
            mTimeoutManager->addSubscriber(
                    new RequestTimeout(E_ALLOCATE_CHANNEL,channel_id, this), 250000);
            return;
        }
    }

    if (!err.isNoError())
    {
        // messagign about failed channel allocation
        // SEQ_E_4

        if (dirId != -1)
        {
            sendIpcNotification(accessor, dirId);
        }
    }
}

void L1InterfaceStub::processServiceDeallocateRequest(CDataAccessor & accessor)
{
    UInt32 channel_id = accessor.getChannelID();
    LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceDeallocateRequest() => "
            "Channel deallocate request id = " + convertIntegerToString(channel_id));

    ConnectivityAgentError ret = deallocateChannel(channel_id );
    accessor.setOpCode(E_DEALLOCATE_CHANNEL_RESP);
    UInt32 data = ret.getCode();
    accessor.setData(reinterpret_cast<UInt8 *>(&data), sizeof(UInt32));
    sendRequest(accessor);

    if (ret.isNoError())
    {
        LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceDeallocateRequest() => "
                "channel successfully deallocated. "
        "Notifying client");

        iviLink::Ipc::DirectionID dirId;
        bool found = false;

        mRegistryMutex.lock();
        tChannelsRegistryMap::iterator iter = mL1ChannelRegistry.find(accessor.getChannelID());
        if (mL1ChannelRegistry.end() != iter)
        {
            dirId = iter->second.mClientDir;
            found = true;

            LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceDeallocateRequest() => "
                    "channel found in registry - removing");
            mL1ChannelRegistry.erase(accessor.getChannelID());
        }
        mRegistryMutex.unlock();

        if (found)
        {
            accessor.setOpCode(E_DEALLOCATE_CHANNEL_NTF);
            UInt8* buf = new UInt8[accessor.getObjectSize()];
            accessor.copyToRawArray(buf);
            BaseError err = mpIpc->asyncRequest(mMsgIdGen.next(),
            buf, accessor.getObjectSize(), &dirId);

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
    ConnectivityAgentError err(static_cast<ConnectivityAgentError::AgentErrorCodes>
            (accessor.getErrorCode()));

    LOG4CPLUS_TRACE_METHOD(logger, "L1InterfaceStub::processServiceAllocateResponse() => "
            "Channel allocated responce id = "
            + convertIntegerToString(channel_id) + ", err = " +
            convertIntegerToString((int)err.getCode()));

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

    if (err.isNoError())
    {
        err = endAllocateChannel(channel_id, dirId);

        if (err.getCode() == ConnectivityAgentError::ERROR_DEFERRED)
        {
            LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateResponse() => "
                    "was E_TRANSITION_AGENT, messaging to other side");
            accessor.setErrorCode(BaseError::IVILINK_NO_ERROR);
            err = sendRequest(accessor);
        }
    }

    if (!err.isNoError() || dirId == -1)
    {
        // SEQ_E_4

        LOG4CPLUS_ERROR(logger, "L1InterfaceStub::processServiceAllocateResponse() => "
                "failed channel allocation");

        failAllocateChannel(channel_id, dirId);
        accessor.setErrorCode(err.getCode());
    }

    if (dirId != -1)
    {
        LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceAllocateResponse() => "
                "message about allocation result");
        sendIpcNotification(accessor, dirId);
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "L1InterfaceStub::processServiceAllocateResponse() => "
                "unknown client, failing channel");
    }
}

void L1InterfaceStub::processServiceDeallocateResponse(CDataAccessor & accessor)
{
    UInt32 channel_id = accessor.getChannelID();
    UInt32 errCode = 0;
    memcpy(&errCode, accessor.getData(), sizeof(UInt32));

    ConnectivityAgentError ret(static_cast<ConnectivityAgentError::AgentErrorCodes>(errCode));
    LOG4CPLUS_INFO(logger, "L1InterfaceStub::processServiceDeallocateResponse() => "
            "Channel deallocated responce id = "
            + convertIntegerToString(channel_id) + " err = " +
            convertIntegerToString((int)ret.getCode()));
    if (ret.isNoError())
    {
        ret = deallocateChannel(channel_id);
        if (ret.isNoError())
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

                BaseError err = mpIpc->asyncRequest(mMsgIdGen.next(),
                        buf, accessor.getObjectSize(), &dirId);

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


void L1InterfaceStub::sendIpcNotification(CDataAccessor & accessor,
        iviLink::Ipc::DirectionID dirId)
{
    assert(dirId != -1);

    UInt8* buf = new UInt8[accessor.getObjectSize()];
    accessor.copyToRawArray(buf);

    BaseError err = mpIpc->asyncRequest(mMsgIdGen.next(),
            buf, accessor.getObjectSize(), &dirId);

    if (!err.isNoError())
    {
        LOG4CPLUS_WARN(logger, "sendIpcNotification error : " + static_cast<std::string>(err));
    }

    delete [] buf;
}

L1InterfaceStub::RequestTimeout::RequestTimeout(tOpCode req, UInt32 channel_id,
        L1InterfaceStub* pOwner ,bool enabled)
    : mRequest(req), mChannelID(channel_id), mpOwner(pOwner), mEnabled(enabled)
{
}

void L1InterfaceStub::RequestTimeout::onTimeout()
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
            bool found = (mpOwner->mL1ChannelRegistry.find(mChannelID) !=
                    mpOwner->mL1ChannelRegistry.end());
            mpOwner->mRegistryMutex.unlock();

            if (found)
            {
                LOG4CPLUS_INFO(logger, "L1InterfaceStub::RequestTimeout::onTimeout()=> channel "
                        + convertIntegerToString(mChannelID) + " allocated!");
            }
            else
            {
                LOG4CPLUS_WARN(logger, "L1InterfaceStub::RequestTimeout::onTimeout()=> Channel "
                        + convertIntegerToString(mChannelID) + " allocation timeout!");
                resp.setChannelID(mChannelID);
                resp.setOpCode(E_ALLOCATE_CHANNEL_RESP);
                resp.setData(reinterpret_cast<UInt8 *>(&data), sizeof(UInt32));
                resp.setErrorCode(ConnectivityAgentError::ERROR_TIMEOUT);
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
                LOG4CPLUS_INFO(logger, "L1InterfaceStub::RequestTimeout::onTimeout() => "
                        "Channel deallocation timeout!");
                resp.setChannelID(mChannelID);
                resp.setOpCode(E_DEALLOCATE_CHANNEL_RESP);
                resp.setData(reinterpret_cast<UInt8 *>(&data), sizeof(UInt32));
                resp.setErrorCode(ConnectivityAgentError::ERROR_TIMEOUT);
                UInt8* buf = new UInt8[resp.getObjectSize()];
                resp.copyToRawArray(buf);

                BaseError err = mpOwner->mpIpc->asyncRequest(mpOwner->mMsgIdGen.next(),
                        buf, resp.getObjectSize(), &client_dir);
                if (!err.isNoError())
                {
                    LOG4CPLUS_WARN(logger, static_cast<std::string>(err));
                }

                delete [] buf;
            }
            else
            {
                LOG4CPLUS_INFO(logger, "L1InterfaceStub::RequestTimeout::onTimeout() => "
                        "channel Deallocated!");
            }
            break;
        }
        default:
            break;
        }
    }
}

