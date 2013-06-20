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

#include "ChannelClient.hpp"
#include "Channel.hpp"

#include "Logger.hpp"
#include "BaseError.hpp"
#include "Types.hpp"
#include "PublicError.hpp"

Logger ChannelClient::logger = Logger::getInstance(LOG4CPLUS_TEXT("ProfileLibTest.Client"));

static UInt32 getMaxBufSize()
{
    return MAX_SIZE;
}

ChannelClient::ChannelClient()
    :  CThread("TestClientThread")
    , mSrvSyncSem(gSemServerName.c_str())
    , mCliSyncSem(gSemClientName.c_str())
{
}

ChannelClient::~ChannelClient()
{
}

void ChannelClient::onBufferReceived(const iviLink::Channel::tChannelId channel, iviLink::Buffer const& buffer)
{
    LOG4CPLUS_INFO(logger, "bufferReceived callback");
}

void ChannelClient::onChannelDeleted(const UInt32 channel_id)
{
    LOG4CPLUS_INFO(logger, "channelDeletedCallback callback");
    iviLink::Error err = iviLink::Channel::deallocateChannel(channel_id);
    if (!err.isNoError())
    {
        LOG4CPLUS_INFO(logger, "channelDeletedCallback callback - consequent deallocation returned with error!");
    }
}

void ChannelClient::onConnectionLost()
{
    LOG4CPLUS_INFO(logger, "connectionLostCallback callback");
}

