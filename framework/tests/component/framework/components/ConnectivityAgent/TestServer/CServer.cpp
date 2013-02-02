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

/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "tests/component/framework/components/ConnectivityAgent/TestServer/CServer.hpp"
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "framework/components/ConnectivityAgent/generic/common/API.hpp"
#include "utils/misc/Logger.hpp"

Logger CServer::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.test.server.CServer"));

CServer::CServer() : 
   CThread("TestServerThread"),
   mSrvSyncSem("/connectivity_agent_test_server"),
   mCliSyncSem("/connectivity_agent_test_client")
{
   mDataBuf = new UInt8[::getMaxBufSize()];
}
CServer::~CServer()
{
   if(mDataBuf) delete [] mDataBuf;
}

void CServer::dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size/*, const UInt32 free_size*/)
{
   LOG4CPLUS_INFO(logger, "CServer::dataReceivedCallback(): available "
         + convertIntegerToString(read_size) + " bytes on "
         + convertIntegerToString(channel_id) + " channel | free space ");
//         + convertIntegerToString(free_size));
   UInt32 received = 0;
   ::receiveData(channel_id, mDataBuf, received, ::getMaxBufSize());
   LOG4CPLUS_INFO(logger, "CServer::dataReceivedCallback()=>read "
      + convertIntegerToString(received) + " bytes");
}

void CServer::bufferOverflowCallback(const UInt32 channel_id)
{
   LOG4CPLUS_INFO(logger, "CServer::bufferOverflowCallback()=>try to read from "
         + convertIntegerToString(channel_id) + " channel buffer...");
   UInt32 received = 0;
   ::receiveData(channel_id, mDataBuf, received, ::getMaxBufSize());
   LOG4CPLUS_INFO(logger, "CServer::bufferOverflowCallback()=>read "
         + convertIntegerToString(received) + " bytes");
}

void CServer::channelDeletedCallback(const UInt32 channel_id)
{
   LOG4CPLUS_INFO(logger, "CServer::channelDeletedCallback() - "
         + convertIntegerToString(channel_id) + " channel deleted!");
   mDeleteSem.signal();
}

void CServer::connectionLostCallback()
{
   LOG4CPLUS_TRACE(logger, "CServer::connectionLostCallback()");
}

/*void CServer::ConnectivityAgent_15()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 1;
   ERROR_CODE ret = ::allocateChannel(eSafetyCritical,channel_id,this);
   if(ret == ERR_OK)
   {
      char* str = "ConnectivityAgent_15 test data!\0";
      ret =  sendData(channel_id, reinterpret_cast<UInt8*>(str), strlen(str));
      LOG4CPLUS_ERROR(logger, "SendData TC_15 : error " + convertIntegerToString((int)ret));
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "AllocateChannel (1) failed: error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_17()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = UINT_MAX-1;
   ERROR_CODE ret = ::allocateChannel(ePlainData,channel_id,this);
   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)   
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_17 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_17 - sendData error " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_17 - allocation error " + convertIntegerToString((int)ret));
   }
}
void CServer::ConnectivityAgent_21()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 1;
   ERROR_CODE ret = ::allocateChannel(eSafetyCritical,channel_id,this);
   if((ret == ERR_OK)||(ret == ERR_NUMBER_BUSY))
   {
      char* str = "ConnectivityAgent_21 test data!\0";

     ret =  sendData(channel_id, reinterpret_cast<UInt8*>(str), strlen(str));
     LOG4CPLUS_ERROR(logger, "sendData TC_21: error " + convertIntegerToString((int)ret));

   }
   else
   {
      LOG4CPLUS_ERROR(logger, "allocateChannel  (1) failed: error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_24()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 25;
   ERROR_CODE ret = ::allocateChannel(eSafetyCritical,channel_id,this);
   if((ret == ERR_OK)||(ret == ERR_NUMBER_BUSY))
   {
      LOG4CPLUS_INFO(logger, "Channel 25 is allocated for ConnectivityAgent_24 " + convertIntegerToString((int)ret));
  
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "AllocateChannel (25) failed for TC_24: error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_25()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 1;

   ERROR_CODE ret = ::allocateChannel(eRealTime,channel_id,this);
   if((ret == ERR_NUMBER_BUSY))
   {
      LOG4CPLUS_INFO(logger, "ConnectivityAgent_25 wait for channel closing...");
      mDeleteSem.wait();
      LOG4CPLUS_INFO(logger, "ConnectivityAgent_25 channel is closed: error " + convertIntegerToString((int)ret));
      LOG4CPLUS_INFO(logger, "ConnectivityAgent_25 allocate : error " + convertIntegerToString((int)ret));
      ret = ::allocateChannel(eRealTime,channel_id,this);
      if(ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_25 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "CClient::ConnectivityAgent_25 - Failed => error " + convertIntegerToString((int)ret));
      }       
   }
   else
   {
      if(ret == ERR_OK) 
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_25 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_25 allocateChannel (): error " + convertIntegerToString((int)ret));
      }
   }
}

void CServer::ConnectivityAgent_27()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   UInt32 channel_id = UINT_MAX;
   ERROR_CODE ret = ::allocateChannel(ePlainData,channel_id,this);
   if(ret == ERR_OK) 
   {
      LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_27 - Failed");
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_27 - Passed " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_28()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   // precondition
   UInt32 channel_id = 1;
   ERROR_CODE ret = ::closeChannel(channel_id);

   sync_test("ConnectivityAgent_28 precondition");

   // action
   ret = ::allocateChannel(eSafetyCritical,channel_id,this);
   if(ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "CClient::ConnectivityAgent_28 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_25 allocateChannel (): error " + convertIntegerToString((int)ret));
      }
}

void CServer::ConnectivityAgent_50()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = UINT_MAX;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);
   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_50 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_50 - sendData error " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_50 - allocation error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_51()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = UINT_MAX - 1;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);
   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_51 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_51 - sendData error " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_51 - allocation error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_52()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 0;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);
   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_52 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_52 - sendData error " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_52 - allocation error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_53()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = rand() % UINT_MAX;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);
   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_53 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_53 - sendData error " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_53 - allocation error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_54()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = rand() % UINT_MAX;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);
   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_54 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_54 - sendData error " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_54 - allocation error " + convertIntegerToString((int)ret));
   }
}*/

