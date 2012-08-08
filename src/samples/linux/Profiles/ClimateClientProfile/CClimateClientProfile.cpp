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





#include "CClimateClientProfile.hpp"

#include "framework/components/ChannelSupervisor/Tube/ChannelSupervisorTube.hpp"
#include "samples/linux/Profiles/ClimateClientProfile/CSenderThread.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/threads/CSignalSemaphore.hpp"

#include <cstring>
#include <cassert>

Logger CClimateClientProfile::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Profiles.CClimateClientProfile"));

CClimateClientProfile::CClimateClientProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy)
   : mChannelID(0)
   , mpAppCallbacks(static_cast<IClimateClientProfile::Callbacks*>(pCbProxy))
   , mpSenderThread(new CSenderThread(this))
   , mpReqMutex(new CMutex())
   , mpReqSemaphore(new CSignalSemaphore())
   , mBe(true)
   , mTag("ClimateProfileV1")
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
}

CClimateClientProfile::~CClimateClientProfile()
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

void CClimateClientProfile::tempRequest(ETempEvent event)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   prepareRequest(REQ_TYPE_TEMP, true, event);
}

void CClimateClientProfile::fanRequest(EFanEvent event)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   prepareRequest(REQ_TYPE_FAN, true, event);
}

void CClimateClientProfile::heaterRequest(EHeaterEvent event)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   prepareRequest(REQ_TYPE_HEATER, true, event);
}

void CClimateClientProfile::generalRequest(EGeneralRequest event)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   prepareRequest(REQ_TYPE_GENERAL, true, event);
}
void CClimateClientProfile::initRequest()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   prepareRequest(REQ_TYPE_INIT, false, 0);
}

void CClimateClientProfile::prepareRequest(EReqType regType, bool has_event, UInt16 event)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   const UInt32 size = 4;
   CBuffer buf(new UInt8[size], size);
   bool res = true;
   res = res && buf.write((UInt16)regType);
   if (has_event)
      res = res && buf.write(event);
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

void CClimateClientProfile::onEnable()
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

void CClimateClientProfile::onDisable()
{
   CError err = iviLink::Channel::deallocateChannel(mChannelID);
   if (!err.isNoError())
   {
      LOG4CPLUS_WARN(msLogger, "Unable to deallocate channel: " + static_cast<std::string>(err));
   }
}

//from CChannelHandler
void CClimateClientProfile::bufferReceived(const iviLink::Channel::tChannelId channel, iviLink::CBuffer const& buffer)
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

   Json::Value value; // will contain the root value after parsing.
   Json::Reader reader;

   bool parsingSuccessful = reader.parse((char*)buffer.getBuffer(), value);
   if (!parsingSuccessful)
   {
     LOG4CPLUS_INFO(msLogger, "Failed to parse income message");
   }
   else
   {
      LOG4CPLUS_INFO(msLogger, "Incoming notification");
      mpAppCallbacks->incomingNotification(value);
   }
}

void CClimateClientProfile::channelDeletedCallback(const UInt32 channel_id)
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

void CClimateClientProfile::connectionLostCallback()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CClimateClientProfile::senderLoop()
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

bool CClimateClientProfile::hasRequests()
{
   mpReqMutex->lock();
   bool res = !mReqQueue.empty();
   mpReqMutex->unlock();
   return res;
}

void CClimateClientProfile::handleRequest()
{
   mpReqMutex->lock();
   CBuffer buf = mReqQueue.front();
   mReqQueue.pop();
   mpReqMutex->unlock();

   CError err = iviLink::Channel::sendBuffer(mChannelID, buf);
   if (!err.isNoError())
   {
      LOG4CPLUS_INFO(msLogger, "CClimateClientProfile::handleRequest() :: Send error"
         + static_cast<std::string>(err));
   }

   delete [] buf.getBuffer();
}
