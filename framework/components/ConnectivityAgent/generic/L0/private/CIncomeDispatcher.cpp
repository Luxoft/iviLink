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


#include <time.h>
#include <unistd.h>
#include <algorithm>

#include "CTransmittedFramesQueue.hpp"
#include "CSource.hpp"
#include "CTarget.hpp"
#include "Frame.hpp"
#include "CCarrierAdapter.hpp"
#include "CIncomeDispatcher.hpp"
#include "Logger.hpp"

using namespace iviLink::ConnectivityAgent::L0;
using namespace iviLink::ConnectivityAgent;

Logger CIncomeDispatcher::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L0.CIncomeDispatcher"));

CIncomeDispatcher::CIncomeDispatcher(iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pCarrierAdaptor, CTransmittedFramesQueue* pTransmittedFrameQueue):
   mpCarrier(pCarrierAdaptor),
   mpTransmittedFrameQueue(pTransmittedFrameQueue),
   mDestroyed(false)
{
   pCarrierAdaptor->registerFrameReceiver(*this);
}

ConnectivityAgentError CIncomeDispatcher::openChannel(const UInt32 channel_id, IBufferConsumer& bufferConsumer)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   MutexLocker lock(mMapMutex);
   ConnectivityAgentError result;
   LOG4CPLUS_INFO(logger, "CIncomeDispatcher::openChannel(chID = "
         + convertIntegerToString(channel_id) + ", bufCons = "
         + convertIntegerToString((intptr_t)&bufferConsumer) + ")");
   TTargetMap::iterator iter = mTargetMap.find(channel_id);
   if (iter != mTargetMap.end())
   {
      if (channel_id == (static_cast<CTarget*>((*iter).second))->getChannelID())
      {
         LOG4CPLUS_ERROR(logger, "CIncomeDispatcher::openChannel: channel "
            + convertIntegerToString(channel_id) + " already exists");
         result.setErrorCode(ConnectivityAgentError::ERROR_CHANNEL_BUSY);
      }
   }
   else
   {
      CTarget* pTarget = new CTarget(channel_id);
      pTarget->registerConsumer(&bufferConsumer);
      bufferConsumer.registerProducer(pTarget);
      mTargetMap[channel_id] = pTarget;
      LOG4CPLUS_INFO(logger, "CIncomeDispatcher::openChannel: opened channel "
         + convertIntegerToString(channel_id));
      result.setNoError();
   }
   return result;
}

ConnectivityAgentError CIncomeDispatcher::closeChannel(const UInt32 channel_id)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(logger, "CIncomeDispatcher::closeChannel(chID = "
         + convertIntegerToString(channel_id));
   MutexLocker lock(mMapMutex);
   TTargetMap::iterator iter = mTargetMap.find(channel_id);
   if (iter != mTargetMap.end())
   {
      if (channel_id == static_cast<CTarget*> ((*iter).second)->getChannelID())
      {
         LOG4CPLUS_INFO(logger, "CIncomeDispatcher::closeChannel(chID = "
         + convertIntegerToString(channel_id) + ": found CTarget*)");
         delete (*iter).second;
         mTargetMap.erase(iter);
         return ConnectivityAgentError::NoError();
      }
   }
   return ConnectivityAgentError(ConnectivityAgentError::ERROR_NOT_FOUND);
}

CIncomeDispatcher::~CIncomeDispatcher()
{  
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   MutexLocker lock(mMapMutex);
   mDestroyed = true;
   for (TTargetMap::iterator iter = mTargetMap.begin(); iter != mTargetMap.end(); ++iter)
   {
      delete (*iter).second;
   }
   mTargetMap.clear();
}

ConnectivityAgentError CIncomeDispatcher::receiveFrame(const iviLink::ConnectivityAgent::HAL::Frame& frame)
{
   ConnectivityAgentError result;
   LOG4CPLUS_TRACE(logger, "CIncomeDispatcher::receiveFrame ChID "
         + convertIntegerToString(frame.mFrameHeader.channel_id) + ", number "
         + convertIntegerToString(frame.mFrameHeader.number) +   ", size "
         + convertIntegerToString(frame.getSize()));
   if (frame.isACK())
   {
      LOG4CPLUS_INFO(logger, "CIncomeDispatcher::receiveFrame(): frame is ACK, channel_id = "
               + convertIntegerToString(frame.mFrameHeader.channel_id) + ", number = "
               + convertIntegerToString(frame.mFrameHeader.number));
      mpTransmittedFrameQueue->dequeue(frame.mFrameHeader.channel_id, frame.mFrameHeader.number );
      result.setNoError();
   }
   else
   {
      if (mDestroyed)
      {
         return result;
      }

      CTarget* pTarget = NULL;
      {
         MutexLocker lock(mMapMutex);
         LOG4CPLUS_INFO(logger, "CIncomeDispatcher::receiveFrame mTargetMap.size() = "
               + convertIntegerToString(mTargetMap.size()));
         TTargetMap::iterator iter = mTargetMap.find(frame.mFrameHeader.channel_id);
         if (iter != mTargetMap.end())
         {
            pTarget = (*iter).second;
         }
      }

      if (pTarget)
      {
         LOG4CPLUS_INFO(logger, "CIncomeDispatcher::receiveFrame target found");
         result = pTarget->receiveFrame(frame);
      }
      else
      {
         result = ConnectivityAgentError(ConnectivityAgentError::ERROR_NOT_FOUND);
         LOG4CPLUS_ERROR(logger, "CIncomeDispatcher::receiveFrame error - frame.channel_id = "
              + convertIntegerToString(frame.mFrameHeader.channel_id) + " not found");
      }

   }
   return result;
}
void CIncomeDispatcher::replaceCarrier(iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pCarrierAdapter)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   mpCarrier = pCarrierAdapter;
   TTargetMap::iterator iter = mTargetMap.begin();
   while(iter != mTargetMap.end())
   {
      static_cast<CTarget*>((*iter).second)->setCarrierReplaced();
      ++iter;
   }
}
