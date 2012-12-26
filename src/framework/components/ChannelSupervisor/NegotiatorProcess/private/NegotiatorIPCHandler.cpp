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
 

#include <sys/time.h>

#include "pugixml.hpp"
#include "Notification.hpp"
#include "CSError.hpp"

//local messages
#include "AllocateChannelRequest.hpp"
#include "DeallocateChannelRequest.hpp"
#include "AllocateChannelResponse.hpp"
#include "DeallocateChannelResponse.hpp"
#include "AllocateRequest.hpp"
#include "DeallocateRequest.hpp"
#include "AllocateResponse.hpp"
#include "DeallocateResponse.hpp"
#include "CAAllocDoneRequest.hpp"
#include "CAAllocDoneResponse.hpp"
#include "AllocateNotification.hpp"
#include "MapCheckNotification.hpp"
#include "UpdateMapNotification.hpp"
#include "DeallocateNotification.hpp"
#include "CAAllocationNotification.hpp"

#include "CommandRejectResponse.hpp"

#include "ChannelSupervisorTube.hpp"

#include "NegotiatorIPCHandler.hpp"
#include "NegotiatorStates.hpp"
#include "NegotiatorTube.hpp"

using namespace iviLink;
using namespace ChannelSupervisor;
using namespace Messages;

const int MAX_TRY_COUNT = 50;
const char defErrMsg[] = "no error occurred";
Logger NegotiatorIPCHandler::msLogger = Logger::getInstance(
        LOG4CPLUS_TEXT("NegotiatorProcess.NegotiatorIPCHandler"));

NegotiatorIPCHandler::NegotiatorIPCHandler(NegotiatorTube * tube, NegotiaterStates * states,
        std::string const& sock_addr)
        : m_id(1), m_negotiatorTube(tube), m_negotiatorStates(states)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    char const* addr = ipcNegotiatorId;
    if (!sock_addr.empty())
        addr = sock_addr.c_str();

    m_ipc = new CIpc(addr, *this);
    m_map = Map::getInstance();
}

NegotiatorIPCHandler::~NegotiatorIPCHandler()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (m_ipc)
    {
        delete m_ipc;
    }
    m_ipc = 0;
    if (m_negotiatorTube)
    {
        delete m_negotiatorTube;
    }
    m_negotiatorTube = NULL;
    if (m_negotiatorStates)
    {
        delete m_negotiatorStates;
    }
    m_negotiatorStates = NULL;
}

void NegotiatorIPCHandler::OnRequest(MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
        UInt8* const pResponseBuffer, UInt32& bufferSize, DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::string response;
    ProcessIPCClientMessage(std::string((const Int8*) pPayload).substr(0, payloadSize), response);

    //put the response to the buffer
    bufferSize = response.size();
    memcpy(pResponseBuffer, response.c_str(), bufferSize);
}

void NegotiatorIPCHandler::OnAsyncRequest(MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
             iviLink::Ipc::DirectionID dirId)
{

}

void NegotiatorIPCHandler::OnConnectionLost(DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    m_connLostSem.signal();
}

bool NegotiatorIPCHandler::IPCConnect()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    CError err = CError::NoError();
    m_ipc->beginWaitForConnection();
    return false;
}

void NegotiatorIPCHandler::IPCWait()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    m_reqSem.wait();
}

void NegotiatorIPCHandler::dismissSema()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    m_reqSem.signal();
}

void NegotiatorIPCHandler::IPCDisconnect()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    m_ipc->endWaitForConnection();
    m_ipc->disconnect();
    m_connLostSem.wait();
}

