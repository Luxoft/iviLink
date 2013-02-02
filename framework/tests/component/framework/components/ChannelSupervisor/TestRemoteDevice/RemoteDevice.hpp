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


#ifndef REMOTEDEVICE_HPP_
#define REMOTEDEVICE_HPP_

#include "../Tube/include/IChannelSupervisorObserver.hpp"

using namespace iviLink;
using namespace ChannelSupervisor;

class LocalChannelObserver: public IChannelSupervisorTubeObserver
{

  public:
	LocalChannelObserver (){}
    virtual ~LocalChannelObserver (){}

    void dataReceivedCallback(const unsigned int channel_id, const unsigned int read_size, const unsigned int free_size)
    {
        std::cout << "We are in callback Size:" << read_size << channel_id  << std::endl;
		//receiveDataFromCB(channel_id);
    }

    void bufferOverflowCallback(const unsigned int channel_id)
    {
            std::cout << "Buffer overflow " << channel_id  << std::endl;
    }

    void channelDeletedCallback(const unsigned int channel_id)
    {
            std::cout << "Channel deleted" << channel_id  << std::endl;
    }

    void connectionLostCallback()
    {
            std::cout << "Connection lost"  << std::endl;
    }
};


#endif /* REMOTEDEVICE_HPP_ */
