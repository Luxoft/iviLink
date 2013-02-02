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


/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <cassert>
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "tests/component/framework/components/ChannelSupervisor/TestServer/CServer.hpp"
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "framework/components/ChannelSupervisor/Tube/ChannelSupervisorTube.hpp"
#include "utils/misc/CError.hpp"
#include "framework/components/ChannelSupervisor/common/CSError.hpp"
#include "utils/misc/Types.hpp"

using iviLink::ChannelSupervisor::CSError;
static UInt32 getMaxBufSize()
{
	return MAX_SIZE;
}
CServer::CServer() : 
   CThread("TestServerThread"),
   mSrvSyncSem("/channel_supervisor_test_server"),
   mCliSyncSem("/channel_supervisor_test_client")
{
   mDataBuf = new UInt8[::getMaxBufSize()];
   mDataBufReceive = new UInt8[::getMaxBufSize()];
}
CServer::~CServer()
{
   if(mDataBuf) delete [] mDataBuf;
   if(mDataBufReceive) delete [] mDataBufReceive;
}

void CServer::dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size)
{
   Log("CServer::dataReceivedCallback(): available %d bytes on %d channel", read_size, channel_id);
   UInt32 received = 0;
   iviLink::ChannelSupervisor::receiveData(channel_id, "SeatControlAPI",mDataBufReceive, received,::getMaxBufSize());
   Log("CServer::dataReceivedCallback()=>read %d bytes", received);
}

void CServer::bufferOverflowCallback(const UInt32 channel_id)
{
   Log("CServer::bufferOverflowCallback()=>try to read from %d channel buffer...", channel_id);
   UInt32 received = 0;
   iviLink::ChannelSupervisor::receiveData(channel_id, "SeatControlAPI",mDataBufReceive, received,::getMaxBufSize());
   Log("CServer::bufferOverflowCallback()=>read %d bytes", received);
}

void CServer::channelDeletedCallback(const UInt32 channel_id)
{
   Log("CServer::channelDeletedCallback() - %d channel deleted!", channel_id);

   mDeleteCond.lock();
   mDeletedChannels.insert(channel_id);
   mDeleteCond.broadcast();
   mDeleteCond.unlock();
}

void CServer::connectionLostCallback()
{
   Log("CServer::connectionLostCallback()");
}
void CServer::ChannelSupervisor_04()//Trying to send by channel already in use
{
   TRACE();
   UInt32 mChannelID;
   UInt32 sending = 1;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_04", mChannelID);
   if (ret.isNoError())
   {
      Log("CServer::ChannelSupervisor_04 allocation done ");
   }
   else
   {
      Log("CServer::ChannelSupervisor_04 failed! problems with channel allocating. Error code - %d ", ret.getCode());
   }

}
void CServer::ChannelSupervisor_07()//Sending data to with Null pointer to data 
{
   TRACE();
   UInt32 sending = 1;
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ClimateProfileAPI_07", mChannelID);
   if (ret.isNoError())
   {
      Log("CServer::ChannelSupervisor_07 allocation done ");
   }
   else
   {
     Log("CServer::ChannelSupervisor_07 FAILED! problems with channel allocating %d", ret.getCode());
   }

}
void CServer::ChannelSupervisor_08()////Sending data with correct data pointer and minimum data size value 
{
   TRACE();
   UInt32 sending = 1;
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ClimateProfileAPI_08", mChannelID);
   if (ret.isNoError())
   {
      Log("CServer::ChannelSupervisor_08 allocation done ");
   }
   else
   {
     Log("CServer::ChannelSupervisor_08 FAILED! problems with channel allocating %d", ret.getCode());
   }

}

