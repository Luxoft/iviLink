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


#ifndef CHANNELSUPERVISORTUBE_H
#define	CHANNELSUPERVISORTUBE_H

#include <string>
#include "Types.hpp"
#include "Common.hpp"
#include "CSError.hpp"
#include "IChannelSupervisorObserver.hpp"
//#include "Logger.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{

/**
 * Negotiates channel id with other party and allocates the channel
 * @param observer is registration of the observer for Channel supervisor events receiving
 * @param tag is tag of the channel
 * @param channelId is channel id
 * @param priority is channel priority
 * @return CS_SUCCESS if channel successfully allocated, otherwise CS_ERR_OTHER
 */
BaseError allocateChannel(IChannelSupervisorTubeObserver* observer,
                       std::string const& tag,
                       UInt32 & channelId,
                       TChannelPriority priority);

/**
 * Deallocates the channel with the given id
 * @param channelId - id of the channel to be deallocated
 * @param tag		- tag of the channel
 * @return CS_SUCCESS if channel successfully deallocated, otherwise CS_ERR_OTHER
 */
BaseError deallocateChannel(const UInt32 channelId);

/**
 * Sends the data through the channel
 * @param channelId - id of the channel
 * @param tag		- tag of the channel
 * @param data		- pointer to the data array
 * @param size		- data length
 * @return CS_SUCCESS if channel successfully sent, otherwise CS_ERR_OTHER
 */
BaseError sendData(const UInt32 channelId, UInt8 const* data, const UInt32 size);

/**
 * Receives the data from the channel
 * @param channelId		- id of the channel
 * @param tag			- tag of the channel
 * @param data			- pointer to the data array to be read
 * @param receivedSize	- data length
 * @param maxSize		- size of the array the data will be read to
 * @return CS_SUCCESS if channel successfully read, otherwise CS_ERR_OTHER
 */
BaseError receiveData(const UInt32 channelId, UInt8 * data, UInt32 & receivedSize,
		const UInt32 maxSize);

/**
 * Retrieve free size left in channel
 * @param channelId channel number
 * @param[out] freeSize free size left in channel. In case of error, value of
 *    param is undefined.
 * @retval IVILINK_NO_ERROR operation successful, result in @c freeSize param
 * @retval ERROR_OTHER something wrong
 */
BaseError getFreeSize(const UInt32 channelId, UInt32 & freeSize);

/**
 * Kicks watchdog for the given channel
 * @param channelId		- id of the channel
 * @param tag			- tag of the channel
 * @return CS_SUCCESS if channel successfully kicked, otherwise CS_ERR_OTHER
 */
BaseError kickWatchdog(const UInt32 channelId, std::string const& tag);

/**
 * Allocates the service channel: 1 - Channel Supervisor service channel
 * @param observer	- registration of the observer for Channel supervisor events receiving
 * @param tag		- tag of the channel
 * @param channelId - id of the channel
 * @return CS_SUCCESS if channel successfully allocated, otherwise CS_ERR_OTHER
 */
BaseError allocateServiceChannel(IChannelSupervisorTubeObserver* observer, const UInt32 channelId);

bool isIdRestricted(const UInt32 numberToCheck);

} //namespace ChannelSupervisor
} //namespace AXIS

#endif	/* CHANNELSUPERVISORTUBE_H */
