/* 
 * 
 * iviLINK SDK, version 1.1.2
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











#ifndef CHANNELSUPERVISORTUBE_H
#define	CHANNELSUPERVISORTUBE_H

#include <string>
#include "utils/misc/Types.hpp"
#include "framework/components/ChannelSupervisor/common/Common.hpp"
#include "framework/components/ChannelSupervisor/common/CSError.hpp"
#include "IChannelSupervisorObserver.hpp"
#include "utils/misc/Logger.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{

/**
 * Negotiates channel id with other party and allocates the channel
 * @param observer	- registration of the observer for Channel supervisor events receiving
 * @param tag		- tag of the channel
 * @param channelId - channel id
 * @return CS_SUCCESS if channel successfully allocated, otherwise CS_ERR_OTHER
 */
CError allocateChannel(IChannelSupervisorTubeObserver* observer, std::string tag,
		UInt32 & channelId);

/**
 * Deallocates the channel with the given id
 * @param channelId - id of the channel to be deallocated
 * @param tag		- tag of the channel
 * @return CS_SUCCESS if channel successfully deallocated, otherwise CS_ERR_OTHER
 */
CError deallocateChannel(const UInt32 channelId, std::string tag);

/**
 * Sends the data through the channel
 * @param channelId - id of the channel
 * @param tag		- tag of the channel
 * @param data		- pointer to the data array
 * @param size		- data length
 * @return CS_SUCCESS if channel successfully sent, otherwise CS_ERR_OTHER
 */
CError sendData(const UInt32 channelId, std::string tag, unsigned char const* data, UInt32 size);

/**
 * Receives the data from the channel
 * @param channelId		- id of the channel
 * @param tag			- tag of the channel
 * @param data			- pointer to the data array to be read
 * @param receivedSize	- data length
 * @param maxSize		- size of the array the data will be read to
 * @return CS_SUCCESS if channel successfully read, otherwise CS_ERR_OTHER
 */
CError receiveData(const UInt32 channelId, std::string tag, unsigned char * data,
		UInt32 & receivedSize, UInt32 maxSize);

/**
 * Retrieve free size left in channel
 * @param channelId channel number
 * @param[out] freeSize free size left in channel. In case of error, value of
    *    param is undefined.
 * @retval NO_ERROR operation successful, result in @c freeSize param
 * @retval ERROR_OTHER something wrong
 */
CError getFreeSize(const UInt32 channelId, UInt32 & freeSize);

/**
 * Kicks watchdog for the given channel
 * @param channelId		- id of the channel
 * @param tag			- tag of the channel
 * @return CS_SUCCESS if channel successfully kicked, otherwise CS_ERR_OTHER
 */
CError kickWatchdog(const UInt32 channelId, std::string tag);

/**
 * Allocates the 0 channel
 * @param observer	- registration of the observer for Channel supervisor events receiving
 * @param tag		- tag of the channel
 * @return CS_SUCCESS if channel successfully allocated, otherwise CS_ERR_OTHER
 */
CError allocateZeroChannel(IChannelSupervisorTubeObserver* observer, std::string tag);



namespace UnstableAPI
{

/**
 * Returns type and adresses of underlying carrier.
 * @param[out] type
 * @param[out] localAddr
 * @param[out] remoteAddr
 * @retval NO_ERROR all ok
 * @retval ERROR_OTHER some error, parrameters were not changed
 */
CError getConnectionAddr(std::string& type, std::string& localAddr, std::string& remoteAddr);

} // namespace UnstableAPI
} //namespace ChannelSupervisor
} //namespace AXIS

#endif	/* CHANNELSUPERVISORTUBE_H */

