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


#include <stdlib.h>
#include <cstring>
#include <cassert>
#include <string>
#include <iostream>

#include "AppLibTestProfile.hpp"

//namespace required for working with receiving and sending data
using namespace iviLink::Channel;

Logger AppLibTestProfile::mLogger = Logger::getInstance(
        LOG4CPLUS_TEXT("samples.Profiles.AppLibTestProfile"));

/**
 * Sending the summands
 */
void AppLibTestProfile::sendData(std::string data)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    iviLink::Error err = iviLink::Channel::sendBuffer(mChannelId, data.c_str(), data.length());

    if (!err.isNoError())
    {
        LOG4CPLUS_INFO(mLogger, "TEST PROFILE data sent: " + data);
    }
    else
    {
        LOG4CPLUS_ERROR(mLogger, "TEST PROFILE Error: " + err.getCode());
    }
}

AppLibTestProfile::AppLibTestProfile(iviLink::Profile::IProfileCallbackProxy* callbackProxy)
        : mChannelId(0), mApplicationCallbacks(
                static_cast<IAppLibTestProfile_API::Callbacks*>(callbackProxy))
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

}

AppLibTestProfile::~AppLibTestProfile()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    if (mChannelId != 0)
    {
        //deallocating channel after deleting instance 
        deallocateChannel (mChannelId);
        mChannelId = 0;
    }
}

/**
 * Allocating channel
 */
void AppLibTestProfile::onEnable()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    //Allocating channel
    iviLink::Error err = allocateChannel("AppLibTestProfile", this, mChannelId, eRealTime);
    if (mChannelId != 0)
    {
        LOG4CPLUS_INFO(mLogger,
                "TEST PROFILE channel allocated: " + convertIntegerToString(mChannelId));
    }
    else
    {
        err = allocateChannel("AppLibTestProfile", this, mChannelId, eRealTime);
        if (mChannelId == 0)
        {
            LOG4CPLUS_ERROR(mLogger, "TEST APP Allocate error:  " + err.getCode());
            exit(1);
        }

    }
}

/**
 * Deallocating channel
 */
void AppLibTestProfile::onDisable()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    //Deallocating channel
    deallocateChannel (mChannelId);
}

/**
 * Callback that should be invoked when the data is received
 */
void AppLibTestProfile::onBufferReceived(const tChannelId channel, Buffer const& buffer)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "TEST APP onBufferReceived: " + convertIntegerToString(channel));

    //checking channel number
    assert(channel == mChannelId);
    if (channel != mChannelId)
    {
        LOG4CPLUS_ERROR(mLogger,
                "TEST APP onBufferReceived wring channel id error"
                        + convertIntegerToString(channel));
        return;
    }

    //incoming data
    UInt8 *incomingData = buffer.getBuffer();
    int readSize = buffer.getSize();

    std::string receivedData((const char*) incomingData);

    //send data to the application
    mApplicationCallbacks->dataReceived(receivedData);

    return;
}

/**
 * Callback that should be invoked when the channel is deleted
 */
void AppLibTestProfile::onChannelDeleted(const UInt32 channel_id)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "TEST APP onChannelDeleted:  " + convertIntegerToString(channel_id));

    assert(channel_id == mChannelId);
    if (channel_id != mChannelId)
    {
        LOG4CPLUS_ERROR(mLogger,
                "TEST APP onChannelDeleted wring channel id error:"
                        + convertIntegerToString(channel_id));
        return;
    }

    //Deallocating channel
    deallocateChannel (mChannelId);
    mChannelId = 0;
}

/**
 * Callback that should be invoked when the connection is lost
 */
void AppLibTestProfile::onConnectionLost()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "TEST APP onConnectionLost: ");
}

