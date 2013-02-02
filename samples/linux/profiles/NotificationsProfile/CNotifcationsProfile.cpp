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

#include "CNotificationsProfile.hpp"
#include "Exit.hpp"

//namespace required for working with receiving and sending data
using namespace iviLink::Channel;

Logger CNotificationsProfile::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CNotificationsProfile"));

void CNotificationsProfile::sendString(const std::string & data)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_TRACE(msLogger, data.c_str());
   UInt8* outgoingData = new UInt8[data.length()];
   memcpy(outgoingData, (void*)const_cast<char*>(data.c_str()), data.length());
   iviLink::Channel::sendBuffer(mChannelID, outgoingData, data.length());
   delete[] outgoingData;
}

CNotificationsProfile::CNotificationsProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
   : mChannelID(0)
   , mpAppCallbacks(static_cast<INotificationsProfileAPI::Callbacks*>(pCbProxy))
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

CNotificationsProfile::~CNotificationsProfile()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   if (mChannelID != 0)
   {
      //deallocating channel after deleting instance 
      deallocateChannel(mChannelID);
      mChannelID = 0;
   }
}


/**
 * Allocating channel
 */
void CNotificationsProfile::onEnable()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   iviLink::Error error = allocateChannel("CNotificationsProfile", this, mChannelID, eRealTime);
   if(error.isNoError())
   {
      LOG4CPLUS_INFO(msLogger, "Channel allocated, starting the communication...");
   }
   else
   {
      LOG4CPLUS_INFO(msLogger, "allocate Channel failed");
      killProcess();
   }
}

/**
 * Deallocating channel
 */
void CNotificationsProfile::onDisable()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   if(mChannelID != 0)
   {
      //Deallocating channel
      deallocateChannel(mChannelID);
      mChannelID = 0;
   }
}

/**
 * Callback that should be invoked when the data is received
 */
void CNotificationsProfile::onBufferReceived(const tChannelId channel, Buffer const& buffer)
{   
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    UInt8 *incomingData = buffer.getBuffer();
    int read_size = buffer.getSize();
    std::string str((char*)incomingData, read_size);
    LOG4CPLUS_TRACE(msLogger, "got string:");
    LOG4CPLUS_TRACE(msLogger, str);
    mpAppCallbacks->gotString(str);
}

/**
 * Callback that should be invoked when the channel is deleted
 */
void CNotificationsProfile::onChannelDeleted(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   mChannelID = 0;
}

/**
 * Callback that should be invoked when the connection is lost
 */
void CNotificationsProfile::onConnectionLost()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

