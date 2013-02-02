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


#ifndef CINCOME_DISPATCHER_HPP
#define CINCOME_DISPATCHER_HPP

#include <list>
#include <deque>
#include <functional>

#include "IFrameReceiver.hpp"
#include "Types.hpp"
#include "CMutex.hpp"
#include "Logger.hpp"
#include "ConnectivityAgentError.hpp"

namespace iviLink
{
   namespace ConnectivityAgent
   {
      namespace HAL
      {
         class CCarrierAdapter;
      }

      namespace L0
      {
         class CTarget;
         class IBufferConsumer;
         class CTransmittedFramesQueue;
         ///CIncomeDispatcher class
         /**
          * The CIncomeDispatcher implements IFrameReceiver interface which notification interface from carrier 
          * When new frame arrived, carrier notifies dispatcher with it, and dispatcher send it to appropriate Target
          */

         class  CIncomeDispatcher: public IFrameReceiver
         {
         private:
            static Logger logger;

         public:

            // Types section
            /**
             * TTargetList type represents list of data targets 
             */
            //typedef std::list<CTarget* > TTargetList;
            typedef std::map<UInt32, CTarget* > TTargetMap;

            // Methods section

            /**
             * Constructor
             * @param pCarrierAdaptor is the pointer to current carrier adapter which interacts with physical connection 
             * @param pTransmittedFrameQueue is the pointer to queue where the transmitted frames are stored in waiting for acknowledge
             */
            CIncomeDispatcher(iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pCarrierAdaptor, CTransmittedFramesQueue* pTransmittedFrameQueue);

            /**
             * Destructor
             */ 
            virtual ~CIncomeDispatcher();

            /**
             * Callback method for carrier adapter inherited from IFrameReceiver interface
             * @param frame is the reference to received frame
             */
            virtual ConnectivityAgentError receiveFrame(const iviLink::ConnectivityAgent::HAL::Frame& frame);


            /**
             * Open the incoming channel with fixed number and priority. It creates CTarget object for given channel, put it
             * to targets list, and establish connections between CTargetAgent and appropriate CTarget
             * @param channel_id is the channel number
             * @param bufferConsumer is the connection with L0 target representation i.e. TargetAgent to exchange the freed buffers
             * @return ERR_OK if channel is opened
             */
            ConnectivityAgentError openChannel(const UInt32 channel_id, IBufferConsumer& bufferConsumer);

            /**
             * Close the outgoing channel by its number
             * @param channel_id is the channel number
             */  
            ConnectivityAgentError closeChannel(const UInt32 channel_id);
            
            void replaceCarrier(iviLink::ConnectivityAgent::HAL::CCarrierAdapter* pCarrierAdapter);
            
         private:

            // Methods section 

            /**
             * Copying constructor
             *
             * Unimplemented to avoid missusing
             *
             * @param classRef The reference to the object the class is constructing on
             */ 
            CIncomeDispatcher(const CIncomeDispatcher& classRef );

            /**
             * Assigning operator
             *
             * Unimplemented to avoid missusing
             *
             * @param classRef The reference to the object the class is assiging from
             */
            CIncomeDispatcher& operator=(const CIncomeDispatcher& classRef );

            // Members section

            iviLink::ConnectivityAgent::HAL::CCarrierAdapter*           mpCarrier;            
            TTargetMap                                                  mTargetMap;
            CMutex                                                      mMapMutex;
            CTransmittedFramesQueue*                                    mpTransmittedFrameQueue;
            bool                                                        mDestroyed;
         };         
      }
   }
}

#endif
