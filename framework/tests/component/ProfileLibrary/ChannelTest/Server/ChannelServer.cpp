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


#include <cstring>

#include "ChannelServer.hpp"
#include "Channel.hpp"

#include "Logger.hpp"
#include "PublicError.hpp"
#include "Types.hpp"

Logger ChannelServer::logger = Logger::getInstance(LOG4CPLUS_TEXT("ProfileLibTest.Server"));

int allocStressTestCounter = 0;

static UInt32 getMaxBufSize()
{
    return MAX_SIZE;
}

ChannelServer::ChannelServer()
    : CThread("TestServerThread")
    , mSrvSyncSem(gSemServerName.c_str())
    , mCliSyncSem(gSemClientName.c_str())
{
}

ChannelServer::~ChannelServer()
{
}

void ChannelServer::onBufferReceived(const iviLink::Channel::tChannelId channel, iviLink::Buffer const& buffer)
{
    LOG4CPLUS_INFO(logger, "buffer received!");
    if (SMOKE_TEST_ID == currentTest)
    {
        UInt8 a, b, c;
        buffer.read(a);
        buffer.read(b);
        buffer.read(c);
        if(a == 1 && b==2 && c == 3)
        {
            LOG4CPLUS_INFO(logger, "smoke_test bufferReceived success");
            mCliSyncSem.signal();
        }
        else
        {
            LOG4CPLUS_INFO(logger, "smoke_test bufferReceived got corrupted data");
        }
    }

    if (BIG_DATA_TEST_ID == currentTest)
    {
        UInt8 a;
        for(UInt32 i=0; i<getMaxBufSize(); i++)
        {
            buffer.read(a);
            if (1 != a)
            {
                LOG4CPLUS_INFO(logger, "send_big_data_test bufferReceived got corrupted data");
                return;
            }
        }
        LOG4CPLUS_INFO(logger, "send_big_data_test bufferReceived success");
        mCliSyncSem.signal();
    }

    if (SEND_STRESS_TEST_ID == currentTest)
    {
        LOG4CPLUS_INFO(logger, "send_data_stress_test_with_confirmation confirming receiving of data");
        mCliSyncSem.signal();
    }
}

void ChannelServer::onChannelDeleted(const UInt32 channel_id)
{
    LOG4CPLUS_INFO(logger, "channel deleted");
    if (!iviLink::Channel::deallocateChannel(channel_id).isNoError())
    {
        LOG4CPLUS_ERROR(logger, "for some reason deallocateChannel returned error, please check the issue!");
    }
    if (ALLOC_STRESS_TEST_NO_CONFIRM_ID == currentTest)
    {
        allocStressTestCounter++;
        LOG4CPLUS_INFO(logger, "counting deallocated channels: "+convertIntegerToString(allocStressTestCounter));
        if(allocStressTestCounter == gAttemptsLimit)
        {
            LOG4CPLUS_INFO(logger, "all channels deallocated");
            mCliSyncSem.signal();
        }
    }
    else
    {
        LOG4CPLUS_INFO(logger, "notifying client of deallocation");
        mCliSyncSem.signal();
    }
}

void ChannelServer::onConnectionLost()
{
    LOG4CPLUS_ERROR(logger, "connectionLostCallback");
}

void ChannelServer::threadFunc()
{
    sync_test("start testing");
    smoke_test();sync_test("after smoke_test");
    allocate_same_name_twice();sync_test("after alloc_same_name_twice");
    send_data_stress_test_with_confirmation();sync_test("after send_data_stress_test_with_confirmation");
    send_data_stress_test_no_confirmation(); sync_test("after send_data_stress_test_no_confirmation");
    //send_big_data_test();sync_test("after send_big_data_test");
    alloc_dealloc_stress_test_with_confirmation();sync_test("after alloc_dealloc_stress_test_with_confirmation");
    alloc_dealloc_stress_test_no_confirmation();sync_test("after alloc_dealloc_stress_test_no_confirmation");
    smoke_test();sync_test("after smoke_test_with_errors");
    if(alloc_empty_name())
    {
        sync_test("after alloc_empty_name");
    }
    else
    {
        LOG4CPLUS_FATAL(logger, "alloc_empty_name failed!");
        exit(1);
    }
    sync_test("end");
}

