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
#include <time.h>
#include <unistd.h>
#include <cstring>
#include <limits.h>
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
 #include "tests/component/framework/components/ConnectivityAgent/TestClient/CClient.hpp"
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
 #include "utils/misc/Logger.hpp"
 #include "framework/components/ConnectivityAgent/generic/common/API.hpp"

Logger CClient::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.test.client.CClient"));

CClient::CClient() : 
   CThread("TestClientThread"),
   mSrvSyncSem("/connectivity_agent_test_server"),
   mCliSyncSem("/connectivity_agent_test_client")
{
   mDataBufReceive = new UInt8[::getMaxBufSize()];
   mDataBufSend = new UInt8[::getMaxBufSize()];
   data_avail[1] = 0;
}

CClient::~CClient()
{
   if(mDataBufReceive) delete [] mDataBufReceive;
   if(mDataBufSend) delete [] mDataBufSend;
}

void CClient::dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size)//, const UInt32 free_size)
{
   LOG4CPLUS_INFO(logger, "CClient::dataReceivedCallback(): available "
         + convertIntegerToString(read_size) + " bytes on "
         + convertIntegerToString(channel_id) + " channel | free space ");
//         + convertIntegerToString(free_size));
   UInt32 received = 0;
   mWaitCond.lock();
   data_avail[channel_id]+=read_size;
   mWaitCond.broadcast();
   mWaitCond.unlock();
   ////::receiveData(channel_id, mDataBufReceive, received, ::getMaxBufSize());
   ////Log("CClient::dataReceivedCallback()=>read %d bytes", received);
   //_hex_print(mDataBuf,received );
    ///mWaitCond.signal();
}

void CClient::bufferOverflowCallback(const UInt32 channel_id)
{
   LOG4CPLUS_INFO(logger, "CClient::bufferOverflowCallback()=>try to read from "
         + convertIntegerToString(channel_id) + " channel buffer...");
   UInt32 received = 0;
   ::receiveData(channel_id, mDataBufReceive, received, ::getMaxBufSize());
   LOG4CPLUS_INFO(logger, "CClient::bufferOverflowCallback()=>read "
         + convertIntegerToString(received) + " bytes");
}

void CClient::channelDeletedCallback(const UInt32 channel_id)
{
   LOG4CPLUS_INFO(logger, "CClient::channelDeletedCallback() - "
         + convertIntegerToString(channel_id) + " channel deleted!");

}

void CClient::connectionLostCallback()
{
   LOG4CPLUS_INFO(logger, "CClient::connectionLostCallback()");
}


void CClient::ConnectivityAgent_01()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_01 --- ");
   LOG4CPLUS_INFO(logger, "--- Simple channel allocation ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_01 - Passed");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_01 - Failed problems with channel allocating. Error code - " + convertIntegerToString((int)ret));;
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_02()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_02 --- ");
   LOG4CPLUS_INFO(logger, "--- Channel allocation with eSafetyCritical priority (0) ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;

   ERROR_CODE ret = ::allocateChannel(eSafetyCritical, channel_id, this);

   if (ret == ERR_OK)
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_02 - Passed");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_02 - Failed problems with channel allocating. Error code - " + convertIntegerToString((int)ret));;
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_03()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_03 --- ");
   LOG4CPLUS_INFO(logger, "--- Channel allocation with eRealTime priority (1) ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;

   ERROR_CODE ret = ::allocateChannel(eRealTime, channel_id, this);

   if (ret == ERR_OK)
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_03 - Passed");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_03 - Failed problems with channel allocating. Error code - " + convertIntegerToString((int)ret));;
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_04()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_04 --- ");
   LOG4CPLUS_INFO(logger, "--- Allocation of channel with id = 0 ---");

   UInt32 channel_id = 0;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_04 - Failed");;
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_04 - Passed");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_05()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_05 --- ");
   LOG4CPLUS_INFO(logger, "--- Allocation of channel with id = MAX VALUE ---");

   UInt32 channel_id = 0xFFFF;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_05 - Passed");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_05 - Failed");;
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_06()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_06 --- ");
   LOG4CPLUS_INFO(logger, "--- Allocation of channel with id = MAX VALUE + 1 ---");

   UInt32 channel_id = 0xFFFF + 1;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_06 - Failed");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_06 - Passed");;
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_07()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_07 --- ");
   LOG4CPLUS_INFO(logger, "--- Allocation of channel which is already in use ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      ret = ::allocateChannel(ePlainData, channel_id, this);

      if (ret == ERR_OK)
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_07 - Failed. Double channel allocating with same id ");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_07 - Passed");
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_07 - Failed. Error allocating channel for the first time");;
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_08()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_08 --- ");
   LOG4CPLUS_INFO(logger, "--- Channel allocation with NULL-pointer observer ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, NULL);

   if (ret == ERR_OK)
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_08 - Failed");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_08 - Passed");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_09()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_09 --- ");
   LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_09 - Failed. Test isn't yet implemented");
}

