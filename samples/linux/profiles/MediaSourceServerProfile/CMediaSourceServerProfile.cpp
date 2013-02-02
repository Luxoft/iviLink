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


#include "CMediaSourceServerProfile.hpp"

#include "CSenderThread.hpp"
#include "CMutex.hpp"
#include "CSignalSemaphore.hpp"
#include "Exit.hpp"

#include <cstring>
#include <cassert>
#include <iostream>

Logger CMediaSourceServerProfile::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CMediaSourceServerProfile"));

CMediaSourceServerProfile::CMediaSourceServerProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
   : mChannelID(0)
   , mpAppCallbacks(static_cast<IMediaSourceServerProfile_API::Callbacks*>(pCbProxy))
   , mpSenderThread(new CSenderThread(this))
   , mpReqMutex(new CMutex())
   , mpReqSemaphore(new CSignalSemaphore())
   , mBe(true)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
}

CMediaSourceServerProfile::~CMediaSourceServerProfile()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   mBe = false;

   mpSenderThread->disconnect();
   delete mpSenderThread;
   delete mpReqMutex;

   while (!mReqQueue.empty())
   {
      mReqQueue.pop();
   }
}

void CMediaSourceServerProfile::sendTrackList(std::string const& trackList)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   static const int packetSize = MEDIA_TRACKLIST_PACKET_SIZE;
   div_t divresult = div(trackList.size(), packetSize);
   int packetsNumber = divresult.quot + (divresult.rem == 0 ? 0 : 1);
   LOG4CPLUS_INFO(msLogger, "sendTrackList() total packets number = " + convertIntegerToString(packetsNumber));
   LOG4CPLUS_INFO(msLogger, "sendTrackList() full packets number = " + convertIntegerToString(divresult.quot));
   prepareRequest(DECLARE_TRACKLIST_PACKETS_NUMBER, true, convertIntegerToString(packetsNumber));
   int offset = 0;
   for (int i = 0; i < divresult.quot; i++) 
   {
      LOG4CPLUS_INFO(msLogger, "sendTrackList() sending packet number: " + convertIntegerToString(i));
      offset = i * packetSize;
      LOG4CPLUS_INFO(msLogger, "sendTrackList() sending packet, offset is: " + convertIntegerToString(offset));
      LOG4CPLUS_INFO(msLogger, "sendTrackList() sending packet, content is: " + trackList.substr(offset, packetSize));
      prepareRequest(SENDTRACKLIST, true, trackList.substr(offset, packetSize));
   }
   offset = packetSize * divresult.quot;
   if (divresult.rem != 0)
   {
      LOG4CPLUS_INFO(msLogger, "sendTrackList() sending last packet: " + trackList.substr(offset));
      prepareRequest(SENDTRACKLIST, true, trackList.substr(offset));
   }
}

void CMediaSourceServerProfile::trackListChanged()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   prepareRequest(TRACKLISTCHANGED, false);
}
void CMediaSourceServerProfile::recallTrackList()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   prepareRequest(RECALLTRACKLIST, false);
}

void CMediaSourceServerProfile::prepareRequest(PROCEDURES_SOURCE_IDS proc, bool has_event, std::string const& trackList)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   std::string msg;
   msg = static_cast<UInt8>(proc);
   if (has_event)
   { 
	  msg += trackList;
   }
   mpReqMutex->lock();
   mReqQueue.push(msg);
   mpReqMutex->unlock();
   mpReqSemaphore->signal();     
}


void CMediaSourceServerProfile::onEnable()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   iviLink::Error err = iviLink::Channel::allocateChannelAsServer(MEDIA_SOURCE_TAG, this, mChannelID, eRealTime);
   if (err.isNoError())
   {
      LOG4CPLUS_INFO(msLogger, "Channel allocated, starting the communication...");
      mpSenderThread->start();
   }
   else
   {
      LOG4CPLUS_WARN(msLogger, "allocate Channel failed");
      killProcess(1);
   }
}

void CMediaSourceServerProfile::onDisable()
{
   iviLink::Error err = iviLink::Channel::deallocateChannel(mChannelID);
   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
}

//from CChannelHandler
void CMediaSourceServerProfile::onBufferReceived(const iviLink::Channel::tChannelId channel, iviLink::Buffer const& buffer)
{
  LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if (mChannelID != channel)
   {
      LOG4CPLUS_ERROR(msLogger, "mChannelID != channel_id");
      return;
   }
   else
   {
      LOG4CPLUS_INFO(msLogger, "mChannelID == channel_id");
   }
   
   UInt8 *incomingData = buffer.getBuffer();
   int read_size = buffer.getSize();

   LOG4CPLUS_INFO(msLogger, "Procedure ID = " + convertIntegerToString(incomingData[0]));

   if(incomingData[0] == REQUESTTRACKLIST)
   {
         LOG4CPLUS_INFO(msLogger, "case REQUESTTRACKLIST");
         mpAppCallbacks->onRequestTrackList(true);
   }
   else
   {
        LOG4CPLUS_INFO(msLogger, "unknown procedure ID");
   }

}

void CMediaSourceServerProfile::onChannelDeleted(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   if (mChannelID == channel_id)
      mChannelID = 0;

   iviLink::Error err = iviLink::Channel::deallocateChannel(channel_id);
   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
}

void CMediaSourceServerProfile::onConnectionLost()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CMediaSourceServerProfile::senderLoop()
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
         mpReqSemaphore->wait();
      }
   }
}

bool CMediaSourceServerProfile::hasRequests()
{
   mpReqMutex->lock();
   bool res = !mReqQueue.empty();
   mpReqMutex->unlock();
   return res;
}

void CMediaSourceServerProfile::handleRequest()
{
   mpReqMutex->lock();
   std::string msg;
   msg = mReqQueue.front();
   mReqQueue.pop();
   mpReqMutex->unlock();

   iviLink::Error err = iviLink::Channel::sendBuffer(mChannelID, msg.c_str(), msg.length());
   if (!err.isNoError())
   {
      LOG4CPLUS_INFO(msLogger, "CMediaSourceServerProfile::handleRequest() :: Send error"
         + static_cast<std::string>(err));
   }
}

