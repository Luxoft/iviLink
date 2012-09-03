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





#include "CMediaSourceServerProfile.hpp"

#include "framework/components/ChannelSupervisor/Tube/ChannelSupervisorTube.hpp"
#include "samples/linux/Profiles/mediaCommon/CSenderThread.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/threads/CSignalSemaphore.hpp"

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
   , mTag("CMediaSourceServerProfile")
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
      CBuffer buf = mReqQueue.front();
      mReqQueue.pop();
      delete [] buf.getBuffer();
   }
}

void CMediaSourceServerProfile::sendTrackList(std::string const& trackList)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   prepareRequest(SENDTRACKLIST, true, trackList);
}

void CMediaSourceServerProfile::trackListChanged()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   prepareRequest(TRACKLISTCHANGED, false, mTag);
}
void CMediaSourceServerProfile::recallTrackList()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   prepareRequest(RECALLTRACKLIST, false, mTag);
}

void CMediaSourceServerProfile::prepareRequest(PROCEDURES_SOURCE_IDS proc, bool has_event, std::string const& trackList)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   const UInt32 size = 4;
   CBuffer buf(new UInt8[size], size);
   bool res = true;
   res = res && buf.write((UInt16)proc);
   if (has_event)
      res = res && buf.write(trackList);
   if (res)
   {
      mpReqMutex->lock();
      mReqQueue.push(buf);
      mpReqMutex->unlock();
      mpReqSemaphore->signal();     
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger, "Unable to prepare buffer - buffer size is not enough");
      delete [] buf.getBuffer();
   }
}


void CMediaSourceServerProfile::onEnable()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   mChannelID = iviLink::Channel::allocateChannel(mTag, this);
   if (mChannelID)
   {
      LOG4CPLUS_INFO(msLogger, "Channel allocated, starting the communication...");
      mpSenderThread->start();
   }
   else
   {
      LOG4CPLUS_WARN(msLogger, "allocate Channel failed");
   }
}

void CMediaSourceServerProfile::onDisable()
{
   CError err = iviLink::Channel::deallocateChannel(mChannelID);
   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
}

//from CChannelHandler
void CMediaSourceServerProfile::bufferReceived(const iviLink::Channel::tChannelId channel, iviLink::CBuffer const& buffer)
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
   
   bool res = true;
   UInt16 tmp;
   res = res && buffer.read(tmp);
   if (!res)
   {
      LOG4CPLUS_WARN(msLogger, "Unable to read request type from buffer");
      return;
   }

   PROCEDURES_SOURCE_IDS proc = static_cast<PROCEDURES_SOURCE_IDS>(tmp);
   
   if (proc == REQUESTTRACKLIST)
   {
         LOG4CPLUS_INFO(msLogger, "case REQUESTTRACKLIST");
         mpAppCallbacks->onRequestTrackList();
   }
   else
   {
         LOG4CPLUS_WARN(msLogger, "Unknown request type");
   }
}

void CMediaSourceServerProfile::channelDeletedCallback(const UInt32 channel_id)
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

void CMediaSourceServerProfile::connectionLostCallback()
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
   CBuffer buf = mReqQueue.front();
   mReqQueue.pop();
   mpReqMutex->unlock();

   CError err = iviLink::Channel::sendBuffer(mChannelID, buf);
   if (!err.isNoError())
   {
      LOG4CPLUS_INFO(msLogger, "CMediaSourceServerProfile::handleRequest() :: Send error"
         + static_cast<std::string>(err));
   }

   delete [] buf.getBuffer();
}

