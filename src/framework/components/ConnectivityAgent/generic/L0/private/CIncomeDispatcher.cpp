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
 

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <time.h>
#include <unistd.h>
#include <algorithm>

/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "CTransmittedFramesQueue.hpp"
#include "CSource.hpp"
#include "CTarget.hpp"
#include "Frame.hpp"
#include "CCarrierAdapter.hpp"
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
 #include "CIncomeDispatcher.hpp"
/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "Logger.hpp"

using namespace iviLink::ConnectivityAgent::L0;

Logger CIncomeDispatcher::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L0.CIncomeDispatcher"));

CIncomeDispatcher::CIncomeDispatcher(iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pCarrierAdaptor, CTransmittedFramesQueue* pTransmittedFrameQueue):
   mpCarrier(pCarrierAdaptor),
   mpTransmittedFrameQueue(pTransmittedFrameQueue),
   mDestroyed(false)
{
   pCarrierAdaptor->registerFrameReceiver(*this);
}

ERROR_CODE CIncomeDispatcher::openChannel(const UInt32 channel_id, IBufferConsumer& bufferConsumer)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   MutexLocker lock(mMapMutex);
   ERROR_CODE res = ERR_FAIL;
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
      res = ERR_OK;
   }
   return res;
}

void CIncomeDispatcher::closeChannel(const UInt32 channel_id)
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
      }
   }
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

ERROR_CODE CIncomeDispatcher::receiveFrame(const iviLink::ConnectivityAgent::HAL::Frame& frame)
{
   ERROR_CODE ret = ERR_FAIL;
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
      ret = ERR_OK;
   }
   else
   {
      if (mDestroyed)
      {
         return ERR_FAIL;
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
         ret = pTarget->receiveFrame(frame);
      }
      else
      {
         ret = ERR_NOTFOUND;
         LOG4CPLUS_ERROR(logger, "CIncomeDispatcher::receiveFrame error - frame.channel_id = "
              + convertIntegerToString(frame.mFrameHeader.channel_id) + " not found");
      }

   }
   return ret;
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
