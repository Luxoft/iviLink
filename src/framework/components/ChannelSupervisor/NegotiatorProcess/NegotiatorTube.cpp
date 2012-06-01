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
 * NegotiatorTube.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: mprosuntsov
 */

#include <cassert>
#include "../Tube/include/ChannelSupervisorTube.hpp"
#include "NegotiatorTube.hpp"
#include "utils/misc/include/Types.hpp"
#include "utils/misc/include/logging.hpp"

 Logger NegotiatorTube::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("NegotiatorProcess.NegotiatorTube"));

void NegotiatorTube::sendDataTube(const char * data)
{
   m_sender->sendDataRemote(data);
}

void NegotiatorTube::dataReceivedCallback(const unsigned int channel_id, const unsigned int read_size,
      const unsigned int free_size)
{
   assert(channel_id == CS_SERVICE_CHANNEL );
   LOG4CPLUS_TRACE(msLogger, "NegotiatorTube::dataReceivedCallback()=> channel"
            +convertIntegerToString(channel_id) + "size"+convertIntegerToString(read_size));

   UInt8 * data = new UInt8[read_size];
   memset(data, 0, read_size);
   
   unsigned int receivedSize = 0;
   
   std::string tag = "Channel supervisor";

   CError err = AXIS::ChannelSupervisor::receiveData(channel_id, tag, data, receivedSize, read_size);

   if (err.isNoError())
   {
      m_negotiatorStates->ProcessTubeNotification(data);
   }
   else
   {
      LOG4CPLUS_WARN(msLogger, "NegotiatorTube::bufferOverflowCallback()=> fail: " + static_cast<std::string>(err));
   }

   delete [] data;
}

void NegotiatorTube::bufferOverflowCallback(const unsigned int channel_id)
{
   LOG4CPLUS_TRACE(msLogger, "NegotiatorTube::bufferOverflowCallback()=> channel"
         +convertIntegerToString(channel_id));
}

void NegotiatorTube::channelDeletedCallback(const unsigned int channel_id)
{
   LOG4CPLUS_TRACE(msLogger, "NegotiatorTube::channelDeletedCallback()=> channel"
         +convertIntegerToString(channel_id));
}

void NegotiatorTube::connectionLostCallback()
{
   LOG4CPLUS_TRACE(msLogger, "NegotiatorTube::connectionLostCallback()");
   mpMsgProxy->sendConnectionLost();
}

void NegotiatorTube::Sender::sendDataRemote(std::string data)
{
   LOG4CPLUS_TRACE(msLogger, "NegotiatorTube::sendDataRemote()");
   AXIS::ChannelSupervisor::sendData(m_CSChannelID, m_CSTag, (UInt8 *) data.c_str(), data.length());
}
