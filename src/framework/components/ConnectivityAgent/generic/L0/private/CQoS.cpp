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

/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
 
 /********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CQoS.hpp"
#include "framework/components/ConnectivityAgent/generic/L1/CChannelAllocator.hpp"

//using AXIS::ConnectivityAgent::L1::CChannelAllocator;
//using namespace  AXIS::ConnectivityAgent::L0;
//using namespace AXIS::ConnectivityAgent::L0;
using namespace iviLink::ConnectivityAgent::L0;

CQoS::CQoS(): CThread("CQos")
{
   start();
}

CQoS::~CQoS()
{
   stop();
}

void CQoS::openChannel(UInt32 channel_id, UInt64 upper_threshold, UInt64 lower_threshold)
{
   tTimeStat timeStat;
   timeStat.mUpperTime = upper_threshold;
   timeStat.mLowerTime = lower_threshold;
   mMapMutex.lock();
   mStatMap.insert(TItem(channel_id,timeStat ));
   mMapMutex.unlock();
}

void CQoS::closeChannel(UInt32 channel_id)
{
   mMapMutex.lock();
   TStatisticsMap:: iterator iter = mStatMap.find(channel_id);
   if (iter != mStatMap.end())
   {
      mStatMap.erase (iter);
   }
   mMapMutex.unlock();
}
void CQoS::setTime(UInt32 channel_id, UInt64 time)
{
   mMapMutex.lock();
   TStatisticsMap:: iterator iter = mStatMap.find(channel_id);
   if (iter != mStatMap.end())
   {
      (*iter).second.setTime(time);
   }
   mMapMutex.unlock();
}

UInt64 CQoS::getAverageForChannel(UInt32 channel_id)
{
   UInt64 ret = 0;

   mMapMutex.lock();
   TStatisticsMap:: iterator iter = mStatMap.find(channel_id);
   if (iter != mStatMap.end())
   {
      ret =  (*iter).second.getAverage();
   }
   mMapMutex.unlock();
   return ret;
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
