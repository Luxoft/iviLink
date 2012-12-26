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
 

#include <map>

#include "Logger.hpp"
#include "CMutex.hpp"
#include "ChannelSupervisorTube.hpp"

#include "Channel.hpp"

namespace iviLink
{
namespace Channel
{

typedef std::map<tChannelId, std::string> tChannelTagMap;
tChannelTagMap gChannelTagMap;
CMutex gChannelTagMutex;

const char gModuleName[] = "profileLib.Channel";
Logger gsLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileLib.Channel"));

tChannelId allocateChannel(std::string const& tag,
                           CChannelHandler * pHandler,
                           TChannelPriority priority)
{
    LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
    tChannelId id = 0;
    CError err = iviLink::ChannelSupervisor::allocateChannel(pHandler, tag, id, priority);
    if (err.isNoError())
    {
        gChannelTagMap[id] = tag;
        return id;
    }

    LOG4CPLUS_ERROR(gsLogger, "Error allocating channel: " + static_cast<std::string>(err));
    return 0;
}

tChannelId allocateChannelAsClient(std::string const& tag,
                                   iviLink::Channel::CChannelHandler * pHandler,
                                   TChannelPriority priority)
{
    LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
    std::string fixedTag(tag.c_str());
    fixedTag.append(TAG_SUFFIX_SEPARATOR).append(TAG_CLIENT_SUFFIX);
    return allocateChannel(fixedTag, pHandler, priority);
}

tChannelId allocateChannelAsServer(std::string const& tag,
                                   iviLink::Channel::CChannelHandler * pHandler,
                                   TChannelPriority priority)
{
    LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
    std::string fixedTag(tag.c_str());
    fixedTag.append(TAG_SUFFIX_SEPARATOR).append(TAG_SERVER_SUFFIX);
    return allocateChannel(fixedTag, pHandler, priority);
}

CError deallocateChannel(const tChannelId channel)
{
    LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
    gChannelTagMutex.lock();
    tChannelTagMap::iterator iter = gChannelTagMap.find(channel);
    if (iter != gChannelTagMap.end())
    {
        CError err = iviLink::ChannelSupervisor::deallocateChannel(channel);
        gChannelTagMap.erase(iter);
        gChannelTagMutex.unlock();
        return err;
    }
    gChannelTagMutex.unlock();

    LOG4CPLUS_ERROR(gsLogger, "Error deallocating channel: channel not found");
    return CError(1, gModuleName, CError::ERROR, "Error deallocating channel: channel not found");
}

CError sendBuffer(const tChannelId channel, CBuffer const& buffer)
{
    return sendBuffer(channel, buffer.getBuffer(), buffer.getWritePosition());
}

CError sendBuffer(const tChannelId channel, void const* pBuffer, const UInt32 bufferSize)
{
    return iviLink::ChannelSupervisor::sendData(channel, static_cast<UInt8 const*>(pBuffer),
            bufferSize);
}

void CChannelHandler::onDataReceived(const tChannelId channelId, const UInt32 readSize)
{
    LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
    UInt8* pbuf = new UInt8[readSize];
    UInt32 receivedSize = 0;

    CError err = iviLink::ChannelSupervisor::receiveData(channelId, pbuf, receivedSize, readSize);
    if (err.isNoError())
    {
        CBuffer buf(pbuf, readSize);
        buf.setWritePosition(receivedSize);
        this->onBufferReceived(channelId, buf);
    } else
    {
        LOG4CPLUS_ERROR(gsLogger,
                "CChannelHandler::dataReceivedCallback error: " + static_cast<std::string>(err));
    }

    delete[] pbuf;
}

void CChannelHandler::onBufferOverflow(const tChannelId channelId)
{
    LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_ERROR(gsLogger,
            "CChannelHandler::bufferOverflowCallback channelId="
                    + convertIntegerToString(channelId));
}

}  // namespace Channel

}  // namespace iviLink
