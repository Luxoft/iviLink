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


#include "CQoS.hpp"
#include "CChannelAllocator.hpp"

using namespace iviLink::ConnectivityAgent::L0;
using namespace iviLink::ConnectivityAgent;

CQoS::CQoS(): CThread("CQos")
{
   start();
}

CQoS::~CQoS()
{
   stop();
}

ConnectivityAgentError CQoS::openChannel(const UInt32 channel_id, const UInt64 upper_threshold, const UInt64 lower_threshold)
{
   tTimeStat timeStat;
   timeStat.mUpperTime = upper_threshold;
   timeStat.mLowerTime = lower_threshold;
   
   MutexLocker lock(mMapMutex);
   TStatisticsMap:: iterator iter = mStatMap.find(channel_id);
   if (iter != mStatMap.end())
   {
      return ConnectivityAgentError(ConnectivityAgentError::ERROR_CHANNEL_BUSY);
   }
   mStatMap.insert(TItem(channel_id,timeStat ));
   return ConnectivityAgentError::NoError();
}

ConnectivityAgentError CQoS::closeChannel(UInt32 channel_id)
{
   ConnectivityAgentError retError;
   mMapMutex.lock();
   TStatisticsMap:: iterator iter = mStatMap.find(channel_id);
   if (iter != mStatMap.end())
   {
      mStatMap.erase (iter);
      retError.setNoError();
   }
   else 
   {
      retError.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
   }
   mMapMutex.unlock();
   return retError;
}

ConnectivityAgentError CQoS::setTime(const UInt32 channel_id, const UInt64 time)
{
   ConnectivityAgentError retError;
   mMapMutex.lock();
   TStatisticsMap:: iterator iter = mStatMap.find(channel_id);
   if (iter != mStatMap.end())
   {
      (*iter).second.setTime(time);
      retError.setNoError();
   }
   else 
   {
      retError.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
   }
   mMapMutex.unlock();
   return retError;
}

ConnectivityAgentError CQoS::getAverageForChannel(const UInt32 channel_id, UInt64& result)
{
   ConnectivityAgentError retError;
   mMapMutex.lock();
   TStatisticsMap:: iterator iter = mStatMap.find(channel_id);
   if (iter != mStatMap.end())
   {
      result =  (*iter).second.getAverage();
      retError.setNoError();
   }
   else 
   {
      retError.setErrorCode(ConnectivityAgentError::ERROR_NOT_FOUND);
   }
   mMapMutex.unlock();
   return retError;
}

void CQoS::threadFunc()
{
  while (false == getStopFlag())
   {
      mMapMutex.lock();
      if (!mStatMap.empty())
      {
         TStatisticsMap:: iterator iter = mStatMap.begin();
         while (iter != mStatMap.end())
         {
            tTimeStat timeStat = iter->second;
            if (timeStat.mUpperTime < timeStat.getAverage())
            {
               iviLink::ConnectivityAgent::L1::CChannelAllocator::getInstance()->narrowBandwidth(iter->first);
            }
            if (timeStat.mLowerTime > timeStat.getAverage())
            {
               iviLink::ConnectivityAgent::L1::CChannelAllocator::getInstance()->higherBandwidth(iter->first);
            }
            ++iter;
         }
      }
      mMapMutex.unlock();
      CThread::sleep(5000);
   }
}
