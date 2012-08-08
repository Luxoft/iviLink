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








/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <time.h>
#include <unistd.h>
#include <assert.h>
/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "CQoS.hpp"
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CTransmittedFramesQueue.hpp"
#include "utils/misc/Logger.hpp"

using namespace iviLink::ConnectivityAgent::L0;

Logger CTransmittedFramesQueue::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L0.CTransmittedFramesQueue"));

void CTransmittedFramesQueue::enqueue(iviLink::ConnectivityAgent::HAL::Frame& frame)
{
   tFrameTimeStamped frameTS;
   frameTS.mFrame = frame;
   LOG4CPLUS_INFO(logger, "CTransmittedFramesQueue::enqueue() ChID "
           + convertIntegerToString(frame.mFrameHeader.channel_id) + ", num "
           + convertIntegerToString(frame.mFrameHeader.number) + ", size "
           + convertIntegerToString((int)frame.mFrameHeader.sizeAndFlags) + ", qsize = "
           + convertIntegerToString((int)mQueue.size()));

   clock_gettime(CLOCK_MONOTONIC, &frameTS.mStartTime);
   mQueue.push_back(frameTS);

   LOG4CPLUS_INFO(logger, "CTransmittedFramesQueue::enqueue() qsize = " + convertIntegerToString((int)mQueue.size()));
}

void CTransmittedFramesQueue::dequeue(UInt32 channel_id, UInt32 number)
{
   mMutex.lock();
   LOG4CPLUS_INFO(logger, "CTransmittedFramesQueue::dequeue(chID = "
         + convertIntegerToString(channel_id) + ", num = "
         + convertIntegerToString(number) + ") qsize = "
         + convertIntegerToString((int)mQueue.size()));

   timespec end_time;
   clock_gettime(CLOCK_MONOTONIC, &end_time);

   bool found = false;
   for (TTransmittedFrameQueue::iterator iter = mQueue.begin(); iter != mQueue.end(); ++iter)
   {
      if (((*iter).mFrame.mFrameHeader.channel_id == channel_id) && ((*iter).mFrame.mFrameHeader.number == number))
      {
         UInt64 time = getTimePeriodInNanoSec((*iter).mStartTime, end_time);
         mQueue.erase(iter);
         LOG4CPLUS_INFO(logger, "CTransmittedFramesQueue::dequeue() qsize = "
               + convertIntegerToString((int)mQueue.size()));
         mpQoS->setTime(channel_id, time);
         found = true;
         break;
      }
   }

   if (!found)
   {
      LOG4CPLUS_WARN(logger, "CTransmittedFramesQueue::dequeue(chID = "
            + convertIntegerToString(channel_id) + "%u, number = "
            + convertIntegerToString(number) + ") info is not found!!!!!");
   }

   mMutex.unlock();
}

void CTransmittedFramesQueue::deleteFramesForChannel(UInt32 channel_id)
{
   mMutex.lock();
   LOG4CPLUS_INFO(logger, "CTransmittedFramesQueue::deleteFramesForChannel(chID = " + convertIntegerToString(channel_id));

   mQueue.remove_if(RemoveByChannelIDPredicate(channel_id));

   mMutex.unlock();
}

iviLink::ConnectivityAgent::HAL::Frame* CTransmittedFramesQueue::getFrameToRetransmit()
{
   mMutex.lock();
   if (!mQueue.empty())
   {
      timespec cur_time;
      clock_gettime(CLOCK_MONOTONIC, &cur_time);

      tFrameTimeStamped& fts = mQueue.front();

      UInt64 time = getTimePeriodInNanoSec(fts.mStartTime, cur_time);
      if (time > RETRANSMIT_TIME_NSEC)
      {
         // We must unlock mMutex in reenqueueRetransmittedFrame
         return &fts.mFrame;
      }
   }
   mMutex.unlock();

   return NULL;
}

void CTransmittedFramesQueue::reenqueueRetransmittedFrame(iviLink::ConnectivityAgent::HAL::Frame* pFrame)
{
   assert(!mQueue.empty());
   tFrameTimeStamped& fts = mQueue.front();
   assert(&fts.mFrame == pFrame);
   assert(fts.mFrame.mFrameHeader.channel_id == pFrame->mFrameHeader.channel_id);
   assert(fts.mFrame.mFrameHeader.number == pFrame->mFrameHeader.number);

   clock_gettime(CLOCK_MONOTONIC, &(fts.mStartTime));

   LOG4CPLUS_INFO(logger, "CTransmittedFramesQueue::reenqueueRetransmittedFrame frame "
         + convertIntegerToString((intptr_t)pFrame) + " (chID = "
         + convertIntegerToString(pFrame->mFrameHeader.channel_id) + ", num = "
         + convertIntegerToString(pFrame->mFrameHeader.number) + ") retransmitted");

   mQueue.push_back(tFrameTimeStamped(fts));
   mQueue.pop_front();

   mMutex.unlock();
}

CTransmittedFramesQueue::~CTransmittedFramesQueue()
{
   mMutex.lock();
   while (!mQueue.empty())
   {
      TTransmittedFrameQueue::iterator iter = mQueue.begin();
      mQueue.erase(iter);
   }
   mMutex.unlock();
}

inline UInt64 CTransmittedFramesQueue::getTimePeriodInNanoSec(timespec& first, timespec& second)
{
   if (!(((UInt64) second.tv_sec * 1000000000LLU + (UInt64) second.tv_nsec) >= ((UInt64) first.tv_sec * 1000000000LLU + (UInt64) first.tv_nsec)))
   {
      LOG4CPLUS_INFO(logger, "CTransmittedFramesQueue::getTimePeriodInNanoSec first("
            + convertIntegerToString(first.tv_sec) + ", "
            + convertIntegerToString(first.tv_nsec) + "), second("
            + convertIntegerToString(second.tv_sec) + ", "
            + convertIntegerToString(second.tv_nsec) + ")");
   }
   assert( ((UInt64)second.tv_sec * 1000000000LLU + (UInt64)second.tv_nsec) >= ((UInt64)first.tv_sec * 1000000000LLU + (UInt64)first.tv_nsec));
   return ((UInt64) second.tv_sec * 1000000000LLU + (UInt64) second.tv_nsec) - ((UInt64) first.tv_sec * 1000000000LLU + (UInt64) first.tv_nsec);
}

void CTransmittedFramesQueue::lockQueue()
{
   return mMutex.lock();
}

void CTransmittedFramesQueue::unlockQueue()
{
   return mMutex.unlock();
}