void CServer::ConnectivityAgent_28()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 10000;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_28 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_28 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_28 - Failed - Problems with channel allocating error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_29()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 0;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_29 - Failed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_29 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_29 - Passed");
   }
}

void CServer::ConnectivityAgent_30()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 0xFFFF;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_30 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_30 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_30 - Failed - Problems with channel allocating error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_31()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 0xFFFF + 1;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_31 - Failed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_31 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_31 - Passed");
   }
}

void CServer::ConnectivityAgent_32()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 0xFFFF;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_32 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_32 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_32 - Failed - Problems with channel allocating error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_33()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 10000;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_33 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_33 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_33 - Failed - Problems with channel allocating error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_35()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 10000;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_35 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_35 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_35 - Failed - Problems with channel allocating error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_36()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 0;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_36 - Failed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_36 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_36 - Passed");
   }
}

void CServer::ConnectivityAgent_37()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 10000;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_37 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_37 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_37 - Failed - Problems with channel allocating error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_38()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 10000;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_38 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_38 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_38 - Failed - Problems with channel allocating error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_39()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 10000;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_39 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_39 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_39 - Failed - Problems with channel allocating error " + convertIntegerToString((int)ret));
   }
}
void CServer::ConnectivityAgent_40()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 10000;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_40 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_40 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_40 - Failed - Problems with channel allocating error " + convertIntegerToString((int)ret));
   }
}

void CServer::ConnectivityAgent_41()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 10000;
   ERROR_CODE ret = ::allocateChannel(ePlainData, channel_id, this);

   if ((ret == ERR_NUMBER_BUSY)||(ret == ERR_OK))
   {
      ret =  sendData(channel_id, mDataBuf, 1);
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_41 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_41 - Failed " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_41 - Failed - Problems with channel allocating error " + convertIntegerToString((int)ret));
   }
}


void CServer::ConnectivityAgent_42()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt32 channel_id = 1;
   ERROR_CODE ret = ::allocateChannel(eSafetyCritical,channel_id,this);

   if((ret == ERR_OK)||(ret == ERR_NUMBER_BUSY))
   {
      char* str = "ConnectivityAgent_42 Data!\0";

      ret =  sendData(channel_id, reinterpret_cast<UInt8*>(str), strlen(str));
      if (ret == ERR_OK)
      {
         LOG4CPLUS_INFO(logger, "ConnectivityAgent_42 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(logger, "ConnectivityAgent_42 - Failed - sendData error " + convertIntegerToString((int)ret));
      }
   }
   else
   {
      LOG4CPLUS_ERROR(logger, "ConnectivityAgent_42 - Failed - Problems with channel allocating error " + convertIntegerToString((int)ret));
   }
}


void CServer::threadFunc()
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
   
   sync_test("ALL TESTS DONE");
}

void CServer::sync_test(const char* msg/* = ""*/)
{
   LOG4CPLUS_INFO(logger, "CServer BEFORE sync_test: " + std::string(msg));

   mCliSyncSem.signal();
   mSrvSyncSem.wait();

   LOG4CPLUS_INFO(logger, "CServer AFTER  sync_test: " + std::string(msg));
}
