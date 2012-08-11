/* 
 * 
 * iviLINK SDK, version 1.0.1
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





#include "CSeatHUProfile.hpp"

#include "framework/components/ChannelSupervisor/Tube/ChannelSupervisorTube.hpp"
#include "samples/linux/Profiles/SeatHUProfile/CSenderThread.hpp"
#include "samples/linux/Profiles/SeatHUProfile/EReqType.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/threads/CSignalSemaphore.hpp"

#include <cstring>
#include <cassert>

Logger CSeatHUProfile::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CSeatHUProfile"));

CSeatHUProfile::CSeatHUProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
   : mChannelID(0)
   , mpAppCallbacks(static_cast<ISeatHUProfile::Callbacks*>(pCbProxy))
   , mpSenderThread(new CSenderThread(this))
   , mpReqMutex(new CMutex())
   , mpReqSemaphore(new CSignalSemaphore())
   , mBe(true)
   , mTag("SeatProfileV1")
   , mHasRequest(false)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
}

CSeatHUProfile::~CSeatHUProfile()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   mBe = false;

   mpSenderThread->disconnect();
   delete mpSenderThread;
   delete mpReqMutex;
}

//from ISeatHUProfile
void CSeatHUProfile::sendNotification(Json::Value state)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   mpReqMutex->lock();
   mHasRequest = true;
   mSendState = state;
   mpReqMutex->unlock();
   mpReqSemaphore->signal();
}

void CSeatHUProfile::onEnable()
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

void CSeatHUProfile::onDisable()
{
   CError err = iviLink::Channel::deallocateChannel(mChannelID);
   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
}

//from CChannelHandler
void CSeatHUProfile::bufferReceived(const iviLink::Channel::tChannelId channel, iviLink::CBuffer const& buffer)
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
   EReqType reqType = static_cast<EReqType>(tmp);

   switch(reqType)
   {
   case REQ_TYPE_INIT:
      {
         LOG4CPLUS_INFO(msLogger, "case REQ_TYPE_INIT");
         mpAppCallbacks->onInitRequest();
      }
      break;
   case REQ_TYPE_TAB:
      {
         LOG4CPLUS_INFO(msLogger, "case REQ_TYPE_TAB");
         res = res && buffer.read(tmp);
         if (res)
            mpAppCallbacks->onTabRequest(static_cast<EPersonEvent>(tmp));

      }
      break;
   case REQ_TYPE_HEATER:
      {
         LOG4CPLUS_INFO(msLogger, "case REQ_TYPE_HEATER");
         res = res && buffer.read(tmp);
         if (res)
            mpAppCallbacks->onHeaterRequest(static_cast<EPersonEvent>(tmp));
      }
      break;
   case REQ_TYPE_MOVE:
      {
         LOG4CPLUS_INFO(msLogger, "case REQ_TYPE_MOVE");
         res = res && buffer.read(tmp);
         if (res)
            mpAppCallbacks->onMoveRequest(static_cast<EMoveEvent>(tmp));
      }
      break;
   default:
      {
         LOG4CPLUS_WARN(msLogger, "Unknown request type");
         return;
      }
      break;
   }

   if (!res)
   {
      LOG4CPLUS_WARN(msLogger, "Unable to read event from buffer");
      return;
   }
}

void CSeatHUProfile::channelDeletedCallback(const UInt32 channel_id)
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

void CSeatHUProfile::connectionLostCallback()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CSeatHUProfile::senderLoop()
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

bool CSeatHUProfile::hasRequests()
{
   return mHasRequest;
}

void CSeatHUProfile::handleRequest()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   mpReqMutex->lock();
   Json::Value state = mSendState;
   mHasRequest = false;
   mpReqMutex->unlock();

   Json::StyledWriter writer;
   std::string data = writer.write(state);

   CError ret = iviLink::Channel::sendBuffer(mChannelID, data.c_str(), data.size() + 1);
   if(!ret.isNoError())
   {
       LOG4CPLUS_INFO(msLogger, "CSeatHUProfile::sendNotification() :: send error: "
         + static_cast<std::string>(ret));
   }
}
