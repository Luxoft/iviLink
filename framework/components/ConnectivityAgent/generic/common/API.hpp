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


#ifndef CONNECTIVITY_AGENT_API_HPP
#define CONNECTIVITY_AGENT_API_HPP

#include "Types.hpp"
#include "ConnectivityAgentError.hpp"

class IChannelObserver;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/**
 * Proxy/Stub control protocol messages
 */
enum tOpCode
{
   E_ALLOCATE_CHANNEL         = 1,     //> Allocate channel request
   E_DEALLOCATE_CHANNEL       = 2,     //> Deallocate channel request
   E_SEND_DATA                = 3,     //> Send Data request
   E_SYNC                     = 4,     //> Synchronize  request
   E_ALLOCATE_CHANNEL_RESP    = 5,     //> Allocate channel response
   E_DEALLOCATE_CHANNEL_RESP  = 6,     //> Deallocate channel response
   E_SEND_DATA_RESP           = 7,     //> Send Data response
   E_SYNC_RESP                = 8,     //> Syncronize  response
   E_RECEIVE_DATA_NTF         = 9,     //> Data received  notification
   E_CONNECTION_LOST_NTF      = 10,    //> Connection Lost notification
   E_DEALLOCATE_CHANNEL_NTF   = 11,    //> Channel deallocation notification

   //
   E_GET_CONNECTION_ADDR      = 100,
   //
   E_DEALLOCATE_CHANNEL_WATCHDOG = 200,
};

enum MessageSender
{
   E_CLIENT              =0,
   E_CONNECTIVITY_AGENT  =1
};

/**
 * Allocates the channel
 * @param prio channel priority type
 * @param channel_id channel number
 * @param observer client observer of the channel
 * @return IVILINK_NO_ERROR in case of success, 
 * ERROR_REQUEST_FAILED in case could not connect to ConnectivityAgent
 */
iviLink::ConnectivityAgent::ConnectivityAgentError allocateChannel(TChannelPriority prio, UInt32 channel_id, IChannelObserver* observer);

/**
 * Deallocates the channel
 * @param channel_id channel number
 * @return ERROR_NOT_FOUND if no channel could be found with such id
 * ERROR_REQUEST_FAILED in case could not connect to ConnectivityAgent
 * IVILINK_NO_ERROR in case of success
 */
iviLink::ConnectivityAgent::ConnectivityAgentError closeChannel(UInt32 channel_id);

/**
 * Sends the raw data stream through the channel
 * @param channel_id channel number
 * @param data pointer to byte stream
 * @param size data size for sending
 * @return ERROR_INCONSISTENT_DATA if data is too large or null, or size is 0
 * ERROR_REQUEST_FAILED if could not connect to ConnectivityAgent
 * ERROR_NOT_FOUND if there is no channel with such id allocated
 * IVILINK_NO_ERROR in case of success
 */
iviLink::ConnectivityAgent::ConnectivityAgentError sendData(UInt32 channel_id, UInt8 const* data, UInt32 size);

/**
 * Receives the raw data stream from channel buffer
 * @param channel_id channel number
 * @param data pointer to byte stream to store the data
 * @param received_size actual received data size
 * @param max_size allocated  size of data buffer
 * @return ERROR_NOT_FOUND if there is no such channel or if there is no data for such channel
 * IVILINK_NO_ERROR in case of success
 */
iviLink::ConnectivityAgent::ConnectivityAgentError receiveData(UInt32 channel_id, UInt8* data, UInt32 & received_size, UInt32 max_size);

/**
 * Retrieve maximum allowed buffer size at current system
 * @return  maximum allowed buffer size
 */
const UInt32 getMaxBufSize();

/**
 * Retrieve free size left in channel
 * @param channel_id channel number
 * @param[out] free_size free size left in channel. In case of error, value of
 *    param is undefined.
 * @retval IVILINK_NO_ERROR operation successful, result in @c free_size param
 * @retval ERROR_NOT_FOUND channel with specified @c channel_id is not found
 */
iviLink::ConnectivityAgent::ConnectivityAgentError getFreeSize(UInt32 channel_id, UInt32& free_size);


void deinitConnectivityAgentProxy();


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CONNECTIVITY_AGENT_API_HPP

