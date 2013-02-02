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
#include <assert.h>

#include "CQoS.hpp"
#include "CTransmittedFramesQueue.hpp"
#include "Logger.hpp"

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
   mTransmitStartTimes.insert(std::pair<UInt64, timespec>(
           concatenateChannelIdFrameNum(frameTS.mFrame.mFrameHeader), frameTS.mStartTime));

   LOG4CPLUS_INFO(logger, "CTransmittedFramesQueue::enqueue() qsize = " + convertIntegerToString((int)mQueue.size()));
}

void CTransmittedFramesQueue::dequeue(const UInt32 channel_id, const UInt32 number)
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
         mTransmitStartTimes.erase(concatenateChannelIdFrameNum((*iter).mFrame.mFrameHeader));
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

void CTransmittedFramesQueue::deleteFramesForChannel(const UInt32 channel_id)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    MutexLocker lock(mMutex);
    LOG4CPLUS_INFO(logger, "CTransmittedFramesQueue::deleteFramesForChannel(chID = " + convertIntegerToString(channel_id));

    TTransmittedFrameQueue::iterator iter = mQueue.begin();
    while ( iter != mQueue.end())
    {
      if ((*iter).mFrame.mFrameHeader.channel_id == channel_id)
      {
          mTransmitStartTimes.erase(concatenateChannelIdFrameNum((*iter).mFrame.mFrameHeader));
          iter = mQueue.erase(iter);
      } 
      else
      {
        ++iter;
      }
   }

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
         + convertIntegerToString(pFrame->mFrameHeader.number) + ")");

   /**
    * We are trying to retransmit the frame while the RETRANSMIT_TIMEOUT_SEC is not passed
    */
   UInt64 currentChannelIdFrameNum = concatenateChannelIdFrameNum(pFrame->mFrameHeader);
   std::map<UInt64, timespec>::iterator iter = mTransmitStartTimes.find(currentChannelIdFrameNum);
   if (iter != mTransmitStartTimes.end())
   {
       if ((fts.mStartTime.tv_sec - (*iter).second.tv_sec) < RETRANSMIT_TIMEOUT_SEC)
       {
           mQueue.push_back(tFrameTimeStamped(fts));
       }
       else
       {
           LOG4CPLUS_INFO(logger, "Retransmit timeout => dequeue frame from channel "
                   + convertIntegerToString(pFrame->mFrameHeader.channel_id) + ", number "
                   + convertIntegerToString(pFrame->mFrameHeader.number));
           mTransmitStartTimes.erase(currentChannelIdFrameNum);
       }
   }
   else
   {
       LOG4CPLUS_WARN(logger, "Can't find current frame in mTransmitStartTimes");
       mQueue.push_back(tFrameTimeStamped(fts));
   }

   mQueue.pop_front();

   mMutex.unlock();
}

CTransmittedFramesQueue::~CTransmittedFramesQueue()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   mMutex.lock();
   mQueue.clear();
   mTransmitStartTimes.clear();
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

UInt64 CTransmittedFramesQueue::concatenateChannelIdFrameNum(
        const iviLink::ConnectivityAgent::HAL::Frame::FrameHeader& header)
{
    return ((static_cast<UInt64>(header.channel_id) << (sizeof(header.channel_id) * 8))
            | static_cast<UInt64>(header.number));
}
