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
 

#include <stdlib.h>
#include <cstring>

#include "CNaviReceiverProfileImpl.hpp"
#include "utils/json/include/json/json.h"

//namespace required for working with receiving and sending data
using namespace iviLink::Channel;

Logger CNaviReceiverProfileImpl::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CNaviReceiverProfileImpl"));

CNaviReceiverProfileImpl::CNaviReceiverProfileImpl(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
   : mChannelID(0)
   , mpAppCallbacks(static_cast<INaviReceiverProfile_API::Callbacks*>(pCbProxy))
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Navi"));
   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
}

CNaviReceiverProfileImpl::~CNaviReceiverProfileImpl()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   if (mChannelID != 0)
   {
      deallocateChannel(mChannelID);
      mChannelID = 0;
   }
}


/**
 * Allocating channel
 */
void CNaviReceiverProfileImpl::onEnable()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   //Allocating channel
   mChannelID = allocateChannel("NavigationChannel", this, eRealTime);
   if(mChannelID != 0)
   {
      LOG4CPLUS_INFO(msLogger, "CNaviReceiverProfileImpl::CNaviReceiverProfileImpl(): Channel allocated, starting the communication...");
   }
   else
   {
      LOG4CPLUS_INFO(msLogger, "CNaviReceiverProfileImpl::CNaviReceiverProfileImpl(): allocate Channel failed");
   }
}

/**
 * Deallocating channel
 */
void CNaviReceiverProfileImpl::onDisable()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   {
      //Deallocating channel
      deallocateChannel(mChannelID);
   }
}

/**
 * Callback that should be invoked when the data is received
 */
void CNaviReceiverProfileImpl::onBufferReceived(const tChannelId channel, CBuffer const& buffer)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   UInt8 *incomingData = buffer.getBuffer();
   int read_size = buffer.getSize();
   std::string json((char*)incomingData, read_size);
   LOG4CPLUS_TRACE(msLogger, "got string:");
   LOG4CPLUS_TRACE(msLogger, json);
   Json::Value root; // will contain the root value after parsing.
   Json::Reader reader;
   bool parsingSuccessful = reader.parse(json.c_str(), root);
   if (parsingSuccessful) {
      std::deque<std::string> tracks;
      Json::Value list=root["play"];
      Json::Value::UInt index=0;
      LOG4CPLUS_TRACE(msLogger, "tracks to play:");
      for(index=0;index<list.size();index++){
         LOG4CPLUS_TRACE(msLogger, list[index].asString());
         tracks.push_back(list[index].asString());
      }
      mpAppCallbacks->playTracks(tracks);
   } else {
      LOG4CPLUS_ERROR(msLogger, "Failed to parse tracklist!");
   }
}

/**
 * Callback that should be invoked when the channel is deleted
 */
void CNaviReceiverProfileImpl::onChannelDeleted(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   //Deallocating channel
   deallocateChannel(mChannelID);
   mChannelID = 0;
}

/**
 * Callback that should be invoked when the connection is lost
 */
void CNaviReceiverProfileImpl::onConnectionLost()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

