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


/**
 * \file Channel.hpp
 * This file contains declarations of convenience methods for communication with other side
 * (such as allocate/deallocate channel for communication and send data to the other side)
 * and a declaration of a base class for channel callbacks (such as receiving data).
 */

#ifndef CHANNEL_HPP_
#define CHANNEL_HPP_

#include "IChannelSupervisorObserver.hpp"
#include "Buffer.hpp"
#include "PublicError.hpp"

namespace iviLink
{
namespace Channel
{
/**
 * \namespace iviLink::Channel
 * This namespace contains convenience methods for communication with other side
 * (such as allocate/deallocate channel for communication and send data to the other side)
 * and a base class for channel callbacks (such as receiving data).
 */

typedef UInt32 tChannelId;

/**
 * CChannelHandler is an abstract class for handling channel callbacks. Your profile should either 
 * inherit it, or contain an instance of a derived class to be able to
 * receive data from the other side.
 */

class ChannelHandler: public iviLink::ChannelSupervisor::IChannelSupervisorTubeObserver
{
public:
    /**
     * Called from dataReceivedCallback.
     * @param channel ID of a channel that has received data
     * @param buffer  CBuffer object with received data
     */
    virtual void onBufferReceived(const iviLink::Channel::tChannelId channel,
            iviLink::Buffer const& buffer) = 0;

public:
    // The following methods are inherited from iviLink::ChannelSupervisor::IChannelSupervisorTubeObserver

    /**
     * Called when data is avaliable in channel
     *
     * This implementation will receive data and pack it into a dynamically allocated CBuffer
     * and then call bufferReceived()
     * @param channelId ID of the channel that has received data
     * @param readSize total size of data avaliable in the channel
     */
    virtual void onDataReceived(const iviLink::Channel::tChannelId channelId,
            const UInt32 readSize);

    /**
     * Called on channel buffer overflow.
     *
     * This implementation will do nothing
     * @param channelId ID of a channel where buffer overflow has happenned
     */
    virtual void onBufferOverflow(const iviLink::Channel::tChannelId channelId);

    /**
     * Called when a previously allocated channel is deallocated on the other side
     * (this method will be called from the object registered as a channel handler in allocateChannel()).
     * At present, classes derived from CChannelHandler
     * should call deallocateChannel(channelId) in their implementation of this method.
     * @param channelID ID of the deleted channel
     */
    virtual void onChannelDeleted(const iviLink::Channel::tChannelId channelId) = 0;

    /**
     * Called when connection with the other side is lost.
     */
    virtual void onConnectionLost() = 0;

protected:

    virtual ~ChannelHandler()
    {
    }

};

/**
 * Allocate a channel identified with given tag.
 * @param tag  is used to find channel with identical tag on the other side to connect with
 * @param pHandler  pointer to a callbacks handler
 * @param priority is channel priority
 * @return channel  ID of a newly allocated channel in case of success, otherwise 0
 */
Error allocateChannel(std::string const& tag, iviLink::Channel::ChannelHandler * pHandler, tChannelId& resultChannelId,  TChannelPriority priority = ePlainData);

/**
 * Allocate a channel identified with given tag. For asymmetrical profiles. It is expected
 * that the paired profile will call allocateChannelAsClient with the same tag.
 * @param tag  is used to find channel with identical tag on the other side to connect with
 * @param pHandler  pointer to a callbacks handler
 * @param priority is channel priority
 * @return channel  ID of a newly allocated channel in case of success, otherwise 0
 */
Error allocateChannelAsServer(std::string const& tag,
        iviLink::Channel::ChannelHandler * pHandler, tChannelId& resultChannelId,  TChannelPriority priority = ePlainData);

/**
 * Allocate a channel identified with given tag. For asymmetrical profiles. It is expected
 * that the paired profile will call allocateChannelAsServer with the same tag.
 * @param tag  is used to find channel with identical tag on the other side to connect with
 * @param pHandler  pointer to a callbacks handler
 * @param priority is channel priority
 * @return channel  ID of a newly allocated channel in case of success, otherwise 0
 */
Error allocateChannelAsClient(std::string const& tag,
        iviLink::Channel::ChannelHandler * pHandler, tChannelId& resultChannelId,  TChannelPriority priority = ePlainData);

/**
 * Deallocate previously allocated channel.
 * @param channel  ID of a channel to be deallocated
 * @return error code
 */
Error deallocateChannel(const iviLink::Channel::tChannelId channel);

/**
 * Send a buffer to the other side.
 * Uses buffer's write position to get the size of data to send.
 * @param channel ID of channel
 * @param buffer CBuffer with data to send
 * @return error code
 */
Error sendBuffer(const iviLink::Channel::tChannelId channel, iviLink::Buffer const& buffer);

/**
 * Send a buffer to the other side
 * @param channel  ID of a channel the buffer will be sent to
 * @param pBuffer  pointer to an array of bytes with data to send
 * @param buferSize  size of pBuffer
 * @return error code
 */
Error sendBuffer(iviLink::Channel::tChannelId channel, void const* pBuffer, UInt32 bufferSize);

}  // namespace Channel
}  // namespace iviLink

#endif /* CHANNEL_HPP_ */
