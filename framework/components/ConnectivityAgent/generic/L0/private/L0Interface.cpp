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


#include <map>
#include <unistd.h>

#include "CIncomeDispatcher.hpp"
#include "CQoS.hpp"
#include "CTransmitDispatcher.hpp"
#include "L0Interface.hpp"

using namespace iviLink::ConnectivityAgent::L0;
using namespace iviLink::ConnectivityAgent;
Logger L0Interface::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L0.L0Interface"));

L0Interface::L0Interface(CTransmitDispatcher *pTransmitDispatcher, CIncomeDispatcher* pIncomeDispatcher, CQoS* pQoS) :
   mpTransmitDispatcher(pTransmitDispatcher), 
   mpIncomeDispatcher(pIncomeDispatcher), 
   mpQoS(pQoS)
{
}

L0Interface::~L0Interface()
{
   mpTransmitDispatcher = 0; 
   mpIncomeDispatcher = 0;
   mpQoS = 0;
}

ConnectivityAgentError L0Interface::openTransmissionChannel(const UInt32 channel_id, const UInt8 priority,
      IBufferProducer& bufferProducer, UInt64 upper_threshold, UInt64 lower_threshold)
{
   LOG4CPLUS_TRACE(logger, "L0Interface::openTransmissionChannel(chID = "
         + convertIntegerToString(channel_id) + ", prio = "
         + convertIntegerToString(priority) + ", bufProd = "
         + convertIntegerToString((intptr_t)&bufferProducer) + ", up_tres = "
         + convertIntegerToString(upper_threshold) + ", lo_tres = "
         + convertIntegerToString(lower_threshold));

   ConnectivityAgentError result;

   if (mpTransmitDispatcher && mpQoS)
   {
      result = mpTransmitDispatcher->openChannel(channel_id, static_cast<TChannelPriority> (priority), bufferProducer);
      if (!result.isNoError())
      {
         return result;
      }
      result = mpQoS->openChannel(channel_id, upper_threshold, lower_threshold);
      if (!result.isNoError())
      {
         return result;
      }
      result.setNoError();
      return result;
   }
   else 
   {
      return result;
   }
}

ConnectivityAgentError L0Interface::openIncomingChannel(const UInt32 channel_id,
      IBufferConsumer& bufferConsumer)
{
   LOG4CPLUS_TRACE(logger, "L0Interface::openIncomingChannel(chID = "
         + convertIntegerToString(channel_id) + ", bufCons = "
         + convertIntegerToString((intptr_t)&bufferConsumer));

   if (mpIncomeDispatcher)
   {
      return mpIncomeDispatcher->openChannel(channel_id, bufferConsumer);
   }
   return ConnectivityAgentError(ConnectivityAgentError::ERROR_OTHER);
}

ConnectivityAgentError L0Interface::closeTransmissionChannel(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE(logger, "L0Interface::closeTransmissionChannel(chID = " + convertIntegerToString(channel_id) + ")");

   ConnectivityAgentError result;

   if (mpTransmitDispatcher)
   {
      result = mpTransmitDispatcher->closeChannel(channel_id);
      if (!result.isNoError())
      {
         return result;
      }
   }
   else 
   {
      result.setErrorCode(ConnectivityAgentError::ERROR_OTHER);
      return result;
   }
   if (mpQoS)
   {
      result = mpQoS->closeChannel(channel_id);
      if (!result.isNoError())
      {
         return result;
      }
   }
   else 
   {
      result.setErrorCode(ConnectivityAgentError::ERROR_OTHER);
      return result;
   }
}

ConnectivityAgentError L0Interface::closeIncomingChannel(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE(logger, "L0Interface::closeIncomingChannel(chID = %u" + convertIntegerToString(channel_id) + ")");
   if (mpIncomeDispatcher)
   {
      return mpIncomeDispatcher->closeChannel(channel_id);
   }
   else
   {
      return ConnectivityAgentError(ConnectivityAgentError::ERROR_OTHER);
   }
}

UInt64 L0Interface::getQoSReport(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   UInt64 retVal = 0;
   ConnectivityAgentError qosError = mpQoS->getAverageForChannel(channel_id, retVal);
   if (!qosError.isNoError()) 
   {
      LOG4CPLUS_WARN(logger, "Could not find info for channel: " + convertIntegerToString(channel_id));
   }
   return retVal;
}
