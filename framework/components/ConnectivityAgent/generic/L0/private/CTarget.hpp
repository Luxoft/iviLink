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


#ifndef CTARGET_HPP_
#define CTARGET_HPP_

#include <list>

#include "IBufferProducer.hpp"

#include "Types.hpp"
#include "CMutex.hpp"
#include "ConnectivityAgentError.hpp"


namespace iviLink
{
   namespace ConnectivityAgent
   {

      namespace L1
      {
         class IBufferProducer;
      }  // namespace L1

      namespace L0
      {
         class Frame;
         class IDataConsumer;
         class CTargetTest;
         /// CTarget class
         /**
          * This class is HAL-level channel receiving part. It receives data frames and fills buffers.
          */
         class CTarget: public iviLink::ConnectivityAgent::L1::IBufferProducer
         {
         private:
            static Logger logger;

            /*
             * Friend class for testing
             */
            friend class CTargetTest;

         public:

            /**
             * Constructor
             * @param channel_id - logical channel ID
             */
            CTarget(UInt32 channel_id);

            virtual ~CTarget();

            /**
             * This function is used register data receiver in L0.
             * @param channel_agent - implementor of IDataReceiver
             *
             */
            virtual void registerConsumer(IBufferConsumer* pConsumer);

            /**
             *
             * @param pBuffer
             * @return
             */
            virtual void returnBufferBack(iviLink::ConnectivityAgent::Buffer* pBuffer);

            /**
             *
             * @return
             */
            virtual void unregisterConsumer(IBufferConsumer* pConsumer);

            /**
             * This function uses passed frame to fill buffers and forward them to IDataReceiver
             * @param frame - data frame
             * @return
             */
            virtual ConnectivityAgentError receiveFrame(const iviLink::ConnectivityAgent::HAL::Frame& frame);

            /**
             * Return logical channel ID.
             * @return channel id
             */
            inline const UInt32 getChannelID() const
            {
               return mChannelID;
            }

            void setCarrierReplaced()
            {
               mCarrierReplaced = true;
            }
         private:
            // Methods section

            /**
             * Method copies data form frame to passed buffer. It (re)allocates needed memory in buffer.
             * @param frame - reference to frame with data
             * @param buffer - reference to buffer to be filled with data
             * @return
             */
            void copyFromFrame(const iviLink::ConnectivityAgent::HAL::Frame& frame,
               iviLink::ConnectivityAgent::Buffer& buffer);


         private:

            typedef std::list<iviLink::ConnectivityAgent::Buffer*> tBufferQueue;

            //Member section

            /// Logical channel ID
            UInt32 mChannelID;

            /// Registered buffer consumer
            IBufferConsumer* mpBufferConsumer;


            UInt32                        mLastFrameNr;
            bool                          mCarrierReplaced;
            ///
            tBufferQueue                  mBufferQueue;
            ///
            mutable CMutex                mBufferQueueMutex;
         };
      }
   }
}

#endif /* CTARGET_HPP_ */
