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
 

#include <cassert>
#include <strings.h>
#include "ChannelSupervisorTube.hpp"
#include "NegotiatorTube.hpp"
#include "Types.hpp"

using namespace iviLink::ChannelSupervisor;

Logger NegotiatorTube::msLogger = Logger::getInstance(
        LOG4CPLUS_TEXT("NegotiatorProcess.NegotiatorTube"));

void NegotiatorTube::sendDataTube(const char * data)
{
    m_sender->sendDataRemote(data);
}

void NegotiatorTube::onDataReceived(const unsigned int channel_id, const unsigned int read_size)
{
    assert(channel_id == CS_SERVICE_CHANNEL);
    LOG4CPLUS_TRACE(msLogger,
            "NegotiatorTube::onDataReceived()=> channel" + convertIntegerToString(channel_id)
                    + "size" + convertIntegerToString(read_size));

    UInt8 * data = new UInt8[read_size];
    bzero(data, read_size);

    unsigned int receivedSize = 0;

    CError err = iviLink::ChannelSupervisor::receiveData(channel_id, data, receivedSize, read_size);

    if (err.isNoError())
    {
        m_negotiatorStates->ProcessTubeNotification(data);
    } else
    {
        LOG4CPLUS_WARN(msLogger,
                "NegotiatorTube::bufferOverflowCallback()=> fail: "
                        + static_cast<std::string>(err));
    }

    if (err.isNoError())
    {
        m_negotiatorStates->ProcessTubeNotification(data);
    } else
    {
        LOG4CPLUS_WARN(msLogger,
                "NegotiatorTube::bufferOverflowCallback()=> fail: "
                        + static_cast<std::string>(err));
    }

    delete[] data;
}

void NegotiatorTube::onBufferOverflow(const unsigned int channel_id)
{
    LOG4CPLUS_TRACE(msLogger,
            "NegotiatorTube::bufferOverflowCallback()=> channel"
                    + convertIntegerToString(channel_id));
}

void NegotiatorTube::onChannelDeleted(const unsigned int channel_id)
{
    LOG4CPLUS_TRACE(msLogger,
            "NegotiatorTube::channelDeletedCallback()=> channel"
                    + convertIntegerToString(channel_id));
}

void NegotiatorTube::onConnectionLost()
{
    LOG4CPLUS_TRACE(msLogger, "NegotiatorTube::connectionLostCallback()");
    mpMsgProxy->sendConnectionLost();
}

void NegotiatorTube::Sender::sendDataRemote(std::string const& data)
{
    LOG4CPLUS_TRACE(msLogger, "NegotiatorTube::sendDataRemote()");
    iviLink::ChannelSupervisor::sendData(m_CSChannelID,
            reinterpret_cast<UInt8*>(const_cast<char*>(data.c_str())), data.length());
}
