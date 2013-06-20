/* 
 * 
 * iviLINK SDK, version 1.0
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


#ifndef CHANNEL_CLIENT_HPP
#define CHANNEL_CLIENT_HPP

#include "CThread.hpp"
#include "CCondVar.hpp"
#include "CSignalSemaphoreInterproc.hpp"
#include "Channel.hpp"
#include "common.hpp"


class ChannelClient : public iviLink::Channel::ChannelHandler
                     , public CThread
{
   public:

    /**
     * Constructor
     */
    ChannelClient();

    /**
     * Destructor
     */
    ~ChannelClient();

    // from IChannelObserver

    ///from CChannelHandler
    virtual void onBufferReceived(const iviLink::Channel::tChannelId channel, iviLink::Buffer const& buffer);
    virtual void onChannelDeleted(const UInt32 channel_id);
    virtual void onConnectionLost();
   
    // from CThread
    /**
     * Thread routine
     */
    virtual void threadFunc();


private:

    void sync_test(const char* msg = "");
      
    bool smoke_test(); //checks basic functionality: allocate-send-deallocate
    bool smoke_test_with_errors(); //the same but with some error handlig checks
    bool smoke_test_core(bool errorChecking);
   
    bool alloc_empty_name(); // try to allocate channel with tag ""
   
    bool send_big_data_test(); // try to send MAX_SIZE buf
    bool allocate_same_name_twice(); //try to allocate-deallocate-allocate-deallocate channel with same tag
   
    bool send_data_stress_test_core(bool confirm);
    bool send_data_stress_test_no_confirmation(); //sends lots of small data without confirmation of receiving from server
    bool send_data_stress_test_with_confirmation(); //sends lots of small data with confirmation of receiving from server
   
    bool alloc_dealloc_stress_test_core(bool confirm);
    bool alloc_dealloc_stress_test_no_confirmation(); //allocates several channels in a row and deallocates them, success of these operations is not confirmed from the other side
    bool alloc_dealloc_stress_test_with_confirmation(); //allocates several channels in a row and deallocates them, success of these operations is confirmed from the other side
   
    iviLink::Channel::tChannelId cid;
    iviLink::Channel::tChannelId mCids[gAttemptsLimit];
   
    static Logger logger;

    CSignalSemaphoreInterproc mSrvSyncSem;
    CSignalSemaphoreInterproc mCliSyncSem;
};
#endif //CHANNEL_CLIENT_HPP
