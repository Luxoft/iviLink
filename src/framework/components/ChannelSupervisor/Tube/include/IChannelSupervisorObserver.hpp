/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */

/* 
 * File:   IChannelSupervisorObserver.h
 * Author: mprosuntsov
 *
 * Created on January 26, 2012, 5:01 PM
 */


#ifndef CHANNEL_SUPERVISOR_TUBE_H
#define CHANNEL_SUPERVISOR_TUBE_H

#include <string>
#include "../../common/Common.hpp"



namespace AXIS
{
   namespace ChannelSupervisor
   {
      /// IChannelSupervisorTubeObserver interface
      /**
      * Proxy uses this observer interface to send the data flow control callbacks to the client.
      * each call back should be implemented by the client
      */
      class IChannelSupervisorTubeObserver
      {
      public:
         /**
          * Callback informing client the data is ready to be received by the the client.
          * @param channelId id of the channel from where data may be read
          * @param readSize data size available for read
          * @param freeSize free size left
          */
         virtual void dataReceivedCallback(const unsigned int channelId, const unsigned int readSize, const unsigned int freeSize) = 0;

         /**
         * Callback indicating channel buffer overflow.
         * @param channelId channel number which data has been lost
         */
         virtual void bufferOverflowCallback(const unsigned int channelId) = 0;

         /**
          * Callback indicating channel is deleted
          * @param channelId channel number which has been deallocated
          */
         virtual void channelDeletedCallback(const unsigned int channelId) = 0;

         /**
          * Callback indicating connection between devices is lost
          * @param channelId channel number which has been deallocated
          */
         virtual void connectionLostCallback() = 0;

      protected:
         virtual ~IChannelSupervisorTubeObserver()
         {
         }
      };
   }
}



#endif //CHANNEL_SUPERVISOR_TUBE_H