void CClient::ConnectivityAgent_10()
{
   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_10 --- ");
   LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_10 - Failed. Test isn't yet implemented");
}

void CClient::ConnectivityAgent_11()
{
   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_11 --- ");
   LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_11 - Failed. Test isn't yet implemented");
}

void CClient::ConnectivityAgent_12()
{
   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_12 --- ");
   LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_12 - Failed. Test isn't yet implemented");
}

void CClient::ConnectivityAgent_13()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_13 --- ");
   LOG4CPLUS_INFO(logger, "--- Close channel which id = 0 ---");

   UInt32 channel_id = 0;
   ERROR_CODE ret = ::closeChannel(channel_id);

   if(ret == ERR_NOTFOUND)
   {
      LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_13 - Passed");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_13 - Failed " + convertIntegerToString((int)ret));
   }
}

void CClient::ConnectivityAgent_14()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_14 --- ");
   LOG4CPLUS_INFO(logger, "--- Close channel which id = MAX VALUE ---");

   UInt32 channel_id = 0xFFFF;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      ret = ::closeChannel(channel_id);

      if (ret == ERR_OK)
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_07 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_07 - Failed Couldn't close allocated channel");
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_07 - Failed. Error allocating channel");;
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_15()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_15 --- ");
   LOG4CPLUS_INFO(logger, "--- Close not allocated channel ---");

   ERROR_CODE ret = ::closeChannel(rand() % 0xFFFF + 1);

   if (ret == ERR_OK)
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_15 - Failed");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_15 - Passed");
   }
}

void CClient::ConnectivityAgent_16()
{
   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_16 --- ");
   LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_16 - Failed. Test isn't yet implemented");
}

void CClient::ConnectivityAgent_17()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_17 --- ");
   LOG4CPLUS_INFO(logger, "--- Simple send data ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;
   UInt32 sending = 1;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      ret = ::sendData(channel_id, mDataBufSend, sending);

      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_17 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_17 - Failed " + convertIntegerToString((int)ret) );
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_17 failed! problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_18()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_18 --- ");
   LOG4CPLUS_INFO(logger, "--- Send data to channel with id = 0 ---");

   UInt32 channel_id =0;
   UInt32 sending = 1;

   ERROR_CODE ret = ::sendData(channel_id, mDataBufSend, sending);

   if ((ret == ERR_FAIL)||(ret == ERR_NOTFOUND))
   {
      LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_18 - Passed");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_18 - Failed " + convertIntegerToString((int)ret));
   }
}

void CClient::ConnectivityAgent_19()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_19 --- ");
   LOG4CPLUS_INFO(logger, "--- Send data to channel with id = MAX VALUE ---");

   UInt32 channel_id = 0xFFFF;
   UInt32 sending = 1;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      ret = ::sendData(channel_id, mDataBufSend, sending);

      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_19 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_19 - Failed " + convertIntegerToString((int)ret) );
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_19 failed! problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_20()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_20 --- ");
   LOG4CPLUS_INFO(logger, "--- Send data to channel with id = MAX VALUE + 1 ---");

   UInt32 channel_id = 0xFFFF + 1;
   UInt32 sending = 1;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      ret = ::sendData(channel_id, mDataBufSend, sending);

      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_20 - Failed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_20 - Failed" + convertIntegerToString((int)ret) );
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_20 Passed");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_21()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_21 --- ");
   LOG4CPLUS_INFO(logger, "--- Send data to not allocated channel ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;
   ::closeChannel(channel_id);
   UInt32 sending = 1;

   ERROR_CODE ret = ::sendData(channel_id, mDataBufSend, sending);

   if (ret == ERR_OK)
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_21 - Failed");
   }
   else
   {
      LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_21 - Passed");
   }
}