void ChannelServer::sync_test(const char* msg/* = ""*/)
{
    std::string message = std::string("before sync test: ")+std::string(msg);
    LOG4CPLUS_INFO(logger, message);
    mCliSyncSem.signal();
    mSrvSyncSem.wait();
    message = std::string("after sync test: ")+std::string(msg);
    LOG4CPLUS_INFO(logger, message);
}

void ChannelServer::smoke_test()
{
    currentTest = SMOKE_TEST_ID;
    iviLink::Error error = iviLink::Channel::allocateChannel(gSmokeTest, this, cid);
    if (!error.isNoError() || cid == 0)
    {
        LOG4CPLUS_ERROR(logger, "smoke test allocation failed");
        return;
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "smoke test allocation success, signaling to client");
    }
    mCliSyncSem.signal();
    mSrvSyncSem.wait();
}

void ChannelServer::allocate_same_name_twice()
{
    currentTest = ALLOC_SAME_TWICE_ID;
    for (int i=0; i<2; ++i)
    {
        iviLink::Error error = iviLink::Channel::allocateChannel(gDoubleAlloc, this, cid);
        if (!error.isNoError() || cid == 0)
        {
            LOG4CPLUS_ERROR(logger, "allocate_same_name_twice channel allocation failed! Step No. "+convertIntegerToString(i));
            return;
        }
        mCliSyncSem.signal();
        mSrvSyncSem.wait();
    }
}

bool ChannelServer::alloc_empty_name()
{
    iviLink::Error error = iviLink::Channel::allocateChannel("", this, cid);
    if (error.isNoError() &&  0 != cid)
    {
        LOG4CPLUS_ERROR(logger, "managed to allocate channel with empty tag");
        return false;
    }
    return true;
}

void ChannelServer::send_data_stress_test_core(bool confirm)
{
    iviLink::Error error = iviLink::Channel::allocateChannel(gSendData, this, cid);
    if (!error.isNoError() || cid == 0)
    {
        LOG4CPLUS_ERROR(logger, "send_data_stress_test allocation failed");
        return;
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "send_data_stress_test allocation successful, signaling to client");
    }
    mCliSyncSem.signal();
    mSrvSyncSem.wait();
}

void ChannelServer::send_data_stress_test_no_confirmation()
{
    currentTest = SEND_STRESS_TEST_NO_CONFIRMED_ID;
    send_data_stress_test_core(false);
}

void ChannelServer::send_data_stress_test_with_confirmation()
{
    currentTest = SEND_STRESS_TEST_ID;
    send_data_stress_test_core(true);
}
 
void ChannelServer::alloc_dealloc_stress_test_core(bool confirm)
{
    for (int i=0; i<gAttemptsLimit; i++)
    {
        iviLink::Error error = iviLink::Channel::allocateChannel((confirm? gMultipleAlDealC : gMultipleAlDeal)+convertIntegerToString(i), this, mCids[i]);
        if (!error.isNoError() || mCids[i]==0)
        {
            LOG4CPLUS_ERROR(logger, "alloc_dealloc_stress_test failed in allocation stage! Step No." + convertIntegerToString(i));
            return;
        }
        if (confirm)
        {
            mCliSyncSem.signal();
        }
    }
    if (confirm)
    {
        sync_test("alloc_dealloc_stress_test between stages");
    }
    mSrvSyncSem.wait();
}

void ChannelServer::alloc_dealloc_stress_test_no_confirmation()
{
    currentTest=ALLOC_STRESS_TEST_NO_CONFIRM_ID;
    alloc_dealloc_stress_test_core(false);
}

void ChannelServer::alloc_dealloc_stress_test_with_confirmation()
{
    currentTest = ALLOC_STRESS_TEST_ID;
    alloc_dealloc_stress_test_core(true);
}

void ChannelServer::send_big_data_test()
{
    currentTest = BIG_DATA_TEST_ID;
    iviLink::Error error = iviLink::Channel::allocateChannel(gBigData, this, cid);
    if (!error.isNoError() || cid == 0)
    {
        LOG4CPLUS_ERROR(logger, "send_big_data_test allocation failed");
        return;
    }
    else
    {
        LOG4CPLUS_ERROR(logger, "send_big_data_test allocation success, signaling to client to proceed");
    }
    mCliSyncSem.signal();
    mSrvSyncSem.wait();
}