void CServer::ChannelSupervisor_09()//0-size data to send 
{
   TRACE();
   UInt32 mChannelID;
   UInt32 sending = 0;
   BaseError ret = iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_09", mChannelID);
   if (ret.isNoError())
   {
      Log("CServer::ChannelSupervisor_09 allocation done ");
   }
   else
   {
      Log("CServer::ChannelSupervisor_09 failed! problems with channel allocating %d", ret.getCode());
   }

}
void CServer::ChannelSupervisor_15()//Recieve data from correctly allocated channel
{
   TRACE();
   UInt32 mChannelID;
     BaseError ret = iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_15", mChannelID);
   if(ret.isNoError())
   {
      char* str = "ChannelSupervisor_15 test data!\0";
      ret =  iviLink::ChannelSupervisor::sendData(mChannelID, "SeatControlAPI_15", reinterpret_cast<UInt8*>(str), strlen(str));
      Log("SendData TC_15 : error %d", ret.getCode());
   }
   else
   {
      Log("AllocateChannel (1) failed: error %d", ret.getCode());
   }
}


void CServer::ChannelSupervisor_18()//Check for unable to send request with null pointer to data
{
   TRACE();
   UInt32 mChannelID;
   UInt32 received = 1;
  // UInt32 bufSize = ::getMaxBufSize();
   
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_18", mChannelID);
   if (!ret.isNoError())
   {
     Log("CServer::ChannelSupervisor_18 allocation done ");
 
   }
   else
   {
      Log("CClient::ChannelSupervisor_18 - FAILED %d",ret.getCode());
   }
}
void CServer::ChannelSupervisor_21()//Check for data buffer size allow take UINT_MAX value for reciveData() received parameter
{
   TRACE();
   UInt32 mChannelID;
   BaseError ret = iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_21", mChannelID);
   if(ret.isNoError())
   {
      char* str = "ChannelSupervisor_21 test data!\0";

     ret =  iviLink::ChannelSupervisor::sendData(mChannelID, "SeatControlAPI_21", reinterpret_cast<UInt8*>(str), strlen(str));
      Log("sendData TC_21: error %d", ret.getCode());

   }
   else
   {
      Log("allocateChannel  (1) failed: error %d", ret.getCode());
   }
}
void CServer::ChannelSupervisor_22()//Check for data buffer size isn't “0” value for reciveData()
{
   TRACE();
   UInt32 mChannelID;
   UInt32 received = 1;
   UInt32 bufferSize = 0;   
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_22", mChannelID);
   if (ret.isNoError())
   {
      Log("CServer::ChannelSupervisor_22 allocation done ");
   }
   else
   {
      Log("CClient::ChannelSupervisor_22 FAILED! problems with channel allocating %d", ret.getCode());
   }

}
void CServer::ChannelSupervisor_28() //Correct allocating channel
{
   TRACE();
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_28", mChannelID);
   if (ret.isNoError())
   {
      Log("CClient::ChannelSupervisor_28 - allocation done ");
   }
   else
   {
      Log("CClient::ChannelSupervisor_28: FAILED %d", ret.getCode());
   }

}

void CServer::ChannelSupervisor_33()//Successful channel close
{
   TRACE();

   const char tag[] = "SeatControlAPI_33";
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this, tag, mChannelID);

   sync_test("ChannelSupervisor_33 alloc sync");

   if(ret.isNoError())
   {
      Log("CClient::ChannelSupervisor_33 - allocation done ");

      mDeleteCond.lock();

      while (mDeletedChannels.find(mChannelID) == mDeletedChannels.end())
         mDeleteCond.wait();

      Log("CClient::ChannelSupervisor_33 - was channel deleted callback");
      mDeletedChannels.erase(mChannelID);

      mDeleteCond.unlock();

      ret = iviLink::ChannelSupervisor::deallocateChannel(mChannelID, tag);
      Log("CClient::ChannelSupervisor_33 - deallocateChannel err = %s", static_cast<std::string>(ret).c_str());
   }
   else
   {
      Log("CClient::ChannelSupervisor_33 Allocation channel- FAILED %d", ret.getCode());
   }
}

