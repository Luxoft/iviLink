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

#include "CScreenSharingProfileImpl.hpp"
#include "Exit.hpp"

using namespace iviLink::Channel;

Logger CScreenSharingProfileImpl::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CScreenSharingProfileImpl"));

void CScreenSharingProfileImpl::sendData(const std::string &data)
{   
   LOG4CPLUS_INFO(msLogger, "CScreenSharingProfileImpl::sendData() : " + data);

   UInt8 outgoingData[data.size() + sizeof(UInt32) ];
   Buffer buffer(outgoingData, data.size() + sizeof(UInt32) );
   if (!buffer.write(data) )
   {
      LOG4CPLUS_WARN(msLogger, "Error writing data to buffer");
   }
   iviLink::Error err = iviLink::Channel::sendBuffer(mChannelID, buffer);
   if (!err.isNoError())
      LOG4CPLUS_WARN(msLogger, "Unable to sendBuffer: " + static_cast<std::string>(err));
}

CScreenSharingProfileImpl::CScreenSharingProfileImpl(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
   : mChannelID(0)
   , mpAppCallbacks(static_cast<IScreenSharingProfile_API::Callbacks*>(pCbProxy))
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("samples.ScreenSharing"));

   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

   LOG4CPLUS_INFO(msLogger, "CScreenSharingProfileImpl::CScreenSharingProfileImpl");
}

CScreenSharingProfileImpl::~CScreenSharingProfileImpl()
{
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
void CScreenSharingProfileImpl::onEnable()
{
   //Allocating channel
   iviLink::Error err = allocateChannel("CScreenSharing", this, mChannelID, eRealTime);
   if(err.isNoError())
   {
      LOG4CPLUS_INFO(msLogger, "CScreenSharingProfileImpl::CScreenSharingProfileImpl(): Channel allocated, starting the communication...");
   }
   else
   {
      LOG4CPLUS_INFO(msLogger, "CScreenSharingProfileImpl::CScreenSharingProfileImpl(): allocate Channel failed");
      killProcess();
   }
}

/**
 * Deallocating channel
 */
void CScreenSharingProfileImpl::onDisable()
{
   {
      //Deallocating channel
      deallocateChannel(mChannelID);
   }
}

/**
 * Callback that should be invoked when the data is received
 */
void CScreenSharingProfileImpl::onBufferReceived(const tChannelId channel, Buffer const& buffer)
{
   LOG4CPLUS_INFO(msLogger, "============= CScreenSharingProfileImpl::bufferReceived");
   std::string data;
   if ( !buffer.read(data) )
   {
      LOG4CPLUS_WARN(msLogger, "Error reading data from buffer");
   }

   LOG4CPLUS_INFO(msLogger, "buffer data : " + data);

   mpAppCallbacks->dataReceived(data);
}

/**
 * Callback that should be invoked when the channel is deleted
 */
void CScreenSharingProfileImpl::onChannelDeleted(const UInt32 channel_id)
{
   LOG4CPLUS_INFO(msLogger, "CScreenSharingProfileImpl::channelDeletedCallback() - " +
         convertIntegerToString(channel_id) + " channel deleted!");
   //Deallocating channel
   deallocateChannel(mChannelID);
   mChannelID = 0;
}

/**
 * Callback that should be invoked when the connection is lost
 */
void CScreenSharingProfileImpl::onConnectionLost()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