void CClient::ConnectivityAgent_22()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_22 --- ");
   LOG4CPLUS_INFO(logger, "--- Send data. Data is NULL-pointer  ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;
   UInt32 sending = 1;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      ret = ::sendData(channel_id, NULL, sending);

      if (ret == ERR_FAIL)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_22 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_22 - Failed");
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_22 - Failed. Couldn't allocate channel");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_23()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_23 --- ");
   LOG4CPLUS_INFO(logger, "--- Send data. Size isn't greater than 0  ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;
   UInt32 sending = 0;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      ret = ::sendData(channel_id, mDataBufSend, sending);

      if (ret == ERR_FAIL)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_23 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_23 - Failed");
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_23 - Failed. Couldn't allocate channel");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_24()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_24 --- ");
   LOG4CPLUS_INFO(logger, "--- Send data. Size = MAX VALUE ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;
   UInt32 sending = 0xFFFF - 1;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      ret = ::sendData(channel_id, mDataBufSend, sending);

      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_24 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_24 - Failed");
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_24 - Failed. Couldn't allocate channel");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_25()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_25 --- ");
   LOG4CPLUS_INFO(logger, "--- Send data. Size = MAX VALUE + 1 ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;
   UInt32 sending = 0xFFFF;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      ret = ::sendData(channel_id, mDataBufSend, sending);

      if (ret == ERR_FAIL)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_25 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_25 - Failed");
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_25 - Failed. Couldn't allocate channel");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_26()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_26 --- ");
   LOG4CPLUS_INFO(logger, "--- Simple send data. Size is normal value ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;
   UInt32 sending = rand() % 0xFFFE + 1;

   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if (ret == ERR_OK)
   {
      ret = ::sendData(channel_id, mDataBufSend, sending);

      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_26 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_26 - Failed " + convertIntegerToString((int)ret) );
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_26 failed! problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_27()
{
   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_27 --- ");
   LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_27 - Failed. Test isn't yet implemented");
}

void CClient::ConnectivityAgent_28()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_28 --- ");
   LOG4CPLUS_INFO(logger, "--- Simple receive data ---");

   UInt32 channel_id = 10000;
   UInt32 receive = 10;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_28 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_28 - Passed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_28 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_28 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_29()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_29 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive from channel with id = 0 ---");

   UInt32 channel_id = 0;
   UInt32 receive = 10;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_29 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_29 - Failed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_29 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_29 - Passed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_30()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_30 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive data from channel with id = MAX VALUE ---");

   UInt32 channel_id = 0xFFFF;
   UInt32 receive = 10;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_30 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_30 - Passed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_30 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_30 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_31()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_31 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive from channel with id = MAX VALUE + 1 ---");

   UInt32 channel_id = 0xFFFF + 1;
   UInt32 receive = 10;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_31 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_31 - Failed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_31 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_31 - Passed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}
void CClient::ConnectivityAgent_32()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_32 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive data from channel with id = MAX VALUE and receive size = MIN VALUE ---");

   UInt32 channel_id = 0xFFFF;
   UInt32 receive = 1;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_32 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_32 - Passed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_32 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_32 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_33()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_33 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive data from correctly-allocated channel ---");

   UInt32 channel_id = 10000;
   UInt32 receive = 10;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_33 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_33 - Passed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_33 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_33 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_34()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_34 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive data from non-allocated channel ---");

   UInt32 channel_id = 25;
   UInt32 received = 1;
   ERROR_CODE ret = ::closeChannel(channel_id);
   if ((ret == ERR_NOTFOUND)||(ret == ERR_OK))
   {
      ret = ::receiveData(channel_id, mDataBufReceive, received, ::getMaxBufSize());
      if (ret == ERR_NOTFOUND)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_34 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_34 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_34 - Failed - problems with channel closing");
   }
}

void CClient::ConnectivityAgent_35()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_35 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive data. Channel id = NORMAL VALUE ---");

   UInt32 channel_id = 10000;
   UInt32 receive = 10;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_35 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_35 - Passed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_35 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_35 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_36()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_36 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive from channel with id = NULL ---");

   UInt32 channel_id = NULL;
   UInt32 receive = 10;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_36 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_36 - Failed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_36 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_36 - Passed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}


void CClient::ConnectivityAgent_37()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_37 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive data. Data is a null-pointer ---");

   UInt32 channel_id = 10000;
   UInt32 receive = 10;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_37 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, NULL , receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_37 - Failed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_37 - Passed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_37 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_38()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_38 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive data. Received size is 0 ---");

   UInt32 channel_id = 10000;
   UInt32 receive = 0;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_38 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_38 - Passed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_38 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_38 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_39()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_39 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive data. Received size is NULL ---");

   UInt32 channel_id = 10000;
   UInt32 receive = NULL;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_39 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_39 - Passed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_39 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_39 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_40()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_40 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive data. Max size = 0 ---");

   UInt32 channel_id = 10000;
   UInt32 receive = 10;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_40 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, 0);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_40 - Passed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_40 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_40 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_41()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_41 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive data. Max size = large is number ---");

   UInt32 channel_id = 10000;
   UInt32 receive = 10;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();

      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_41 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_41 - Passed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_41 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_41 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}


void CClient::ConnectivityAgent_42()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_42 --- ");
   LOG4CPLUS_INFO(logger, "--- Receive data. Received_size = MAX SIZE ---");

   UInt32 channel_id = 1;
   UInt32 receive = ::getMaxBufSize();
   ERROR_CODE ret = ::allocateChannel(ePlainData,channel_id,this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      mWaitCond.lock();
      while (data_avail[channel_id] == 0)
      {
        LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_42 - wait");
        mWaitCond.wait();
      }

      ret = ::receiveData(channel_id, mDataBufReceive, receive, ::getMaxBufSize());
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_42 - Passed => received " + convertIntegerToString(receive));
         data_avail[channel_id]-=receive;
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_42 - Failed " + convertIntegerToString((int)ret));
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_42 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_43()
{
   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_43 --- ");
   LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_43 - Failed. Test isn't yet implemented");
}

void CClient::ConnectivityAgent_44()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_44 --- ");
   LOG4CPLUS_INFO(logger, "--- Get free size. All params are OK ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;
   UInt32 free_size;

   ERROR_CODE ret = ::allocateChannel(ePlainData,channel_id,this);

   if (ret == ERR_OK)
   {
      ret = ::getFreeSize(channel_id, free_size);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_44 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_44 - Failed " + convertIntegerToString((int)ret) );
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_44 - Failed! problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_45()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_45 --- ");
   LOG4CPLUS_INFO(logger, "--- Get free size. Channel id = 0 ---");

   UInt32 channel_id = 0;
   UInt32 free_size;

   ERROR_CODE ret = ::getFreeSize(channel_id, free_size);
   if (ret == ERR_OK)
   {
      LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_45 - Failed");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_45 - Passed");
   }
}

void CClient::ConnectivityAgent_46()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_46 --- ");
   LOG4CPLUS_INFO(logger, "--- Get free size. Channel id = MAX VALUE ---");

   UInt32 channel_id = 0xFFFF;
   UInt32 free_size;

   ERROR_CODE ret = ::allocateChannel(ePlainData,channel_id,this);

   if (ret == ERR_OK)
   {
      ret = ::getFreeSize(channel_id, free_size);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_46 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_46 - Failed " + convertIntegerToString((int)ret) );
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_46 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_47()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_47 --- ");
   LOG4CPLUS_INFO(logger, "--- Get free size. Channel id = MAX VALUE + 1 ---");

   UInt32 channel_id = 0xFFFF + 1;
   UInt32 free_size;

   ERROR_CODE ret = ::allocateChannel(ePlainData,channel_id,this);

   if (ret == ERR_OK)
   {
      ret = ::getFreeSize(channel_id, free_size);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_47 - Failed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_47 - Failed " + convertIntegerToString((int)ret) );
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_47 - Passed");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_48()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_48 --- ");
   LOG4CPLUS_INFO(logger, "--- Get free size. Free size = 0 ---");

   UInt32 channel_id = rand() % 0xFFFF + 1;
   UInt32 free_size = 0;

   ERROR_CODE ret = ::allocateChannel(ePlainData,channel_id,this);

   if (ret == ERR_OK)
   {
      ret = ::getFreeSize(channel_id, free_size);

      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_48 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_48 - Failed " + convertIntegerToString((int)ret) );
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_48 - Failed - problems with channel allocating");
   }

   ::closeChannel(channel_id);
}

void CClient::ConnectivityAgent_49()
{
   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_49 --- ");
   LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_49 - Failed. Test isn't yet implemented");
}

void CClient::ConnectivityAgent_50()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_50 --- ");
   LOG4CPLUS_INFO(logger, "--- getMaxBufSize ---");


   if(MAX_SIZE != getMaxBufSize())
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_50 - Failed ");
   }
   else
   {
      LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_50 - Passed");
   }
}

