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


#include <cassert>
#include <strings.h>
#include "ChannelSupervisorTube.hpp"
#include "NegotiatorTube.hpp"
#include "Types.hpp"

using namespace iviLink::ChannelSupervisor;

Logger NegotiatorTube::mLogger = Logger::getInstance(
        LOG4CPLUS_TEXT("NegotiatorProcess.NegotiatorTube"));

NegotiatorTube::NegotiatorTube(NegotiatorStates * states, CNegotiatorToSysCtrlProxy* pMsgProxy)
    : mNegotiatorStates(states), 
      mMsgProxy(pMsgProxy)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    allocateServiceChannel(this, CS_SERVICE_CHANNEL);
    mMsgProxy->sendConnectionEstablished();
}

NegotiatorTube::~NegotiatorTube()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    deallocateChannel(CS_SERVICE_CHANNEL);
    mMsgProxy->sendConnectionLost();
}

void NegotiatorTube::sendDataTube(const std::string& data)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "sendDataTube: " + data);
    UInt8 * castData = reinterpret_cast<UInt8*>(const_cast<char*>(data.c_str()));
    iviLink::ChannelSupervisor::sendData(CS_SERVICE_CHANNEL, castData, data.length());
}

void NegotiatorTube::onDataReceived(const UInt32 channel_id, const UInt32 read_size)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    UInt8 * data = new UInt8[read_size];
    bzero(data, read_size);
    UInt32 receivedSize = 0;
    BaseError err = iviLink::ChannelSupervisor::receiveData(channel_id, data, receivedSize, read_size);
    if (err.isNoError())
    {
        mNegotiatorStates->processTubeNotification(data);
    } 
    else
    {
        LOG4CPLUS_WARN(mLogger, "Could not receive data: " + static_cast<std::string>(err));
    }
    delete[] data;
}

void NegotiatorTube::onBufferOverflow(const UInt32 channel_id)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void NegotiatorTube::onChannelDeleted(const UInt32 channel_id)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void NegotiatorTube::onConnectionLost()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mMsgProxy->sendConnectionLost();
}