void CServer::ChannelSupervisor_37() //Tag is empty
{
   TRACE();
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"", mChannelID);
   if (ret.isNoError())
   {
      Log("CClient::ChannelSupervisor_37 - allocation done ");
   }
   else
   {
      Log("CClient::ChannelSupervisor_37: FAILED %d", ret.getCode());
   }
}

void CServer::ChannelSupervisor_38() //Tag is empty
{
   TRACE();
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,0, mChannelID);
   if (ret.isNoError())
   {
      Log("CClient::ChannelSupervisor_38 - allocation done ");
   }
   else
   {
      Log("CClient::ChannelSupervisor_38: FAILED %d", ret.getCode());
   }
}

void CServer::ChannelSupervisor_39() //Channel ID is 0
{
   TRACE();
   UInt32 mChannelID = 0;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ChannelSupervisor_39", mChannelID);
   if (ret.isNoError())
   {
      Log("CClient::ChannelSupervisor_39 - allocation done ");
   }
   else
   {
      Log("CClient::ChannelSupervisor_39: FAILED %d", ret.getCode());
   }
}

void CServer::ChannelSupervisor_52() //init with all params OK
{
   TRACE();
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::init(this, "ChannelSupervisor_52");
   if (ret.isNoError())
   {
      Log("CClient::ChannelSupervisor_52 - allocation done ");
   }
   else
   {
      Log("CClient::ChannelSupervisor_52: FAILED %d", ret.getCode());
   }
}

void CServer::ChannelSupervisor_53() //init with observer null pointer
{
   TRACE();
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::init(0, "ChannelSupervisor_52");
   if (ret.isNoError())
   {
      Log("CClient::ChannelSupervisor_53 - allocation done ");
   }
   else
   {
      Log("CClient::ChannelSupervisor_53: FAILED %d", ret.getCode());
   }
}

void CServer::ChannelSupervisor_54() //init with 0 TAG
{
   TRACE();
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::init(this, 0);
   if (ret.isNoError())
   {
      Log("CClient::ChannelSupervisor_54 - allocation done ");
   }
   else
   {
      Log("CClient::ChannelSupervisor_54: FAILED %d", ret.getCode());
   }
}

void CServer::ChannelSupervisor_55() //init with all params OK
{
   TRACE();
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::init(this, "");
   if (ret.isNoError())
   {
      Log("CClient::ChannelSupervisor_55 - allocation done ");
   }
   else
   {
      Log("CClient::ChannelSupervisor_55: FAILED %d", ret.getCode());
   }
}

void CServer::threadFunc()
{
   sync_test("initial");

   // both_sides_close_test();            sync_test("both_sides_close_test end");
   // one_side_close_test();              sync_test("one_side_close_test end");
   // one_side_close_same_thread_test();  sync_test("one_side_close_same_thread_test end");
   multiple_open_close_same_tag_test();sync_test("multiple_open_close_same_tag_test end");

    ChannelSupervisor_04(); sync_test("ChannelSupervisor_04 end");
    ChannelSupervisor_07(); sync_test("ChannelSupervisor_07 end");
    ChannelSupervisor_08(); sync_test("ChannelSupervisor_08 end");
    ChannelSupervisor_09(); sync_test("ChannelSupervisor_09 end");
    ChannelSupervisor_15(); sync_test("ChannelSupervisor_15 end");
    ChannelSupervisor_18(); sync_test("ChannelSupervisor_18 end");
    ChannelSupervisor_21(); sync_test("ChannelSupervisor_21 end");
    ChannelSupervisor_22(); sync_test("ChannelSupervisor_22 end");
    ChannelSupervisor_28(); sync_test("ChannelSupervisor_28 end");
    ChannelSupervisor_33(); sync_test("ChannelSupervisor_33 end");

   sync_test("END TEST");
}


