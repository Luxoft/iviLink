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


#ifndef IFRAMERECEIVER_HPP_
#define IFRAMERECEIVER_HPP_

#include "Types.hpp"
#include "ConnectivityAgentError.hpp"


namespace iviLink
{
   namespace ConnectivityAgent
   {
      namespace HAL
      {
         class Frame;
      }  // namespace HAL

      namespace L0
      {


         /// IFrameReceiver
         /**
          * This is the interface to delegate that will get new received frame
          * from CCarrierAdapter.
          */
         class IFrameReceiver
         {
         public:
            /**
             * Method is called by CCarrierAdapter when new frame is received
             * @param frame Frame is allocated by CCarrierAdapter.
             * @return ERROR_NO_FREE_BUFFERS_LEFT when there are no free buffers left to take the data in
             * ERROR_WRONG_FRAME_SEQUENCE current frame number is greater than the previous one's + 1
             * ERROR_RESEND_ACK current frame number is equal or less than the previous one's 
             * (that means the frame we got is a retransmitted one)
             * IVILINK_NO_ERROR in case of success
             */
            virtual ConnectivityAgentError receiveFrame(const iviLink::ConnectivityAgent::HAL::Frame& frame)=0;

         protected:
            virtual ~IFrameReceiver() {}
         };       
      }
   }
}


#endif /* IFRAMERECEIVER_HPP_ */
