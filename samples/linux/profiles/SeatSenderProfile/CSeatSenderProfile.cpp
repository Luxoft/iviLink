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


#include "CSeatSenderProfile.hpp"

#include "CSenderThread.hpp"
#include "CMutex.hpp"
#include "CSignalSemaphore.hpp"
#include "Exit.hpp"

#include <cstring>
#include <cassert>

using iviLink::Buffer;

Logger CSeatSenderProfile::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profiles.sender.seat"));

CSeatSenderProfile::CSeatSenderProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
   : mChannelID(0)
   , mpAppCallbacks(static_cast<profile_callback_t*>(pCbProxy))
   , mSenderThread(this)
   , mBe(true)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
}

CSeatSenderProfile::~CSeatSenderProfile()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mBe = false;
    mReqSemaphore.signal();
    mSenderThread.disconnect();
    if (mChannelID)
    {
        iviLink::Channel::deallocateChannel(mChannelID);
    }
}

void CSeatSenderProfile::send_state( const ISeatSenderProfile::state_t& state )
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    std::string buf = state.serialize();
    mReqMutex.lock();
    mReqQueue.push(buffer_t());
    mReqQueue.back().swap(buf); // avoid copy
    mReqMutex.unlock();
    mReqSemaphore.signal();
}


void CSeatSenderProfile::onEnable()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mpAppCallbacks);
    if (mpAppCallbacks)
    {
        const std::string ch_id = mpAppCallbacks->get_channel_id();
        LOG4CPLUS_INFO(msLogger, "Trying allocate channel \"" + ch_id + "\"\n");
        iviLink::Error err = iviLink::Channel::allocateChannel(ch_id, this, mChannelID, eRealTime);

       if (err.isNoError())
       {
            LOG4CPLUS_INFO(msLogger, "Channel \""
                           + ch_id
                           + "\" allocated, starting the communication...");
            mSenderThread.start();
        }
        else
        {
            LOG4CPLUS_ERROR(msLogger, "Channel \""
                            + ch_id
                           + "\" allocation failed");
            killProcess(1);
        }
    }
    else
    {
        LOG4CPLUS_ERROR(msLogger, "Can't find application callbacks, so can't get correct channel id");
    }
}

void CSeatSenderProfile::onDisable()
{
    iviLink::Error err = iviLink::Channel::deallocateChannel(mChannelID);
    if (!err.isNoError())
    {
        LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
    }
}

//from CChannelHandler
void CSeatSenderProfile::onBufferReceived(const iviLink::Channel::tChannelId channel, iviLink::Buffer const& buffer)
{
    LOG4CPLUS_ERROR(msLogger, "seat sender profile: Don't know what to do with received data: this is one-way channel");
}

void CSeatSenderProfile::onChannelDeleted(const UInt32 channel_id)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    if (mChannelID == channel_id)
    {
        mChannelID = 0;
    }

    iviLink::Error err = iviLink::Channel::deallocateChannel(channel_id);
    if (!err.isNoError())
    {
        LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
    }
}

void CSeatSenderProfile::onConnectionLost()
{
    LOG4CPLUS_ERROR(msLogger, "seat sender profile: connection lost!");
}

void CSeatSenderProfile::senderLoop()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    while (mBe)
    {
        if (hasRequests())
        {
            handleRequest();
        }
        else
        {
            mReqSemaphore.wait();
        }
    }
}

bool CSeatSenderProfile::hasRequests()
{
    MutexLocker lock(mReqMutex);
    return !mReqQueue.empty();
}

void CSeatSenderProfile::handleRequest()
{
    buffer_t buf;
    MutexLocker lock(mReqMutex);
    buf.swap(mReqQueue.front());
    mReqQueue.pop();

  iviLink::Error err = iviLink::Channel::sendBuffer(mChannelID,
                                            reinterpret_cast<const void*>(buf.c_str()),
                                            static_cast<UInt32>(buf.size()));
    if (!err.isNoError())
    {
        LOG4CPLUS_INFO(msLogger, "CSeatSenderProfile::handleRequest() :: Send error"
                     + static_cast<std::string>(err));
    }
}