void NegotiatorIPCHandler::ProcessIPCClientMessage(std::string message, std::string &response)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    ChannelSupervisor::Messages::Message * msg = NULL;
    CError err = CSError(CSError::ERROR_OTHER);

    LOG4CPLUS_TRACE(msLogger,
            "NegotiatorIPCHandler::ProcessIPCClientMessage() message = \n" + message);

    pugi::xml_document* doc = new pugi::xml_document();
    ParseResponse(doc, (Int8*) message.c_str());

    if (Notification::NOTIFICATIONTTYPE_ALLOCATE == Notification::GetNotificationType(doc))
    {
        LOG4CPLUS_INFO(msLogger, "NegotiatorIPCHandler::ProcessIPCClientMessage()==>ALLOCATE=====");
        AllocateChannelRequest * request = new AllocateChannelRequest(doc);
        std::string tag = request->GetTag();
        UInt32 channel = request->GetChannelID();
        delete request;
        err = NegotiateChannel(tag, channel);
        LOG4CPLUS_INFO(msLogger,
                "NegotiatorIPCHandler::ProcessIPCClientMessage()==>IDLE====="
                        + convertIntegerToString(channel));
        msg = new AllocateChannelResponse(tag.c_str(), channel);
        if (!err.isNoError())
        {
            // TODO correctly set other types of errors
            if (err.getCode() == CSError::NEGOTIATION_CHANNEL_TIMEOUT)
            {
                ((AllocateChannelResponse*) msg)->SetError(ERRORCODE_TIMEOUT_OCCURRED, "timeout");
            } else
            {
                ((AllocateChannelResponse*) msg)->SetError(ERRORCODE_OTHER, "other error");
            }
        }
    } else if (Notification::NOTIFICATIONTTYPE_DEALLOCATE == Notification::GetNotificationType(doc))
    {
        LOG4CPLUS_INFO(msLogger,
                "NegotiatorIPCHandler::ProcessIPCClientMessage()==>DEALLOCATE=====");
        DeallocateChannelRequest * request = new DeallocateChannelRequest(doc);
        UInt32 channel = request->GetChannelId();
        delete request;
        err = ChannelDeallocated(channel);
        msg = new DeallocateChannelResponse(channel);
        if (!err.isNoError())
        {
            // TODO correctly set other types of errors
            if (err.getCode() == CSError::DEALLOCATION_CHANNEL_MAP_WRONG_CID)
                ((DeallocateChannelResponse*) msg)->SetError(ERRORCODE_WRONG_CHANNELID,
                        "wrong cid");
            else if (err.getCode() == CSError::DEALLOCATION_CHANNEL_MAP_TIMEOUT)
                ((DeallocateChannelResponse*) msg)->SetError(ERRORCODE_TIMEOUT_OCCURRED, "timeout");
            else
                ((DeallocateChannelResponse*) msg)->SetError(ERRORCODE_OTHER, "other error");
        }
    } else if (Request::REQUESTTYPE_CA_ALLOCATION_DONE == Request::GetRequestType(doc))
    {
        LOG4CPLUS_INFO(msLogger,
                "NegotiatorIPCHandler::ProcessIPCClientMessage()==>CA_ALLOCATION_DONE=====");
        CAAllocateDoneRequest * request = new CAAllocateDoneRequest(doc);
        std::string tag = request->GetTag();
        UInt32 channel = request->GetChannelId();
        delete request;
        err = UpdateMap(tag, channel);
        msg = new CAAllocateDoneResponse(tag.c_str(), channel);
        if (!err.isNoError())
        {
            // TODO correctly set other types of errors
            if (err.getCode() == CSError::UPDATE_MAP_TIMEOUT)
            {
                ((CAAllocateDoneResponse*) msg)->SetError(ERRORCODE_TIMEOUT_OCCURRED, "timeout");
            } else
            {
                ((CAAllocateDoneResponse*) msg)->SetError(ERRORCODE_OTHER, "other error");
            }
        }
    } else //error here
    {
        LOG4CPLUS_ERROR(msLogger,
                "NegotiatorIPCHandler::ProcessIPCClientMessage()==> UNKNOWN REQUEST");
        msg = new CommandRejectResponse("");
    }

    std::stringstream messageToBeSent;

    msg->Write(messageToBeSent);
    response = messageToBeSent.str();

    if (msg)
        delete msg;
    msg = NULL;
}

