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





#ifndef CHANNELSUPERVISOROBSERVER_HPP_
#define CHANNELSUPERVISOROBSERVER_HPP_

#include "IChannelSupervisorObserver.hpp"
#include "../../../../../utils/misc/include/Types.hpp"
#include "utils/misc/include/Logger.hpp"

static Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("ChannelSupervisor.ChannelSupervisorObserver"));
class ChannelSupervisorObserver: public IChannelSupervisorTubeObserver
{

public:
   ChannelSupervisorObserver()
   {
   }
   virtual ~ChannelSupervisorObserver()
   {
   }

   void dataReceivedCallback(const unsigned int channel_id,
         const unsigned int read_size, const unsigned int free_size)
   {
      LOG4CPLUS_TRACE(logger, "ChannelSupervisorObserver::dataReceivedCallback()=> channel"
            +convertIntegerToString(channel_id) + "size"+convertIntegerToString(read_size));
   }

   void bufferOverflowCallback(const unsigned int channel_id)
   {
      LOG4CPLUS_TRACE(logger, "ChannelSupervisorObserver::bufferOverflowCallback()=> channel"
         +convertIntegerToString(channel_id));
   }

   void channelDeletedCallback(const unsigned int channel_id)
   {
      LOG4CPLUS_TRACE(logger, "ChannelSupervisorObserver::channelDeletedCallback()=> channel"
         +convertIntegerToString(channel_id));
   }

   void connectionLostCallback()
   {
      LOG4CPLUS_TRACE(logger, "ChannelSupervisorObserver::connectionLostCallback()");
   }

private:
};

#endif /* CHANNELSUPERVISOROBSERVER_HPP_ */
