/* 
 * 
 * iviLINK SDK, version 1.0
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
#include "framework/components/ConnectivityAgent/generic/HAL/Frame.hpp"
#include "framework/components/ConnectivityAgent/generic/HAL/CCarrierAdapter.hpp"
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
#include "utils/misc/Logger.hpp"

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
   mListMutex.lock();
   ERROR_CODE res = ERR_FAIL;
   LOG4CPLUS_INFO(logger, "CIncomeDispatcher::openChannel(chID = "
         + convertIntegerToString(channel_id) + ", bufCons = "
         + convertIntegerToString((intptr_t)&bufferConsumer) + ")");
   TTargetList::iterator iter = mTargetList.begin();
   while(iter != mTargetList.end())
   {
      if (channel_id == static_cast<CTarget*>(*iter)->getChannelID())
      {
         LOG4CPLUS_INFO(logger, "CIncomeDispatcher::openChannel: channel "
            + convertIntegerToString(channel_id) + " already exists");
         break;
      }
      ++iter;
   }
   if (iter == mTargetList.end())
   {
      CTarget* pTarget = new CTarget(channel_id);
      pTarget->registerConsumer(&bufferConsumer);
      bufferConsumer.registerProducer(pTarget);
      mTargetList.push_back(pTarget);
      LOG4CPLUS_INFO(logger, "CIncomeDispatcher::openChannel: opened channel "
         + convertIntegerToString(channel_id));
      res = ERR_OK;
   }
   mListMutex.unlock();
   return res;
}

void CIncomeDispatcher::closeChannel(const UInt32 channel_id)
{
   LOG4CPLUS_INFO(logger, "CIncomeDispatcher::closeChannel(chID = "
         + convertIntegerToString(channel_id));
   mListMutex.lock();
   for (TTargetList::iterator iter = mTargetList.begin(); iter != mTargetList.end(); ++iter)
   {
      if (channel_id == static_cast<CTarget*> (*iter)->getChannelID())
      {
         delete *iter;
         mTargetList.erase(iter);
         break;
      }

   }
   mListMutex.unlock();
}

CIncomeDispatcher::~CIncomeDispatcher()
{  
   mListMutex.lock();

   mDestroyed = true;

   for (TTargetList::iterator iter = mTargetList.begin(); iter != mTargetList.end(); ++iter)
   {
      delete *iter;
   }

   mTargetList.clear();

   mListMutex.unlock();
}

ERROR_CODE CIncomeDispatcher::receiveFrame(const iviLink::ConnectivityAgent::HAL::Frame& frame)
{
   ERROR_CODE ret = ERR_FAIL;
   LOG4CPLUS_INFO(logger, "CIncomeDispatcher::receiveFrame ChID "
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
      mListMutex.lock();

      if (mDestroyed)
      {
         return ERR_FAIL;
      }

      CTarget* pTarget = NULL;
      LOG4CPLUS_INFO(logger, "CIncomeDispatcher::receiveFrame mTargetList.size() = "
            + convertIntegerToString(mTargetList.size()));
      for (TTargetList::iterator iter = mTargetList.begin(); iter != mTargetList.end(); ++iter)
      {
         LOG4CPLUS_INFO(logger, "CIncomeDispatcher::receiveFrame check target chID = "
               + convertIntegerToString((*iter)->getChannelID()));
         if (frame.mFrameHeader.channel_id == (*iter)->getChannelID())
         {
            pTarget = *iter;
            break;
         }
      }
      mListMutex.unlock();

      if (pTarget)
      {
         LOG4CPLUS_INFO(logger, "CIncomeDispatcher::receiveFrame target found");
         ret = pTarget->receiveFrame(frame);
      }
      else
      {
         ret = ERR_NOTFOUND;
         LOG4CPLUS_INFO(logger, "CIncomeDispatcher::receiveFrame error - frame.channel_id = "
              + convertIntegerToString(frame.mFrameHeader.channel_id) + " not found");
      }

   }
   return ret;
}
void CIncomeDispatcher::replaceCarrier(iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pCarrierAdapter)
{
   mpCarrier = pCarrierAdapter;
   TTargetList::iterator iter = mTargetList.begin();
   while(iter != mTargetList.end())
   {
      static_cast<CTarget*>(*iter)->setCarrierReplaced();
      ++iter;
   }
}