void CServer::sync_test(const char* msg/* = ""*/)
{
   Log("CServer BEFORE sync_test: %s", msg);

   mCliSyncSem.signal();
   mSrvSyncSem.wait();

   Log("CServer AFTER  sync_test: %s", msg);
}


void CServer::both_sides_close_test()
{
   TRACE();

   const char tag[] = "both_sides_close_test";
   UInt32 chid = 0;

   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this, tag, chid);
   sync_test("both_sides_close_test alloc sync");

   Log("both_sides_close_test alloc ret = %s", static_cast<std::string>(ret).c_str());

   ret = iviLink::ChannelSupervisor::deallocateChannel(chid, tag);
   Log("both_sides_close_test deallocateChannel err = %s", static_cast<std::string>(ret).c_str());

   sync_test("both_sides_close_test dealloc sync");
}


void CServer::one_side_close_test()
{
   TRACE();

   mDeleteCond.lock();
   mDeletedChannels.clear();
   mDeleteCond.unlock();

   const char tag[] = "one_side_close_test";
   UInt32 chid = 0;

   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this, tag, chid);
   sync_test("one_side_close_test alloc sync");

   Log("one_side_close_test alloc ret = %s", static_cast<std::string>(ret).c_str());
   if (!ret.isNoError())
      return;

   Log("one_side_close_test waiting for channel closed");
   mDeleteCond.lock();
   while (mDeletedChannels.empty())
      mDeleteCond.wait();
   assert(mDeletedChannels.size() == 1 && *mDeletedChannels.begin() == chid);
   mDeleteCond.unlock();

   ret = iviLink::ChannelSupervisor::deallocateChannel(chid, tag);
   Log("one_side_close_test deallocateChannel err = %s", static_cast<std::string>(ret).c_str());

   sync_test("one_side_close_test dealloc sync");
}

void CServer::one_side_close_same_thread_test()
{
   TRACE();

   static const char tag[] = "one_side_close_same_thread_test";

   struct Observer : public IChannelSupervisorTubeObserver
   {
      virtual void dataReceivedCallback(const unsigned int channelId, const unsigned int readSize)
      {
         assert(false);
      }

      virtual void bufferOverflowCallback(const unsigned int channelId) 
      {
         assert(false);
      }

      virtual void channelDeletedCallback(const unsigned int channelId)
      {
         // deleting channel from the thread of callback

         BaseError ret = iviLink::ChannelSupervisor::deallocateChannel(channelId, tag);
         Log("one_side_close_same_thread_test deallocateChannel err = %s", static_cast<std::string>(ret).c_str());

         deletedChannel = channelId;
         sem.signal();
      }

      virtual void connectionLostCallback()
      {
         assert(false);
      }

      CSignalSemaphore sem;
      UInt32 deletedChannel;
   };

   Observer obs;

   
   UInt32 chid = 0;

   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(&obs, tag, chid);
   sync_test("one_side_close_same_thread_test alloc sync");

   Log("one_side_close_same_thread_test alloc ret = %s", static_cast<std::string>(ret).c_str());

   Log("one_side_close_same_thread_test waiting for channel closed");
   obs.sem.wait();

   assert(obs.deletedChannel == chid);

   sync_test("one_side_close_same_thread_test dealloc sync");
}

void CServer::multiple_open_close_same_tag_test()
{
   TRACE();

   const char tag[] = "multiple_open_close_same_tag_test";
   

   for (int i = 0 ; i < 10; ++i)
   {
      UInt32 chid = 0;

      BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this, tag, chid);
      sync_test("multiple_open_close_same_tag_test alloc sync");

      Log("multiple_open_close_same_tag_test alloc ret = %s", static_cast<std::string>(ret).c_str());

      ret = iviLink::ChannelSupervisor::deallocateChannel(chid, tag);
      Log("multiple_open_close_same_tag_test deallocateChannel err = %s", static_cast<std::string>(ret).c_str());

      sync_test("multiple_open_close_same_tag_test dealloc sync");
   }
}