void ChannelClient::threadFunc()
{
    sync_test("start testing");
    if (smoke_test())
    {
        sync_test("after smoke_test");
    }
    else
    {
        LOG4CPLUS_FATAL(logger, "smoke_test failed!");
        exit(1);
    }
   
    if (allocate_same_name_twice())
    {
        sync_test("after allocate_same_name_twice");
    }
    else
    {
        LOG4CPLUS_FATAL(logger, "smoke_test failed!");
        exit(1);
    }
   
    if (send_data_stress_test_with_confirmation())
    {
        sync_test("after send_data_stress_test_with_confirmation");
    }
    else
    {
        LOG4CPLUS_FATAL(logger, "send_data_stress_test_with_confirmation failed!");
        exit(1);
    }
   
    if (send_data_stress_test_no_confirmation())
    {
        sync_test("after send_data_stress_test_no_confirmation");
    }
    else
    {
       LOG4CPLUS_FATAL(logger, "send_data_stress_test_no_confirmation failed!");
       exit(1);
    }
    //send_big_data_test();sync_test("after send_big_data_test");
    if (alloc_dealloc_stress_test_with_confirmation())
    {
        sync_test("after alloc_dealloc_stress_test_with_confirmation");
    }
    else
    {
        LOG4CPLUS_FATAL(logger, "alloc_dealloc_stress_test_with_confirmation failed!");
        exit(1);
    }
   
    if (alloc_dealloc_stress_test_no_confirmation())
    {
        sync_test("after alloc_dealloc_stress_test_no_confirmation");
    }
    else
    {
       LOG4CPLUS_FATAL(logger, "alloc_dealloc_stress_test_no_confirmation failed!");
       exit(1);
    }
   
    if (smoke_test_with_errors())
    {
        sync_test("after smoke_test_with_errors");
    }
    else
    {
        LOG4CPLUS_FATAL(logger, "smoke_test_with_errors failed!");
        exit(1);
    }

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

void ChannelClient::sync_test(const char* msg/* = ""*/)
{
    std::string message = std::string("before sync test: ")+std::string(msg);
    LOG4CPLUS_INFO(logger, message);
    mSrvSyncSem.signal();
    mCliSyncSem.wait();
    message = std::string("after sync test: ")+std::string(msg);
    LOG4CPLUS_INFO(logger, message);
}

bool ChannelClient::smoke_test_core(bool testErrorHandling)
{   
    iviLink::Error error  = iviLink::Channel::allocateChannel(gSmokeTest, this,cid);
    if (!error.isNoError() || cid == 0)
    {
        LOG4CPLUS_ERROR(logger, "channel allocation failed");
        return false;
    }
    if (-1==mCliSyncSem.waitTimeout(gTimeout))
    {
        LOG4CPLUS_ERROR(logger, "no confirmation of allocating channel from server within time limit");
        return false;
    }
    else
    {
        LOG4CPLUS_INFO(logger, "other side confirmed allocation!");
    }

    UInt8 outgoingData[3];
    iviLink::Buffer buf(outgoingData, 3);
    bool res = true;
    res = res && buf.write(static_cast<UInt8>(1));
    res = res && buf.write(static_cast<UInt8>(2));
    res = res && buf.write(static_cast<UInt8>(3));
    LOG4CPLUS_ERROR(logger, "about to send buffer to the other side");

    if (testErrorHandling)
    {
        //trying to send data to a not allocated channel
        iviLink::Error eerr = iviLink::Channel::sendBuffer(cid+1, buf);
        if (eerr.isNoError())
        {
            LOG4CPLUS_ERROR(logger, "send data to non-allocated channel returned noError, error handling check failed!");
            mSrvSyncSem.signal();
            return false;
        }
    }

    //sending buffer
    iviLink::Error err = iviLink::Channel::sendBuffer(cid, buf);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(logger, "send data failed!");
        mSrvSyncSem.signal();
        return false;
    }
    else
    {
        LOG4CPLUS_INFO(logger, "sent buffer to the other side! Waiting for confirmation of receiving");
    }
    if(-1==mCliSyncSem.waitTimeout(gTimeout))
    {
        LOG4CPLUS_ERROR(logger, "no confirmation of receiving data from server within time limit!");
        mSrvSyncSem.signal();
        return false;
    }
    else
    {
        LOG4CPLUS_INFO(logger, "other side successfully received sent data!");
        if (testErrorHandling)
        {
            // trying to deallocate not allocated channel
            err = iviLink::Channel::deallocateChannel(cid+1);
            if (err.isNoError())
            {
                LOG4CPLUS_ERROR(logger, "dealocation of not opened channel succeeded!");
                mSrvSyncSem.signal();
                return false;
            }
        }
        err = iviLink::Channel::deallocateChannel(cid);
        if (!err.isNoError())
        {
            LOG4CPLUS_ERROR(logger, "channel deallocation failed!");
            mSrvSyncSem.signal();
            return false;
        }

        if (testErrorHandling)
        {
            //trying to send data to deallocated channel
            err = iviLink::Channel::sendBuffer(cid, buf);
            if (err.isNoError())
            {
                LOG4CPLUS_ERROR(logger, "send data to deallocated channel succeeded!");
                mSrvSyncSem.signal();
                return false;
            }
        }
    }

    if (-1==mCliSyncSem.waitTimeout(gTimeout))
    {
        LOG4CPLUS_ERROR(logger, "no confirmation of deallocating channel from server within time limit");
        mSrvSyncSem.signal();
        return false;
    }
    LOG4CPLUS_INFO(logger, "other side confirmed deallocation! Test passed!");
    mSrvSyncSem.signal();
    return true;
}

bool ChannelClient::smoke_test()
{
    return smoke_test_core(false);
}

bool ChannelClient::smoke_test_with_errors()
{
    return smoke_test_core(true);
}

bool ChannelClient::alloc_empty_name()
{
    iviLink::Error err = iviLink::Channel::allocateChannel("", this, cid);
    if (err.isNoError())
    {
        LOG4CPLUS_ERROR(logger, "succeeded to allocated channel with empty tag");
        return false;
    }
    return true;
}