CError NegotiatorIPCHandler::NegotiateChannel(std::string tag, UInt32 & cid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_TRACE(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>START");

    CError err = CSError(CSError::ERROR_OTHER);

    //get channel id from the map
    UInt32 channelId = m_map->getNextFreeChannelId(cid);
    LOG4CPLUS_INFO(msLogger,
            "NegotiatorIPCHandler::NegotiateChannel()==>channel"
                    + convertIntegerToString(channelId));

    Notification * noti = NULL;

    noti = new AllocateNotification(tag.c_str(), channelId);
    std::stringstream messageToBeSent;
    noti->Write(messageToBeSent);
    delete noti;

    LOG4CPLUS_INFO(msLogger,
            "NegotiatorIPCHandler::NegotiateChannel()=>Message: \n==========\n"
                    + messageToBeSent.str() + "\n===========\n");

    m_negotiatorTube->sendDataTube(messageToBeSent.str().c_str());

    messageToBeSent.str(std::string());

    LOG4CPLUS_INFO(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>WAIT FOR NEGOTIATED");
    waitStateForTag(NegotiaterStates::NEGOTIATED, tag, OTHER_SIDE_RESP_TIMEOUT_MS);
    //here both messages received
    LOG4CPLUS_INFO(msLogger,
            "NegotiatorIPCHandler::NegotiateChannel()==>NEGOTIATED "
                    + convertIntegerToString(channelId));

    //get remote notification
    AllocateNotification * remoteNotification =
            (AllocateNotification *) m_negotiatorStates->GetNotification(tag.c_str());

    UInt32 remoteCid = 0;

    ErrorCode errc;
    const char * errstr;
    if (remoteNotification)
    {
        remoteNotification->GetError(errc, errstr);

        if (errc == ERRORCODE_SUCCESS)
        {
            remoteCid = remoteNotification->GetChannelId();
            if (remoteCid >= channelId)   //use remote
            {
                if (!m_map->isChannelok(remoteCid))
                {
                    return err;
                } else
                {
                    channelId = remoteCid;
                }
            }
        } else
        {
            LOG4CPLUS_ERROR(msLogger,
                    "NegotiatorIPCHandler::NegotiateChannel()==>remote notification error"
                            + (std::string)(errstr));
            return err;
        }
    } else
    {
        LOG4CPLUS_ERROR(msLogger,
                "(AllocateNotification *) m_negotiatorStates->GetNotification \
                returned NULL! other side response timed out");
        return CSError(CSError::NEGOTIATION_CHANNEL_TIMEOUT);
    }

    //create map Notification message
    noti = new MapCheckNotification(tag.c_str(), channelId);
    //send the message
    noti->Write(messageToBeSent);
    delete noti;
    LOG4CPLUS_INFO(msLogger,
            "NegotiatorIPCHandler::NegotiateChannel()=>Message: \n==========\n"
                    + messageToBeSent.str() + "\n===========\n");

    m_negotiatorTube->sendDataTube(messageToBeSent.str().c_str());

    LOG4CPLUS_INFO(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>WAIT FOR CHECKEDINMAP");
    waitStateForTag(NegotiaterStates::CHECKEDINMAP, tag, OTHER_SIDE_RESP_TIMEOUT_MS);
    LOG4CPLUS_INFO(msLogger,
            "NegotiatorIPCHandler::NegotiateChannel()==>CHECKEDINMAP "
                    + convertIntegerToString(channelId));

    //get remote notification
    MapCheckNotification * mapRemoteNotification =
            (MapCheckNotification *) m_negotiatorStates->GetNotification(tag.c_str());

    if (mapRemoteNotification)
    {
        mapRemoteNotification->GetError(errc, errstr);

        if (errc == ERRORCODE_SUCCESS)
        {
            cid = channelId;
            err = CError(CError::NO_ERROR, "ChannelSupervisorProcess");
        } else
        {
            LOG4CPLUS_ERROR(msLogger,
                    "NegotiatorIPCHandler::NegotiateChannel()==>remote Error "
                            + convertIntegerToString((int) errc) + (std::string)(errstr));
            err = CSError(CSError::ERROR_OTHER);
        }
    } else
    {
        LOG4CPLUS_ERROR(msLogger,
                "(MapCheckNotification *) m_negotiatorStates->GetNotification returned \
                NULL! other side response timed out");
        err = CSError(CSError::NEGOTIATION_CHANNEL_TIMEOUT);
    }
    return err;
}

CError NegotiatorIPCHandler::ChannelDeallocated(const UInt32 cid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    CError err = CSError(CSError::ERROR_OTHER);
    {
        ChannelInfo info;
        if (!m_map->getChannelInfo(cid, info) || info.tag.empty())
            return CSError(CSError::DEALLOCATION_CHANNEL_MAP_WRONG_CID);
    }

    m_map->invalidateChannelInfo(cid);
    return err;
}

CError NegotiatorIPCHandler::UpdateMap(std::string tag, UInt32 cid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    CError err = CSError(CSError::ERROR_OTHER);

    //store in the map
    ChannelInfo chInfo;
    chInfo.tag = tag;

    chInfo.timeStamp = time(NULL);

    Notification * noti = new CAAllocationNotification(tag.c_str(), cid);
    std::stringstream messageToBeSent;
    noti->Write(messageToBeSent);
    delete noti;

    LOG4CPLUS_INFO(msLogger,
            "NegotiatorIPCHandler::UpdateMap()=>Message: \n==========\n" + messageToBeSent.str()
                    + "\n===========\n");
    m_negotiatorTube->sendDataTube(messageToBeSent.str().c_str());

    LOG4CPLUS_INFO(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>WAIT FOR CAALLOCATED");
    waitStateForTag(NegotiaterStates::CAALLOCATED, tag, OTHER_SIDE_RESP_TIMEOUT_MS);
    LOG4CPLUS_INFO(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>CAALLOCATED");

    if (m_negotiatorStates->GetTagState(tag.c_str()) != NegotiaterStates::CAALLOCATED)
    {
        LOG4CPLUS_ERROR(msLogger, "CAALLOCATED: other side response timed out for tag " + tag);
        return CSError(CSError::UPDATE_MAP_TIMEOUT);
    }

    messageToBeSent.str(std::string());

    m_map->addChannelInfo(cid, chInfo);

    noti = new UpdateMapNotification(tag.c_str(), cid);
    noti->Write(messageToBeSent);
    delete noti;

    LOG4CPLUS_INFO(msLogger,
            "NegotiatorIPCHandler::UpdateMap()=>Message: \n==========\n" + messageToBeSent.str()
                    + "\n===========\n");
    m_negotiatorTube->sendDataTube(messageToBeSent.str().c_str());

    LOG4CPLUS_INFO(msLogger,
            "NegotiatorIPCHandler::UpdateMap()=> Current state is "
                    + m_negotiatorStates->StateToString(
                            m_negotiatorStates->GetTagState(tag.c_str())));
    m_negotiatorTube->sendDataTube(messageToBeSent.str().c_str());

    LOG4CPLUS_INFO(msLogger,
            "NegotiatorIPCHandler::NegotiateChannel()==>WAIT FOR ALLOCDONE / IDLE");
    waitStateForTag(NegotiaterStates::IDLE, tag, OTHER_SIDE_RESP_TIMEOUT_MS);
    LOG4CPLUS_INFO(msLogger, "NegotiatorIPCHandler::NegotiateChannel()==>ALLOCDONE / IDLE");

    //get remote notification
    UpdateMapNotification * remoteNotification =
            (UpdateMapNotification *) m_negotiatorStates->GetNotification(tag.c_str());

    ErrorCode errc;
    const char * errstr;
    if (remoteNotification)
    {
        remoteNotification->GetError(errc, errstr);

        if (errc == ERRORCODE_SUCCESS)
        {
            err = CSError::NoCSError("no error");
        }
    } else
    {
        LOG4CPLUS_ERROR(msLogger,
                "(UpdateMapNotification *) m_negotiatorStates->GetNotification \
                returned NULL! other side response timed out");
        err = CSError(CSError::UPDATE_MAP_TIMEOUT);
    }

    return err;
}

void iviLink::ChannelSupervisor::ParseResponse(pugi::xml_document* doc, char* responseStr)
{
    pugi::xml_parse_result res;
    res = doc->load((const char *) responseStr);
}

void NegotiatorIPCHandler::waitStateForTag(NegotiaterStates::STATE stateToWait, std::string tag,
        UInt32 timeoutMillis)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(msLogger,
            "Wait for state: " + m_negotiatorStates->StateToString(stateToWait) + " for tag: "
                    + tag);
    m_negotiatorStates->mStateCond.lock();
    {
        timeval begin, end;
        double elapsedTime = 0.0;
        gettimeofday(&begin, NULL);
        UInt32 timeoutToWait = timeoutMillis;
        while (m_negotiatorStates->GetTagState(tag.c_str()) != stateToWait
                && (UInt32) elapsedTime < timeoutMillis)
        {
            m_negotiatorStates->mStateCond.waitTimeout(timeoutToWait);
            gettimeofday(&end, NULL);
            elapsedTime = (end.tv_sec - begin.tv_sec) * 1000.0;      // sec to ms
            elapsedTime += (end.tv_usec - begin.tv_usec) / 1000.0;   // us to ms
            LOG4CPLUS_INFO(msLogger,
                    "Wait for state: " + m_negotiatorStates->StateToString(stateToWait)
                            + " elapsed: " + convertIntegerToString((UInt32) elapsedTime) + "ms");
            timeoutToWait = timeoutMillis - (UInt32) elapsedTime;
        }
    }
    m_negotiatorStates->mStateCond.unlock();
}
