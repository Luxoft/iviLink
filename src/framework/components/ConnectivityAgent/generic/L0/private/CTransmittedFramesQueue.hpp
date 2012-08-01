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






#ifndef CTRANSMITTED_FRAMES_QUEUE_HPP
#define CTRANSMITTED_FRAMES_QUEUE_HPP

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <list>
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/misc/Types.hpp"
#include "utils/threads/CMutex.hpp"
#include "framework/components/ConnectivityAgent/generic/HAL/Frame.hpp"

namespace iviLink
{
   namespace ConnectivityAgent
   {
      namespace L0
      {
         class CQoS;

         ///CTransmittedFramesQueue class
         /**
          * The CTransmittedFramesQueue implements the queue where transmitted frames stored untill the system receives acknowledge for them
          * It also includes timing measurement for each frame duration in queue which is provided to QoS  
          */
         class CTransmittedFramesQueue
         {
         private:
            static Logger logger;

         public:
            // Types section

            /**
             * tFrameTimeStamped type represents frame with timestamp of its queuing
             */
            struct tFrameTimeStamped
            {
               iviLink::ConnectivityAgent::HAL::Frame mFrame;
               timespec mStartTime;
            };

            /**
             * TTransmittedFrameQueue type represents the queue of the frames with timestamp of their queuing
             */
            typedef std::list<tFrameTimeStamped> TTransmittedFrameQueue;

            // Methods section

            /**
             * Constructor
             * @param pQoS is the pointer to current QoS component it the system
             */
            explicit CTransmittedFramesQueue(CQoS* pQoS) :
               mpQoS(pQoS)
            {
            }

            /**
             * Destructor
             */
            ~CTransmittedFramesQueue();

            /**
             * Put the referenced frame in the queue with adding a timestamp
             * Methods lockQueue() and unlockQueue() must be used with this method.
             * @param frame is the reference to the frame that is queuing
             */
            void enqueue(iviLink::ConnectivityAgent::HAL::Frame& frame);

            /**
             * Get the referenced frame out of the queue with adding a timestamp.
             * Basically this function calls when ACK for the frame is received and there is no need to store it any longer.
             * @param channel_id is the channel number frame belong to
             * @param number is the frame sequence number the ACK was received for
             */
            void dequeue(UInt32 channel_id, UInt32 number);

            /**
             * Deletes from queue all frames for specified channel
             * @param channel_id
             */
            void deleteFramesForChannel(UInt32 channel_id);

            /**
             * Returns oldest Frame, that need to be retransmitted because retransmit time is reached for it.
             * If pointer to Frame is returned, queue is locked until the call to reenqueueRetranmittedFrame.
             * @return pointer to Frame to retransmit or NULL.
             */
            iviLink::ConnectivityAgent::HAL::Frame* getFrameToRetransmit();

            /**
             * Puts the referenced retransmitted Frame from front to back of queue with updating timestamp.
             * Unlocks the mMutex that has been locked in getFrameToRetransmit method.
             * @param pFrame - pointer to retransmitted Frame. Must be the oldest Frame in queue.
             */
            void reenqueueRetransmittedFrame(iviLink::ConnectivityAgent::HAL::Frame* pFrame);

            /**
             * Locks the queue with the mutex.
             */
            void lockQueue();

            /**
             * Unlocks queue mutex.
             */
            void unlockQueue();


         private:
            // Types section

            /**
             * Used in deleteFramesForChannel() method to delete frames from queue.
             */
            class RemoveByChannelIDPredicate
            {
               UInt32 chID;
            public:
               RemoveByChannelIDPredicate(UInt32 channel_id) :
                  chID(channel_id)
               {}

               bool operator() (const tFrameTimeStamped& val)
               {
                  return val.mFrame.mFrameHeader.channel_id == chID;
               }
            };

         private:
            // Methods section

            /**
             * Copying constructor
             *
             * Unimplemented to avoid missusing
             *
             * @param classRef The reference to the object the class is constructing on
             */
            CTransmittedFramesQueue(const CTransmittedFramesQueue&);

            /**
             * Assigning operator
             *
             * Unimplemented to avoid missusing
             *
             * @param classRef The reference to the object the class is assiging from
             */
            CTransmittedFramesQueue& operator=(const CTransmittedFramesQueue&);

            /**
             * Gets difference between first and second timestamp.
             * second >= first or assert in the function will not be passed
             * @param first - timestamp
             * @param second - timestamp
             * @return (second - first) time in nanoseconds.
             */
            UInt64 getTimePeriodInNanoSec(timespec& first, timespec& second);

         private:
            // Members section

            CMutex mMutex;
            TTransmittedFrameQueue mQueue;
            CQoS* mpQoS;
         };
      }
   }
}

#endif
