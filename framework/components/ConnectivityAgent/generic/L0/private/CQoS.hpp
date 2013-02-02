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


#ifndef CQOS_HPP
#define CQOS_HPP

#include <time.h>
#include <map>
#include <utility>

#include "Types.hpp"
#include "CThread.hpp"
#include "CMutex.hpp"
#include "ConnectivityAgentError.hpp"

namespace iviLink
{
   namespace ConnectivityAgent
   {
      namespace L0
      {
          ///CQoS class
         /**
          * The CQoS implements the quality of service functionality at HAL layer.
          * It is implemented as time statistics information storage for each channel.
          * It stores the average time frame is staying in trnasmitted frames queue until ACK for given frame is received
          */

         class  CQoS: public CThread
         {

         public:

            // Types section

            /**
             * tTimeStat type represents time statistic for channel
             */ 
            struct tTimeStat
            {
               UInt64 mSummaryTime;
               UInt32 mCounter;
               UInt64 mAverageTime;
               UInt64 mUpperTime;
               UInt64 mLowerTime;
               tTimeStat():mSummaryTime(0),mCounter(0),mAverageTime(0),mUpperTime(0),mLowerTime(0) {};
              ~tTimeStat(){mSummaryTime= 0;mCounter = 0;}
              UInt64 getAverage(){if(mCounter) mAverageTime = mSummaryTime/mCounter; return mAverageTime;}
              void setTime(UInt64 time){mSummaryTime += time; ++mCounter;}
              void resetStat(){mSummaryTime= 0;mCounter = 0;}
            };

            /**
             * TStatisticsMap type represents time statistic information maped on channel numbers
             */    
            typedef std::map<UInt32, tTimeStat> TStatisticsMap;

            /**
             * TItem type represents item of the TStatisticsMap
             */
            typedef std::pair<UInt32, tTimeStat> TItem;
            // Methods section

            /**
             * Default constructor
             */
            CQoS();

            /**
             * Destructor
             */ 
            ~CQoS();

            /**
             * Open channel in QoS context means creating an entry in the map with given channel number
             * @param channel_id is the channel number
             * @param upper_threshold is the upper border for average time for channel
             * @param lower_threshold is the lower border for average time for channel
             * @return ERROR_CHANNEL_BUSY if channel already opened
             */
            ConnectivityAgentError openChannel(const UInt32 channel_id, const UInt64 upper_threshold, const UInt64 lower_threshold);
            
            /**
             * Interface function for CTransmittedFramesQueue to put its time measurement
             * @param channel_id is the channel number
             * @param time is the measured time
             * @return ERROR_NOT_FOUND if channel was not found
             */
            ConnectivityAgentError setTime(const UInt32 channel_id, const UInt64 time);

            /**
             * Interface function for anyone who desire to know statistic for given channel
             * @param channel_id is the channel number
             * @return ERROR_NOT_FOUND if channel was not found
             */
            ConnectivityAgentError getAverageForChannel(const UInt32 channel_id, UInt64& result);

            /**
             * close channel in QoS context means remove an entry with given channel number from the map 
             * @param channel_id is the channel number
             * @return ERROR_NOT_FOUND if channel was not found
             */
            ConnectivityAgentError closeChannel(const UInt32 channel_id);

            /**
             * A threading routine method inherited from CThread class
             */ 
            virtual void threadFunc(); 

         private:
            // Methods section 

            /**
             * Copying constructor
             *
             * Unimplemented to avoid missusing
             *
             * @param classRef The reference to the object the class is constructing on
             */ 
            CQoS(const CQoS&);

            /**
             * Assigning operator
             *
             * Unimplemented to avoid missusing
             *
             * @param classRef The reference to the object the class is assiging from
             */
            CQoS& operator=(const CQoS&);

         private:
            // Members section

            TStatisticsMap                   mStatMap;
            CMutex                           mMapMutex;
         };

      }
   }
}

#endif
