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





#include "CClimateReceiverProfile.hpp"

#include "framework/components/ChannelSupervisor/Tube/ChannelSupervisorTube.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/threads/CSignalSemaphore.hpp"

#include <cstring>
#include <cassert>

namespace {
    Logger msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CClimateReceiverProfile"));
    const std::string ch_tag = "Climate channel";
}

CClimateReceiverProfile::CClimateReceiverProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
   : mChannelID(0)
   , mpAppCallbacks(static_cast<profile_callback_t*>(pCbProxy))
{
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
  PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
}

CClimateReceiverProfile::~CClimateReceiverProfile()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if( mChannelID )
     iviLink::Channel::deallocateChannel( mChannelID );
}


void CClimateReceiverProfile::onEnable()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   assert(mpAppCallbacks);
   if( mpAppCallbacks )
       mChannelID = iviLink::Channel::allocateChannel(mpAppCallbacks->get_channel_id(), this);
   if (mChannelID)
   {
      LOG4CPLUS_INFO(msLogger, "Channel allocated, starting the communication...");
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger, "allocate Channel failed");
   }
}

void CClimateReceiverProfile::onDisable()
{
   CError err = iviLink::Channel::deallocateChannel(mChannelID);
   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
   else
     mChannelID = 0;
}

//from CChannelHandler
void CClimateReceiverProfile::bufferReceived(const iviLink::Channel::tChannelId channel, iviLink::CBuffer const& buffer)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if (mChannelID != channel)
   {
      LOG4CPLUS_INFO(msLogger, "mChannelID != channel");
      return;
   }
   else
   {
      LOG4CPLUS_INFO(msLogger, "mChannelID == channel");
   }

   state_t st;
   if( st.ParseFromArray( buffer.getBuffer(), buffer.getSize() ) )
   {
       //LOG4CPLUS_INFO( msLogger, "seat receiver profile: recive\n" + st.DebugString() );
       receive( st );
   }
   else
     LOG4CPLUS_ERROR( msLogger, "Failed to deserialize state from buffer" );
}

void CClimateReceiverProfile::channelDeletedCallback(const UInt32 channel_id)
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

void CClimateReceiverProfile::connectionLostCallback()
{
    LOG4CPLUS_ERROR(msLogger, "seat receiver profile: connection lost!");
}