bool ChannelClient::allocate_same_name_twice()
{
    for (int i=0; i<2; i++)
    {
        iviLink::Error error = iviLink::Channel::allocateChannel(gDoubleAlloc, this, cid);
        if (!error.isNoError())
        {
            LOG4CPLUS_ERROR(logger, "allocate_same_name_twice channel allocation failed! Step No. "+convertIntegerToString(i));
            return false;
        }
        if (-1 == mCliSyncSem.waitTimeout(gTimeout))
        {
            LOG4CPLUS_ERROR(logger, "allocate_same_name_twice no confirmation of allocating channel from server within time limit! Step No. "+convertIntegerToString(i));
            return false;
        }
        else
        {
            LOG4CPLUS_INFO(logger, "allocate_same_name_twice other side confirmed allocation! Step No. "+convertIntegerToString(i));
        }
        iviLink::Error err = iviLink::Channel::deallocateChannel(cid);
        if (!err.isNoError())
        {
            LOG4CPLUS_ERROR(logger, "allocate_same_name_twice channel deallocation failed! Step No. "+convertIntegerToString(i));
            mSrvSyncSem.signal();
            return false;
        }
        if(-1 == mCliSyncSem.waitTimeout(gTimeout))
        {
            LOG4CPLUS_ERROR(logger, "allocate_same_name_twice no confirmation of deallocating channel from server within time limit! Step No. "+convertIntegerToString(i));
            mSrvSyncSem.signal();
            return false;
        }
        LOG4CPLUS_INFO(logger, "allocate_same_name_twice other side confirmed deallocation! Step No. "+convertIntegerToString(i));
        mSrvSyncSem.signal();
    }
    LOG4CPLUS_INFO(logger, "Test passed!");
    return true;
}
   
bool ChannelClient::alloc_dealloc_stress_test_core(bool confirmation)
{
    for (int i=0;i<gAttemptsLimit;i++)
    {
        iviLink::Error error = iviLink::Channel::allocateChannel((confirmation? gMultipleAlDealC : gMultipleAlDeal)+convertIntegerToString(i), this, mCids[i]);
        if(!error.isNoError() || mCids[i]==0)
        {
            LOG4CPLUS_ERROR(logger, "alloc_dealloc_stress_test failed in allocation stage!");
            return false;
        }
        if(confirmation)
        {
            if(-1 == mCliSyncSem.waitTimeout(gTimeout))
            {
                LOG4CPLUS_ERROR(logger, "alloc_dealloc_stress_test no confirmation of allocating channel from server within time limit");
                return false;
            }
        }
    }

    if (confirmation)
    {
        sync_test("alloc_dealloc_stress_test between stages");
    }

    for (int i=0;i<gAttemptsLimit; i++)
    {
        iviLink::Error err = iviLink::Channel::deallocateChannel(mCids[i]);
        if(!err.isNoError())
        {
            LOG4CPLUS_ERROR(logger, "alloc_dealloc_stress_test failed in deallocation stage!");
            return false;
        }
        if(confirmation)
        {
            if(-1 == mCliSyncSem.waitTimeout(gTimeout))
            {
                LOG4CPLUS_ERROR(logger, "alloc_dealloc_stress_test no confirmation of deallocating channel from server within time limit");
                return false;
            }
        }
    }
    if (!confirmation)
    {
        if(-1 == mCliSyncSem.waitTimeout(gTimeout*gAttemptsLimit))
        {
            LOG4CPLUS_ERROR(logger, "alloc_dealloc_stress_test no confirmation of complete deallocation from server within time limit");
            return false;
        }
        else
        {
            LOG4CPLUS_INFO(logger, "alloc_dealloc_stress_test test passed");
        }
    }
    mSrvSyncSem.signal();
    return true;
}

bool ChannelClient::alloc_dealloc_stress_test_no_confirmation()
{
    return alloc_dealloc_stress_test_core(false);
}

bool ChannelClient::alloc_dealloc_stress_test_with_confirmation()
{
    return alloc_dealloc_stress_test_core(true);
}


