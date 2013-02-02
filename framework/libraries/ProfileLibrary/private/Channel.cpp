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


#include <map>

#include "Logger.hpp"
#include "CRWMutex.hpp"
#include "ChannelSupervisorTube.hpp"
#include "Channel.hpp"
#include "CThread.hpp"

#include "KickWatchdogThread.hpp"
#include "ChannelTagMap.hpp"

namespace iviLink
{
namespace Channel
{

const char gModuleName[] = "profileLib.Channel";
Logger gsLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileLib.Channel"));

Error allocateChannel(std::string const& tag, ChannelHandler * pHandler, tChannelId& resultChannelId, TChannelPriority priority)
{
    LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(gsLogger, "Allocating channel with priority: " + convertIntegerToString((int)priority) + " and tag: " + tag);
    BaseError err = iviLink::ChannelSupervisor::allocateChannel(pHandler, tag, resultChannelId, priority);
    
    if (err.isNoError())
    {
        ChannelTagMap::getInstance()->insertIntoMap(tag, resultChannelId);
        KickWatchdogThread::getInstance()->startKicking();
        return Error::NoError();
    }

    LOG4CPLUS_ERROR(gsLogger, "Error allocating channel: " + static_cast<std::string>(err));
    resultChannelId = 0;
    if (err.getCode() == iviLink::ChannelSupervisor::CSError::IPC_NO_DATA_ERROR)
    {
        return Error(IPC_REQUEST_TIMEOUT, "Either Connectivity Agent or Negotiator are busy, try again", true);
    }
    return Error(CHANNEL_ALLOCATION_FATAL_ERROR, 
        "Error allocating channel - restart of application or, at least, service reload advised", false);
}

Error allocateChannelAsClient(std::string const& tag,
        iviLink::Channel::ChannelHandler * pHandler, tChannelId& resultChannelId, TChannelPriority priority)
{
    LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
    std::string fixedTag(tag.c_str());
    fixedTag.append(TAG_SUFFIX_SEPARATOR).append(TAG_CLIENT_SUFFIX);
    return allocateChannel(fixedTag, pHandler, resultChannelId, priority);
}

Error allocateChannelAsServer(std::string const& tag,
        iviLink::Channel::ChannelHandler * pHandler, tChannelId& resultChannelId, TChannelPriority priority)
{
    LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
    std::string fixedTag(tag.c_str());
    fixedTag.append(TAG_SUFFIX_SEPARATOR).append(TAG_SERVER_SUFFIX);
    return allocateChannel(fixedTag, pHandler, resultChannelId, priority);
}

Error deallocateChannel(const tChannelId channel)
{
    LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
    if (ChannelTagMap::getInstance()->hasChannelInMap(channel))
    {
        BaseError err = iviLink::ChannelSupervisor::deallocateChannel(channel);
        if (err.getCode() == iviLink::ChannelSupervisor::CSError::IPC_NO_DATA_ERROR)
        {
            return Error(IPC_REQUEST_TIMEOUT, "Connectivity Agent not responding, try again later",true);
        }
        ChannelTagMap::getInstance()->eraseFromMap(channel);
        if (!err.isNoError())
        {
            return Error(CHANNEL_DEALLOCATION_ERROR, 
                "Channel deallocation encountered some problems with either CA or CS. Channel info was deleted locally",false);
        }
        return Error::NoError();
    }

    LOG4CPLUS_ERROR(gsLogger, "Error deallocating channel: channel not found");
    return Error(CHANNEL_NOT_FOUND, "Channel not found: it is already deleted or it was never allocated",false);
}

Error sendBuffer(const tChannelId channel, Buffer const& buffer)
{
    return sendBuffer(channel, buffer.getBuffer(), buffer.getWritePosition());
}

Error sendBuffer(const tChannelId channel, void const* pBuffer, const UInt32 bufferSize)
{
    BaseError err = iviLink::ChannelSupervisor::sendData(channel, static_cast<UInt8 const*>(pBuffer),
            bufferSize);
    if (err.isNoError()) 
    {
        return Error::NoError();
    }
    if (err.getCode() == iviLink::ChannelSupervisor::CSError::SEND_TO_RESTRICTED_CHANNEL
        || err.getCode() == iviLink::ChannelSupervisor::CSError::SEND_TO_NOT_EXISTING_CHANNEL)
    {
        return Error(SEND_TO_NOT_ALLOCATED_CHANNEL, "Invalid channel ID: " + convertIntegerToString(channel), false);
    }
    if (err.getCode() == iviLink::ChannelSupervisor::CSError::IPC_NO_DATA_ERROR)
    {
        return Error(IPC_REQUEST_TIMEOUT, "ConnectivityAgent not responding, try again later",true);
    }
    return Error(UNKNOWN_ERROR, "Some error occurred", false);
}

void ChannelHandler::onDataReceived(const tChannelId channelId, const UInt32 readSize)
{
    LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
    UInt8* pbuf = new UInt8[readSize];
    UInt32 receivedSize = 0;
    BaseError err = iviLink::ChannelSupervisor::receiveData(channelId, pbuf, receivedSize, readSize);
    if (err.isNoError())
    {
        Buffer buf(pbuf, readSize);
        buf.setWritePosition(receivedSize);
        this->onBufferReceived(channelId, buf);
    } 
    else
    {
        // TODO ???
        LOG4CPLUS_ERROR(gsLogger,
                "CChannelHandler::dataReceivedCallback error: " + static_cast<std::string>(err));
    }

    delete[] pbuf;
}

void ChannelHandler::onBufferOverflow(const tChannelId channelId)
{
    LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_ERROR(gsLogger,
            "CChannelHandler::bufferOverflowCallback channelId="
                    + convertIntegerToString(channelId));
}

}  // namespace Channel

}  // namespace iviLink
