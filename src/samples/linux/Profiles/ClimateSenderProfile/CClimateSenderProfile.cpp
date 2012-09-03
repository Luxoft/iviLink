/* 
 * 
 * iviLINK SDK, version 1.1.2
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
 * 
 */





#include "CClimateSenderProfile.hpp"

#include "framework/components/ChannelSupervisor/Tube/ChannelSupervisorTube.hpp"
#include "CSenderThread.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/threads/CSignalSemaphore.hpp"

#include <cstring>
#include <cassert>

using iviLink::CBuffer;

Logger CClimateSenderProfile::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CClimateSenderProfile"));

CClimateSenderProfile::CClimateSenderProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
   : mChannelID(0)
   , mpAppCallbacks(static_cast<profile_callback_t*>(pCbProxy))
   , mSenderThread(this)
   , mBe(true)
{
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
  PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
}

CClimateSenderProfile::~CClimateSenderProfile()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   mSenderThread.disconnect();
   if( mChannelID )
     iviLink::Channel::deallocateChannel(mChannelID);
}


void CClimateSenderProfile::send_state( const IClimateSenderProfile::state_t& state )
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   std::string buf;
   if( state.SerializeToString(&buf) )
   {
       //LOG4CPLUS_INFO( msLogger, "seat sender profile: send\n" + state.DebugString() );
     {
       const lock l( mReqMutex );
       mReqQueue.push( buffer_t() );
       mReqQueue.back().swap( buf ); // avoid copy
     }
     mReqSemaphore.signal();
   }
   else
      LOG4CPLUS_ERROR(msLogger, "Unable to serialize state");
}


void CClimateSenderProfile::onEnable()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   assert( mpAppCallbacks );
   if( mpAppCallbacks )
       mChannelID = iviLink::Channel::allocateChannel(mpAppCallbacks->get_channel_id(), this);

   if (mChannelID)
   {
      LOG4CPLUS_INFO(msLogger, "Channel allocated, starting the communication...");
      mSenderThread.start();
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger, "allocate Channel failed");
   }
}

void CClimateSenderProfile::onDisable()
{
   CError err = iviLink::Channel::deallocateChannel(mChannelID);
   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
}

//from CChannelHandler
void CClimateSenderProfile::bufferReceived(const iviLink::Channel::tChannelId channel, iviLink::CBuffer const& buffer)
{
   LOG4CPLUS_ERROR(msLogger, "seat sender profile: Don't know what to do with received data: this is one-way channel");
}

void CClimateSenderProfile::channelDeletedCallback(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   if (mChannelID == channel_id)
      mChannelID = 0;

   CError err = iviLink::Channel::deallocateChannel(channel_id);
   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
}

void CClimateSenderProfile::connectionLostCallback()
{
    //LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_ERROR(msLogger, "seat sender profile: connection lost!");
}

void CClimateSenderProfile::senderLoop()
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

bool CClimateSenderProfile::hasRequests()
{
  lock l( mReqMutex );
  return !mReqQueue.empty();
}

void CClimateSenderProfile::handleRequest()
{
  buffer_t buf;
  {
    lock l( mReqMutex );
    buf.swap( mReqQueue.front() );
    mReqQueue.pop();
  }

  CError err = iviLink::Channel::sendBuffer(mChannelID,
                                            reinterpret_cast<const void*>(buf.c_str()),
                                            static_cast<UInt32>(buf.size()));
  if (!err.isNoError())
  {
      LOG4CPLUS_INFO(msLogger, "CClimateSenderProfile::handleRequest() :: Send error"
                     + static_cast<std::string>(err));
  }

}
