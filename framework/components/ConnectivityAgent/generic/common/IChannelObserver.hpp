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


#ifndef CHANNEL_OBSERVER_HPP
#define CHANNEL_OBSERVER_HPP

#include "Types.hpp"

/// IChannelObserver interface
/**
 * Proxy  uses this observer interface to send the data flow control callbacks to client.
 */

class IChannelObserver
{
public:

   /**
    * Callback for data receiving. Informs client about readable data and free space in buffer.
    * Client should implement this callback
    * @param channel_id channel number which has received data
    * @param read_size data size available for read
    */
   virtual void dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size) = 0;

   /**
    * Callback for channel buffer overflow.
    * Client should implement this callback
    * @param channel_id channel number which data has been lost
    */
   virtual void bufferOverflowCallback(const UInt32 channel_id) =0;

   /**
    * Callback for channel deletion initiated by other side application
    * Client should implement this callback
    * @param channel_id channel number which has been deallocated
    */
   virtual void channelDeletedCallback(const UInt32 channel_id) =0;

   /**
    * Callback for lost connection with the other side Connectivity Agent
    * Client should implement this callback
    */
   virtual void connectionLostCallback() =0;

protected:

   virtual ~IChannelObserver() { }
};

#endif
