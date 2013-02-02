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


#ifndef TUBETEST_H
#define	TUBETEST_H


#include <iostream>
#include "utils/misc/include/Types.hpp"
#include "../Tube/include/IChannelSupervisorObserver.hpp"

using namespace iviLink;
using namespace ChannelSupervisor;


//
void receiveDataFromCB (const UInt32 cid);


class ChannelObserver: public IChannelSupervisorTubeObserver
{
    
  public:
    ChannelObserver (){}
    virtual ~ChannelObserver (){}
      
    void dataReceivedCallback(const unsigned int channel_id, const unsigned int read_size, const unsigned int free_size)
    {
        std::cout << "We are in callback Size:" << read_size << channel_id  << std::endl;
		//receiveDataTubeTest(cid);
		receiveDataFromCB(channel_id);
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

void TubeTest();
//allocation tests
void allocateTubeTest(UInt32 & cid, ChannelObserver* cObserver);
void allocateTubeTest_already(UInt32 & cid, ChannelObserver* cObserver);
void allocateTubeTest_many(UInt32 & cid, ChannelObserver* cObserver);
void allocateTubeTest_noobserver(UInt32 & cid, ChannelObserver* cObserver);
void allocateTubeTest_emptytag(UInt32 & cid, ChannelObserver* cObserver);
//deallocation tests
void deallocateTubeTest(UInt32 & cid, ChannelObserver* cObserver);
void deallocateTubeTest_deallocated(UInt32 & cid, ChannelObserver* cObserver);
void deallocateTubeTest_wrongtag(UInt32 & cid, ChannelObserver* cObserver);
void deallocateTubeTest_wrongcid(UInt32 & cid, ChannelObserver* cObserver);
//send data tests
void sendDataTubeTest (UInt32 & cid,  ChannelObserver* cObserver);
//receive data test
void receiveDataTubeTest (UInt32 & cid, ChannelObserver* cObserver);
//kick wd tests
void wdKickTubeTest (UInt32 cid, ChannelObserver* cObserver);
//call back request
void reqCallback (UInt32 cid);

#endif	/* TUBETEST_H */

