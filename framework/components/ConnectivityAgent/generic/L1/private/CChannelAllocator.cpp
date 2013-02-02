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


#include "CChannelAllocator.hpp"

#include "CSourceAgent.hpp"
#include "CTargetAgent.hpp"
#include "L1InterfaceStub.hpp"
#include "Logger.hpp"
#include "L0Interface.hpp"

using namespace iviLink::ConnectivityAgent::L1;
using namespace iviLink::ConnectivityAgent;

CMutex CChannelAllocator::mSingletonMutex;
CChannelAllocator* CChannelAllocator::mSelf = 0;


using iviLink::ConnectivityAgent::L0::L0Interface;

Logger CChannelAllocator::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L1.CChannelAllocator"));

CChannelAllocator::CChannelAllocator():
mpL0Interface(0)
{}

CChannelAllocator* CChannelAllocator::getInstance()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   mSingletonMutex.lock();
   if (mSelf == NULL)
   {
      mSelf = new CChannelAllocator();
   }
   mSingletonMutex.unlock();
   return mSelf;
}

void CChannelAllocator::deleteInstance()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   delete this;
   mSelf = NULL;
}

void CChannelAllocator::setL0Interface(L0Interface *pL0Interface)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   mpL0Interface = pL0Interface;
}

void CChannelAllocator::OnConnected()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   L1InterfaceStub::getInstance()->OnConnected();
}

void CChannelAllocator::OnDisconnected()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   L1InterfaceStub::getInstance()->OnDisconnected();
}

ConnectivityAgentError CChannelAllocator::allocateChannel(const TChannelPriority prio, const UInt32 channel_id, L1InterfaceStub::tL1ChannelInfo& channelInfo)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   ConnectivityAgentError result;

   if (mpL0Interface)
   {
      iviLink::ConnectivityAgent::L1::CSourceAgent* pSourceAgent = new CSourceAgent(channel_id);
      result = mpL0Interface->openTransmissionChannel(channel_id, prio, *pSourceAgent,
            channelInfo.mUpperThresholdTime, channelInfo.mLowerThresholdTime);
      if (!result.isNoError())
      {
         delete pSourceAgent;
         return result;
      }
      channelInfo.mpSourceAgent = pSourceAgent;
      iviLink::ConnectivityAgent::L1::CTargetAgent* pTargetAgent = new CTargetAgent(channel_id);
      channelInfo.mpTargetAgent = pTargetAgent;
      result = mpL0Interface->openIncomingChannel(channel_id, *pTargetAgent);
      if (!result.isNoError())
      {         
         LOG4CPLUS_WARN(logger, "Failed to openIncomingChannel for channel " + convertIntegerToString(channel_id));
         delete pTargetAgent;
         mpL0Interface->closeTransmissionChannel(channel_id);  // pSourceAgent is deleted here
      }
   }
   return result;
}

ConnectivityAgentError CChannelAllocator::deallocateChannel(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   ConnectivityAgentError result;

   if(mpL0Interface)
   {
      result = mpL0Interface->closeTransmissionChannel(channel_id);
      if (!result.isNoError())
      {
         LOG4CPLUS_WARN(logger, "Failed to closeTransmissionChannel, will try to closeIncomingChannel anyway");
      }
      // to avoid missing on an error when transmission close returns error and incoming returns no error
      ConnectivityAgentError incomingError = mpL0Interface->closeIncomingChannel(channel_id);
      if (!incomingError.isNoError())
      {
         LOG4CPLUS_WARN(logger, "Failed to closeIncomingChannel");
         result.changeErrorCode(incomingError.getCode());
      }
   }
   return result;
}


UInt64 CChannelAllocator::getQoSReportForChannel(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   return mpL0Interface->getQoSReport(channel_id);
}

void CChannelAllocator::narrowBandwidth(const UInt32 channel_id)
{
}

void CChannelAllocator::higherBandwidth(const UInt32 channel_id)
{
}

CChannelAllocator::~CChannelAllocator()
{
   mpL0Interface = 0;

}
