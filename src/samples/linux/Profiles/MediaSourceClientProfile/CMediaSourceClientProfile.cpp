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
 

#include "CMediaSourceClientProfile.hpp"

#include "ChannelSupervisorTube.hpp"
#include "CSenderThread.hpp"
#include "CMutex.hpp"
#include "CSignalSemaphore.hpp"
#include "Exit.hpp"


#include <cstring>
#include <cassert>
#include <iostream>

Logger CMediaSourceClientProfile::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CMediaSourceClientProfile"));

CMediaSourceClientProfile::CMediaSourceClientProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
   : mChannelID(0)
   , mpAppCallbacks(static_cast<IMediaSourceClientProfile_API::Callbacks*>(pCbProxy))
   , mpSenderThread(new CSenderThread(this))
   , mpReqMutex(new CMutex())
   , mpReqSemaphore(new CSignalSemaphore())
   , mBe(true)
   , mHasRequest(false)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
}

CMediaSourceClientProfile::~CMediaSourceClientProfile()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   mBe = false;

   mpSenderThread->disconnect();
   delete mpSenderThread;
   delete mpReqMutex;
}

void CMediaSourceClientProfile::requestTrackList()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   mpReqMutex->lock();
   mHasRequest = true;
   mpReqMutex->unlock();
   mpReqSemaphore->signal();     

}

void CMediaSourceClientProfile::onEnable()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   mChannelID = iviLink::Channel::allocateChannelAsClient(MEDIA_SOURCE_TAG, this, eRealTime);
   if (mChannelID)
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

void CMediaSourceClientProfile::onDisable()
{
   CError err = iviLink::Channel::deallocateChannel(mChannelID);
   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
}

//from CChannelHandler
void CMediaSourceClientProfile::onBufferReceived(const iviLink::Channel::tChannelId channel, iviLink::CBuffer const& buffer)
{
  LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if (mChannelID != channel)
   {
      LOG4CPLUS_INFO(msLogger, "mChannelID != channel_id");
      return;
   }
   else
   {
      LOG4CPLUS_INFO(msLogger, "mChannelID == channel_id");
   }
   
   UInt8 *incomingData = buffer.getBuffer();
   int read_size = buffer.getSize();

   LOG4CPLUS_INFO(msLogger, "Procedure ID = " + convertIntegerToString(incomingData[0]));


   if(incomingData[0] == RECALLTRACKLIST)
   {
         LOG4CPLUS_INFO(msLogger, "case RECALLTRACKLIST");
         mpAppCallbacks->onRecallTrackList();

   }
   else if(incomingData[0] == TRACKLISTCHANGED)
   {
         LOG4CPLUS_INFO(msLogger, "case TRACKLISTCHANGED");        
         mpAppCallbacks->onTrackListChanged();

   }
   else if(incomingData[0] == SENDTRACKLIST)
   {
        LOG4CPLUS_INFO(msLogger, "case TRACKLISTRECIEVED");
        std::string trackList((char*)(incomingData + 1), read_size - 1);
        LOG4CPLUS_INFO(msLogger, "TrackList  = " + trackList);
        mpAppCallbacks->onTrackListReceived(trackList);
   }
   else
   {
        LOG4CPLUS_INFO(msLogger, "unknown procedure ID");
   }
}

void CMediaSourceClientProfile::onChannelDeleted(const UInt32 channel_id)
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

void CMediaSourceClientProfile::onConnectionLost()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   mpAppCallbacks->onRecallTrackList();
}

void CMediaSourceClientProfile::senderLoop()
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

bool CMediaSourceClientProfile::hasRequests()
{
   return mHasRequest;
}

void CMediaSourceClientProfile::handleRequest()
{
   mpReqMutex->lock();
   mHasRequest = false;
   mpReqMutex->unlock();

   bool res = true;
   const UInt32 size = 4;
   CBuffer buf(new UInt8[size], size);
   res = res && buf.write((UInt8)REQUESTTRACKLIST);

   if (res)
   {
       CError err = iviLink::Channel::sendBuffer(mChannelID, buf);
       if (!err.isNoError())
       {
          LOG4CPLUS_INFO(msLogger, "CMediaSourceClientProfile::handleRequest() :: Send error"
             + static_cast<std::string>(err));
       }

   else
   {
      LOG4CPLUS_ERROR(msLogger, "Unable to prepare buffer - buffer size is not enough");
   }
   delete [] buf.getBuffer();
}
}