bool ChannelClient::send_data_stress_test_core(bool confirmation)
{
    iviLink::Error error = iviLink::Channel::allocateChannel(gSendData, this, cid);
    if (!error.isNoError() || cid == 0)
    {
        LOG4CPLUS_ERROR(logger, "channel allocation failed");
        return false;
    }
    if (-1==mCliSyncSem.waitTimeout(gTimeout))
    {
        LOG4CPLUS_ERROR(logger, "no confirmation of allocating channel from server within time limit");
        return false;
    }
    else
    {
        LOG4CPLUS_INFO(logger, "other side confirmed allocation!");
    }
    UInt8 outgoingData[3];
    iviLink::Buffer buf(outgoingData, 3);
    bool res = true;
    res = res && buf.write(static_cast<UInt8>(1));
    res = res && buf.write(static_cast<UInt8>(2));
    res = res && buf.write(static_cast<UInt8>(3));
    for (int i=0; i<gChunksLimit; i++)
    {
        if(!iviLink::Channel::sendBuffer(cid, buf).isNoError())
        {
            LOG4CPLUS_ERROR(logger, "error sending buffer! Attempt No."+convertIntegerToString(i));
            mSrvSyncSem.signal();
            return false;
        }
        if(confirmation)
        {
            if(-1==mCliSyncSem.waitTimeout(gTimeout))
            {
                LOG4CPLUS_ERROR(logger, "no confirmation of receiving data from server within time limit");
                mSrvSyncSem.signal();
                return false;
            }
        }
    }
    iviLink::Error err = iviLink::Channel::deallocateChannel(cid);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(logger, "channel deallocation failed!");
        mSrvSyncSem.signal();
        return false;
    }
    if(-1 == mCliSyncSem.waitTimeout(gTimeout))
    {
        LOG4CPLUS_ERROR(logger, "no confirmation of deallocation from server within time limit");
        mSrvSyncSem.signal();
        return false;
    }
    LOG4CPLUS_INFO(logger, "other side confirmed deallocation! Test passed!");
    mSrvSyncSem.signal();
    return true;
}

bool ChannelClient::send_data_stress_test_no_confirmation()
{
    return send_data_stress_test_core(false);
}
   
bool ChannelClient::send_data_stress_test_with_confirmation()
{
    return send_data_stress_test_core(true);
}

bool ChannelClient::send_big_data_test()
{
    iviLink::Error error = iviLink::Channel::allocateChannel(gBigData, this, cid);
    if (!error.isNoError() || cid == 0)
    {
        LOG4CPLUS_ERROR(logger, "send_big_data_test channel allocation failed");
        return false;
    }
    if (-1 == mCliSyncSem.waitTimeout(gTimeout))
    {
        LOG4CPLUS_ERROR(logger, "send_big_data_test no confirmation of allocating channel from server within time limit");
        return false;
    }
    else
    {
        LOG4CPLUS_INFO(logger, "send_big_data_test other side confirmed allocation!");
    }
    UInt8 outgoingData[getMaxBufSize()];
    iviLink::Buffer buf(outgoingData, getMaxBufSize());
    for (UInt32 i = 0; i<getMaxBufSize(); i++)
    {
        if(!buf.write(static_cast<UInt8>(1)))
        {
            LOG4CPLUS_ERROR(logger, "send_big_data_test error writing to buffer!");
            mSrvSyncSem.signal();
            return false;
        }
    }

    LOG4CPLUS_ERROR(logger, "send_big_data_test about to send buffer to the other side");
    iviLink::Error err = iviLink::Channel::sendBuffer(cid, buf);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(logger, "send_big_data_test send data failed!");
        mSrvSyncSem.signal();
        return false;
    }
    else
    {
        LOG4CPLUS_INFO(logger, "send_big_data_test sent buffer to the other side! Waiting for confirmation of receiving");
    }
    if (-1==mCliSyncSem.waitTimeout(gTimeout))
    {
        LOG4CPLUS_ERROR(logger, "send_big_data_test no confirmation of receiving data from server within time limit!");
        mSrvSyncSem.signal();
        return false;
    }
    else
    {
        LOG4CPLUS_INFO(logger, "send_big_data_test other side successfully received sent data!");
        iviLink::Error err = iviLink::Channel::deallocateChannel(cid);
        if(!err.isNoError())
        {
            LOG4CPLUS_ERROR(logger, "send_big_data_test channel deallocation failed!");
            mSrvSyncSem.signal();
            return false;
        }
    }
    if (-1==mCliSyncSem.waitTimeout(gTimeout))
    {
        LOG4CPLUS_ERROR(logger, "send_big_data_test no confirmation of deallocating channel from server within time limit");
        mSrvSyncSem.signal();
        return false;
    }

    LOG4CPLUS_INFO(logger, "send_big_data_test other side confirmed deallocation!");
    mSrvSyncSem.signal();
    return true;
}

