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


#ifndef CHANNEL_SERVER_HPP
#define CHANNEL_SERVER_HPP

#include "CThread.hpp"
#include "CCondVar.hpp"
#include "CSignalSemaphoreInterproc.hpp"
#include "Channel.hpp"
#include "../common.hpp"
#include "../enum.hpp"


class ChannelServer: public iviLink::Channel::ChannelHandler, public CThread
{
    public:

    /**
     * Constructor
     */
    ChannelServer();

    /**
     * Destructor
     */
    ~ChannelServer();

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
    // see ChannelClient.hpp for comments
    void smoke_test();
    bool alloc_empty_name();
    void send_big_data_test();
    void allocate_same_name_twice();
   
    void send_data_stress_test_core(bool confirm);
    void send_data_stress_test_no_confirmation();
    void send_data_stress_test_with_confirmation();
   
    void alloc_dealloc_stress_test_core(bool confirm);
    void alloc_dealloc_stress_test_no_confirmation();
    void alloc_dealloc_stress_test_with_confirmation();
   
    static Logger logger;
    TestId currentTest;

    iviLink::Channel::tChannelId cid;
    iviLink::Channel::tChannelId mCids[gAttemptsLimit];

    CSignalSemaphoreInterproc mSrvSyncSem;
    CSignalSemaphoreInterproc mCliSyncSem;
};
#endif //CHANNEL_SERVER_HPP