void CClient::ConnectivityAgent_51()
{
   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_51 --- ");
   LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_51 - Failed. Test isn't yet implemented");
}

void CClient::ConnectivityAgent_52()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_52 --- ");
   LOG4CPLUS_INFO(logger, "--- Get connection address. All params are OK ---");

   ConnectionAddrStatic ca;
   ERROR_CODE ret = ::getConnectionAddr(&ca, sizeof(ca));

   if (ret == ERR_OK)
   {
      LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_52 - Passed");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_52 - Failed " + convertIntegerToString((int)ret));
   }
}

void CClient::ConnectivityAgent_53()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_53 --- ");
   LOG4CPLUS_INFO(logger, "--- Get connection address. Type is null-pointer ---");

   LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_53 - Obsolete");
}

void CClient::ConnectivityAgent_54()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_54 --- ");
   LOG4CPLUS_INFO(logger, "--- Get connection address. Local address is null-pointer ---");

   LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_54 - Obsolete");
}

void CClient::ConnectivityAgent_55()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(logger, "--- ConnectivityAgent_55 --- ");
   LOG4CPLUS_INFO(logger, "--- Get connection address. Remote address is null-pointer ---");

   LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_55 - Obsolete");
}

void CClient::threadFunc()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   sync_test("INITIAL");

   ConnectivityAgent_28(); sync_test("ConnectivityAgent_28");
   ConnectivityAgent_29(); sync_test("ConnectivityAgent_29");
   ConnectivityAgent_30(); sync_test("ConnectivityAgent_30");
   ConnectivityAgent_31(); sync_test("ConnectivityAgent_31");
   ConnectivityAgent_32(); sync_test("ConnectivityAgent_32");
   ConnectivityAgent_33(); sync_test("ConnectivityAgent_33");
   ConnectivityAgent_35(); sync_test("ConnectivityAgent_35");
   ConnectivityAgent_36(); sync_test("ConnectivityAgent_36");
   ConnectivityAgent_37(); sync_test("ConnectivityAgent_37");
   ConnectivityAgent_38(); sync_test("ConnectivityAgent_38");
   ConnectivityAgent_39(); sync_test("ConnectivityAgent_39");
   ConnectivityAgent_40(); sync_test("ConnectivityAgent_40");
   ConnectivityAgent_41(); sync_test("ConnectivityAgent_41");
   ConnectivityAgent_42(); sync_test("ConnectivityAgent_42");

   ConnectivityAgent_01();
   ConnectivityAgent_02();
   ConnectivityAgent_03();
   ConnectivityAgent_04();
   ConnectivityAgent_05();
   ConnectivityAgent_06();
   ConnectivityAgent_07();
   ConnectivityAgent_08();
   ConnectivityAgent_09();
   ConnectivityAgent_10();
   ConnectivityAgent_11();
   ConnectivityAgent_12();
   ConnectivityAgent_13();
   ConnectivityAgent_14();
   ConnectivityAgent_15();
   ConnectivityAgent_16();
   ConnectivityAgent_17();
   ConnectivityAgent_18();
   ConnectivityAgent_19();
   ConnectivityAgent_20();
   ConnectivityAgent_21();
   ConnectivityAgent_22();
   ConnectivityAgent_23();
   ConnectivityAgent_24();
   ConnectivityAgent_25();
   ConnectivityAgent_26();
   ConnectivityAgent_27();
   ConnectivityAgent_34();
   ConnectivityAgent_44();
   ConnectivityAgent_45();
   ConnectivityAgent_46();
   ConnectivityAgent_47();
   ConnectivityAgent_48();
   ConnectivityAgent_49();
   ConnectivityAgent_50();
   ConnectivityAgent_51();
   ConnectivityAgent_52();
   ConnectivityAgent_53();
   ConnectivityAgent_54();
   ConnectivityAgent_55();

   sync_test("ALL TESTS DONE");
}

void CClient::sync_test(const char* msg/* = ""*/)
{
   LOG4CPLUS_INFO(logger, "CClient BEFORE sync_test: " + std::string(msg));

   mSrvSyncSem.signal();
   mCliSyncSem.wait();

   LOG4CPLUS_INFO(logger, "CClient AFTER  sync_test: " + std::string(msg));
}

