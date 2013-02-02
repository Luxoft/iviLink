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
#include <cassert>
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
 #include "CClient.hpp"
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "ChannelSupervisorTube.hpp"
#include "CError.hpp"
#include "CSError.hpp"
#include "Types.hpp"

using iviLink::ChannelSupervisor::CSError;
Logger CClient::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ChannelSupervisor.test.client.CClient"));

static UInt32 getMaxBufSize()
{
	return MAX_SIZE;
}
CClient::CClient() : 
   CThread("TestClientThread"),
   mSrvSyncSem("/channel_supervisor_test_server"),
   mCliSyncSem("/channel_supervisor_test_client")
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


void CClient::dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size)
{
   LOG4CPLUS_ERROR(msLogger,"CClient::dataReceivedCallback(): available %d bytes on %d channel" + convertIntegerToString((int)read_size) + convertIntegerToString((int)channel_id));
   UInt32 received = 0;
   mWaitCond.lock();
   data_avail[channel_id]+=read_size;
   mWaitCond.broadcast();
   mWaitCond.unlock();
   ////::receiveData(channel_id, mDataBufReceive, received, ::getMaxBufSize());
   ////LOG4CPLUS_ERROR(msLogger,"CClient::dataReceivedCallback()=>read %d bytes", received);
   //_hex_print(mDataBuf,received );
    ///mWaitCond.signal();
}

void CClient::bufferOverflowCallback(const UInt32 channel_id)
{
   LOG4CPLUS_ERROR(msLogger,"CClient::bufferOverflowCallback()=>try to read from %d channel buffer..." +convertIntegerToString((int)channel_id));
   UInt32 received = 0;
   iviLink::ChannelSupervisor::receiveData(channel_id, "SeatControlAPI",mDataBufReceive, received,::getMaxBufSize());
  // ::receiveData(channel_id, mDataBufReceive, received, ::getMaxBufSize());
   LOG4CPLUS_ERROR(msLogger,"CClient::bufferOverflowCallback()=>read %d bytes"+ convertIntegerToString((int)received));	
}

void CClient::channelDeletedCallback(const UInt32 channel_id)
{
   LOG4CPLUS_ERROR(msLogger,"CClient::channelDeletedCallback() - %d channel deleted!" + convertIntegerToString((int)channel_id));

}

void CClient::connectionLostCallback()
{
   LOG4CPLUS_ERROR(msLogger,"CClient::connectionLostCallback()");
}
void CClient::threadFunc()
{
   // ChannelSupervisor_03();
   // ChannelSupervisor_05();
   // ChannelSupervisor_12();
   // ChannelSupervisor_16();
   // ChannelSupervisor_31();
   // ChannelSupervisor_32();
   // ChannelSupervisor_34();
   // ChannelSupervisor_36();

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

   ChannelSupervisor_03();
   ChannelSupervisor_05();
   ChannelSupervisor_12();
   ChannelSupervisor_16();
   ChannelSupervisor_31();
   ChannelSupervisor_32();
   ChannelSupervisor_34();
  // ChannelSupervisor_36();

}

void CClient::ChannelSupervisor_03()//Wrong reserved channel value “0” trying to use
{
   ////TRACE();
   UInt32 channel_id = 0;
   UInt32 sending = 1;
   BaseError ret = iviLink::ChannelSupervisor::sendData(channel_id,"SeatControlAPI_03", mDataBufSend, sending);

   if (ret.isNoError())
   {
      //LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_03: failed %d"+ (std::string)ret);
      LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_03: failed " + (std::string)ret);
   }
   else
   {

     // LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_03 - Passed %d"+ (std::string)ret);
      LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_03: Passed ");
   }
}

void CClient::ChannelSupervisor_04()//Trying to send by channel already in use
{
  // //TRACE();
   UInt32 mChannelID;
   UInt32 sending = 1;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_04", mChannelID);
   if (ret.isNoError())
   {
      ret = iviLink::ChannelSupervisor::sendData(mChannelID, "SeatControlAPI_04", mDataBufSend, sending);
      if (ret.isNoError())   
      {
         //LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_04 - Passed");
         LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_04 - Passed");

      }
      else
      { 
        // LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_04: failed %d"+ (std::string)ret);
       LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_04: failed %d" + (std::string)ret);
      }
   }
   else
   {
      //LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_04 failed! problems with channel allocating. Error code - %d "+ (std::string)ret);
      LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_04 failed! problems with channel allocating. Error code - %d " + (std::string)ret);
   }
  
}

void CClient::ChannelSupervisor_05()//Sending data to not allocated channel 
{
   //TRACE();
   UInt32 mChannelID = UINT_MAX;
   UInt32 sending = 1;
   BaseError ret = iviLink::ChannelSupervisor::deallocateChannel(mChannelID, "ClimateProfileAPI_05");

   if ((ret.getCode() == CSError::DEALLOCATION_CHANNEL_MAP_WRONG_CID)||(ret.isNoError()))
   {
      ret = iviLink::ChannelSupervisor::sendData(mChannelID,"ClimateProfileAPI_05", mDataBufSend, sending);
      if (ret.isNoError())
      {
         //LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_05 - Passed");
         LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_05 - Passed");
      }
      else
      { 
         //LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_05: failed %d"+ (std::string)ret);
         LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_05: failed %d" + (std::string)ret);
     }

   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_05 failed! problems with channel close %d"+ (std::string)ret);
   }
}

void CClient::ChannelSupervisor_07()//Sending data to with Null pointer to data 
{
   //TRACE();

   UInt32 sending = 1;
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ClimateProfileAPI_07", mChannelID);
   if (ret.isNoError())
   {
      ret = iviLink::ChannelSupervisor::sendData(mChannelID,"ClimateProfileAPI_07", NULL, sending);
      if (!ret.isNoError() )
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_07 - PASSED "+ (std::string)ret);
      }
      else
      { 
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_07: FAILED"+ (std::string)ret);
      }

   }
   else
   {
     LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_07 FAILED! problems with channel allocating");
   }

}

void CClient::ChannelSupervisor_08()//Sending data with correct data pointer and minimum data size value
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 sending = 1;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ClimateProfileAPI_08", mChannelID);
   if (ret.isNoError())
   {
     ret = iviLink::ChannelSupervisor::sendData(mChannelID,"ClimateProfileAPI_08", mDataBufSend, sending);
     if (ret.isNoError()) 
      {
        LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_08 - PASSED");
      }
      else
      { 
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_08: FAILED "+ (std::string)ret);
      }
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_08 failed! problems with channel allocating");
   }

}

void CClient::ChannelSupervisor_09()//0-size data to send 
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 sending = 0;
   BaseError ret = iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_09", mChannelID);
   if (ret.isNoError())
   {
      ret = iviLink::ChannelSupervisor::sendData(mChannelID,"SeatControlAPI_09", mDataBufSend, sending);
      if (!ret.isNoError())  
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_09 - PASSED "+ (std::string)ret);
      }
      else
      { 
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_09: FAILED "+ (std::string)ret);
      }
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_09 failed! problems with channel allocating");
   }

}

void CClient::ChannelSupervisor_12()//Check of channel_id “null” value in recieveData()
{
   //TRACE();
   UInt32 received = 1;
  // UInt32 bufSize = ::getMaxBufSize();
   BaseError ret = iviLink::ChannelSupervisor::receiveData(0, "SeatControlAPI",mDataBufReceive, received,::getMaxBufSize());
   if (!ret.isNoError())   
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_12 - PASSED %d"+ (std::string)ret);
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_12: FAILED %d "+ (std::string)ret);
   }
}

void CClient::ChannelSupervisor_15()//Recieve data from correctly allocated channel
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 received = 1;
   //UInt32 bufSize = ::getMaxBufSize();
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_15", mChannelID);
   if (ret.isNoError())
   {
      mWaitCond.lock();
      while (data_avail[mChannelID] == 0)
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_15 - wait");
         mWaitCond.wait();
      }
      
      ret = iviLink::ChannelSupervisor::receiveData(mChannelID, "SeatControlAPI_15",mDataBufReceive, received,::getMaxBufSize());
      if (ret.isNoError())   
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_15 - PASSED =>received %d" + convertIntegerToString((int)received));
       data_avail[mChannelID] -= received;
      }
      else
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_15: FAILED %d"+ (std::string)ret);
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_15 FAILED! problems with channel allocating");
   }

}

void CClient::ChannelSupervisor_16()// Recieve data to not allocated channel UINT_MAX
{
   //TRACE();
   UInt32 mChannelID = UINT_MAX-1;
   UInt32 received = 1;
   UInt32 bufSize = ::getMaxBufSize();
   BaseError ret = iviLink::ChannelSupervisor::deallocateChannel(mChannelID, "SeatControlAPI_16");
  // if (ret.isNoError())
   //{
      ret = iviLink::ChannelSupervisor::receiveData(mChannelID,"SeatControlAPI_16",mDataBufReceive, received, bufSize);
      if (!ret.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_16 - PASSED %d"+ (std::string)ret);
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_16: FAILED ");
      }
   //}
   //else
   //{
   //   LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_16 FAILED! problems with channel closing %d"+ (std::string)ret);
   //}
}

void CClient::ChannelSupervisor_18()//Check for unable to send request with null pointer to data
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 received = 1;
  // UInt32 bufSize = ::getMaxBufSize();
   
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_18", mChannelID);
   ret = iviLink::ChannelSupervisor::receiveData(mChannelID, "SeatControlAPI_18",NULL, received,::getMaxBufSize());
   if (!ret.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_18 - PASSED %d"+ (std::string)ret);
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_18: FAILED ");
   }

}


void CClient::ChannelSupervisor_21()//Check for data buffer size allow take UINT_MAX value for reciveData() received parameter
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 received = UINT_MAX;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_21", mChannelID);
   if (ret.isNoError())
   { 
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_21 - Channel is allocated");

      mWaitCond.lock();
      if(data_avail[mChannelID] == 0)
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_21 - wait");
         mWaitCond.wait();
      }
      //ret = ::receiveData(channel_id, mDataBufReceive,received, 31);
     ret = iviLink::ChannelSupervisor::receiveData(mChannelID, "SeatControlAPI_21",mDataBufReceive, received,::getMaxBufSize());
      if (ret.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_21 - PASSED=>received %d"+ convertIntegerToString((int)received) );
         data_avail[mChannelID]-=received;
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_21: FAILED %d"+ (std::string)ret);
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_21 FAILED! problems with channel allocating");
   }

}

void CClient::ChannelSupervisor_22()//Check for data buffer size isn't “0” value for reciveData()
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 received = 1;
   UInt32 bufferSize = 0;   
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_22", mChannelID);
   if (ret.isNoError())
   {
      ret = iviLink::ChannelSupervisor::receiveData(mChannelID, "SeatControlAPI_22",mDataBufReceive, received,bufferSize);
      //ret = ::receiveData(channel_id, mDataBufReceive, received, bufferSize);
      if (!ret.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_22 - PASSED %d"+ (std::string)ret);
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_22: FAILED %d"+ (std::string)ret);
      }
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_22 FAILED! problems with channel allocating %d"+ (std::string)ret);
   }

}


void CClient::ChannelSupervisor_28() //Correct allocating channel
{
   //TRACE();
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_28", mChannelID);
   if (ret.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_28 - PASSED");
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_28: FAILED %d"+ (std::string)ret);
   }
}


void CClient::ChannelSupervisor_31() //NULL pointer to object
{
   //TRACE();
   UInt32 mChannelID;
    BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(NULL,"SeatControlAPI_31", mChannelID);
   if (!ret.isNoError()) 
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_31 - Passed %d"+ (std::string)ret);
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_31: failed %d"+ (std::string)ret);
   }
}

void CClient::ChannelSupervisor_32() //Closing reserved channel value "0"
{
   //TRACE();
   UInt32 mChannelID = 0;
   BaseError ret = iviLink::ChannelSupervisor::deallocateChannel(mChannelID, "ClimateProfileAPI");
   if(ret.getCode() == CSError::DEALLOCATION_CHANNEL_MAP_WRONG_CID)
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_32 - PASSED");
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_32 - FAILED %d"+ (std::string)ret);
   }
}

void CClient::ChannelSupervisor_33()//Successful channel close
{
   //TRACE();
   UInt32 mChannelID;
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"SeatControlAPI_33", mChannelID);

   sync_test("ChannelSupervisor_33 alloc sync");

   if(ret.isNoError())
   {
      ret = iviLink::ChannelSupervisor::deallocateChannel(mChannelID, "SeatControlAPI_33");
      if (ret.isNoError())   
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_33 - PASSED");
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_33 - FAILED %d"+ (std::string)ret);
      }
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_33 Allocation channel- FAILED %d"+ (std::string)ret);
   }

}

void CClient::ChannelSupervisor_34()//Close not allocated channel 
{
   //TRACE();

   UInt32 mChannelID = UINT_MAX;
   BaseError ret = iviLink::ChannelSupervisor::deallocateChannel(mChannelID, "ClimateProfileAPI_34");
   if ((ret.isNoError())||(ret.getCode() == CSError::DEALLOCATION_CHANNEL_MAP_WRONG_CID) )  
   {
         ret = iviLink::ChannelSupervisor::deallocateChannel(mChannelID, "ClimateProfileAPI_34");
         if(ret.getCode() == CSError::DEALLOCATION_CHANNEL_MAP_WRONG_CID)
         {
            LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_34 - PASSED");
         }
         else
         {
            LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_34 close channel- FAILED %d"+ (std::string)ret);
         }
      }  
      else 
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_34 - Failed on Prepared steps %d"+ (std::string)ret);
      }

}


void CClient::ChannelSupervisor_36()//Wrong 0 parameter value sending 
{
   //TRACE();

   BaseError ret = iviLink::ChannelSupervisor::deallocateChannel(0, "ClimateProfileAPI");
   if (ret.getCode() == CSError::DEALLOCATION_CHANNEL_MAP_WRONG_CID)   
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_36 - PASSED");
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_36: FAILED %d"+ (std::string)ret);
   }
}

void CClient::ChannelSupervisor_40() //tag is empty
{
   //TRACE();
   UInt32 mChannelID = 0;
   BaseError ret = iviLink::ChannelSupervisor::deallocateChannel(mChannelID, "");
   if(ret.getCode() == CSError::DEALLOCATION_CHANNEL_MAP_WRONG_CID)
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_40 - PASSED");
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_40 - FAILED %d"+ (std::string)ret);
   }
}

void CClient::ChannelSupervisor_41() //tag is 0
{
   //TRACE();
   UInt32 mChannelID = 0;
   BaseError ret = iviLink::ChannelSupervisor::deallocateChannel(mChannelID, 0);
   if(ret.getCode() == CSError::DEALLOCATION_CHANNEL_MAP_WRONG_CID)
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_41 - PASSED");
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_41 - FAILED %d"+ (std::string)ret);
   }
}

void CClient::ChannelSupervisor_42() //Correct allocating channel
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 sending = 1;

   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ChannelSupervisor_42", mChannelID);
   if (ret.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_42 - allocation done ");
      ret = iviLink::ChannelSupervisor::sendData(mChannelID,"ChannelSupervisor_42", mDataBufSend, sending);

      if (ret.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_42 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_42: failed %d" + (std::string)ret);
      }
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_42: Channel allocation FAILED " + convertIntegerToString(ret.getCode()));
   }
}

void CClient::ChannelSupervisor_43() //Correct allocating channel
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 sending = 1;

   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ChannelSupervisor_43", mChannelID);
   if (ret.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_43 - allocation done ");
      ret = iviLink::ChannelSupervisor::sendData(mChannelID,"", mDataBufSend, sending);

      if (ret.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_43 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_43: failed %d" + (std::string)ret);
      }
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_43: Channel allocation FAILED " + convertIntegerToString(ret.getCode()));
   }
}

void CClient::ChannelSupervisor_44() //Correct allocating channel
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 sending = 1;

   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ChannelSupervisor_44", mChannelID);
   if (ret.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, "CServer::ChannelSupervisor_44 - allocation done ");
      ret = iviLink::ChannelSupervisor::sendData(mChannelID,"", mDataBufSend, sending);

      if (ret.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, "CServer::ChannelSupervisor_44 - Passed");
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger, "CServer::ChannelSupervisor_44: failed %d" + (std::string)ret);
      }
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger, "CClient::ChannelSupervisor_44: Channel allocation FAILED " + convertIntegerToString(ret.getCode()));
   }
}

void CClient::ChannelSupervisor_45()//Recieve from channel id = 0
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 received = 1;
   //UInt32 bufSize = ::getMaxBufSize();
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ChannelSupervisor_45", mChannelID);
   if (ret.isNoError())
   {
      mWaitCond.lock();
      while (data_avail[mChannelID] == 0)
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_45 - wait");
         mWaitCond.wait();
      }

      ret = iviLink::ChannelSupervisor::receiveData(0, "ChannelSupervisor_45",mDataBufReceive, received,::getMaxBufSize());
      if (ret.isNoError())
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_45 - PASSED =>received %d" + convertIntegerToString((int)received));
       data_avail[mChannelID] -= received;
      }
      else
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_45: FAILED %d"+ (std::string)ret);
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_15 FAILED! problems with channel allocating");
   }

}

void CClient::ChannelSupervisor_46()//Recieve data with empty TAG
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 received = 1;
   //UInt32 bufSize = ::getMaxBufSize();
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ChannelSupervisor_46", mChannelID);
   if (ret.isNoError())
   {
      mWaitCond.lock();
      while (data_avail[mChannelID] == 0)
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_46 - wait");
         mWaitCond.wait();
      }

      ret = iviLink::ChannelSupervisor::receiveData(mChannelID, "",mDataBufReceive, received,::getMaxBufSize());
      if (ret.isNoError())
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_46 - PASSED =>received %d" + convertIntegerToString((int)received));
       data_avail[mChannelID] -= received;
      }
      else
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_46: FAILED %d"+ (std::string)ret);
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_15 FAILED! problems with channel allocating");
   }

}

void CClient::ChannelSupervisor_47()//Recieve data with 0 TAG
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 received = 1;
   //UInt32 bufSize = ::getMaxBufSize();
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ChannelSupervisor_47", mChannelID);
   if (ret.isNoError())
   {
      mWaitCond.lock();
      while (data_avail[mChannelID] == 0)
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_47 - wait");
         mWaitCond.wait();
      }

      ret = iviLink::ChannelSupervisor::receiveData(mChannelID, 0,mDataBufReceive, received,::getMaxBufSize());
      if (ret.isNoError())
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_47 - PASSED =>received %d" + convertIntegerToString((int)received));
       data_avail[mChannelID] -= received;
      }
      else
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_47: FAILED %d"+ (std::string)ret);
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_47 FAILED! problems with channel allocating");
   }
}

void CClient::ChannelSupervisor_48()//Recieve with received size = 0
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 received = 0;
   //UInt32 bufSize = ::getMaxBufSize();
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ChannelSupervisor_48", mChannelID);
   if (ret.isNoError())
   {
      mWaitCond.lock();
      while (data_avail[mChannelID] == 0)
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_48 - wait");
         mWaitCond.wait();
      }

      ret = iviLink::ChannelSupervisor::receiveData(mChannelID, "ChannelSupervisor_48",mDataBufReceive, received,::getMaxBufSize());
      if (ret.isNoError())
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_48 - PASSED =>received %d" + convertIntegerToString((int)received));
       data_avail[mChannelID] -= received;
      }
      else
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_48: FAILED %d"+ (std::string)ret);
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_48 FAILED! problems with channel allocating");
   }
}


void CClient::ChannelSupervisor_49()//Recieve with max size = 0
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 received = 1;
   //UInt32 bufSize = ::getMaxBufSize();
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ChannelSupervisor_49", mChannelID);
   if (ret.isNoError())
   {
      mWaitCond.lock();
      while (data_avail[mChannelID] == 0)
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_49 - wait");
         mWaitCond.wait();
      }

      ret = iviLink::ChannelSupervisor::receiveData(0, "ChannelSupervisor_49",mDataBufReceive, received,0);
      if (ret.isNoError())
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_49 - PASSED =>received %d" + convertIntegerToString((int)received));
       data_avail[mChannelID] -= received;
      }
      else
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_49: FAILED %d"+ (std::string)ret);
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_49 FAILED! problems with channel allocating");
   }

}

void CClient::ChannelSupervisor_50()//Recieve with received size > max size
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 received = UINT_MAX + 1;
   //UInt32 bufSize = ::getMaxBufSize();
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ChannelSupervisor_50", mChannelID);
   if (ret.isNoError())
   {
      mWaitCond.lock();
      while (data_avail[mChannelID] == 0)
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_50 - wait");
         mWaitCond.wait();
      }

      ret = iviLink::ChannelSupervisor::receiveData(0, "ChannelSupervisor_50",mDataBufReceive, received, ::getMaxBufSize());
      if (ret.isNoError())
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_50 - PASSED =>received %d" + convertIntegerToString((int)received));
       data_avail[mChannelID] -= received;
      }
      else
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_50: FAILED %d"+ (std::string)ret);
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_15 FAILED! problems with channel allocating");
   }

}

void CClient::ChannelSupervisor_51()//Recieve from channel id = 0
{
   //TRACE();
   UInt32 mChannelID;
   UInt32 received = UINT_MAX;
   //UInt32 bufSize = ::getMaxBufSize();
   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this,"ChannelSupervisor_51", mChannelID);
   if (ret.isNoError())
   {
      mWaitCond.lock();
      while (data_avail[mChannelID] == 0)
      {
         LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_51 - wait");
         mWaitCond.wait();
      }

      ret = iviLink::ChannelSupervisor::receiveData(0, "ChannelSupervisor_51",mDataBufReceive, received, ::getMaxBufSize());
      if (ret.isNoError())
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_51 - PASSED =>received %d" + convertIntegerToString((int)received));
       data_avail[mChannelID] -= received;
      }
      else
      {
       LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_51: FAILED %d"+ (std::string)ret);
      }
      mWaitCond.unlock();
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger,"CClient::ChannelSupervisor_51 FAILED! problems with channel allocating");
   }
}


void CClient::sync_test(const char* msg/* = ""*/)
{
   LOG4CPLUS_ERROR(msLogger,"CClient BEFORE sync_test: %s"  +  std::string(msg));

   mSrvSyncSem.signal();
   mCliSyncSem.wait();

   LOG4CPLUS_ERROR(msLogger,"CClient AFTER  sync_test: %s"  +  std::string(msg));
}

void CClient::both_sides_close_test()
{
   //TRACE();

   const char tag[] = "both_sides_close_test";
   UInt32 chid = 0;

   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this, tag, chid);
   sync_test("both_sides_close_test alloc sync");

   LOG4CPLUS_ERROR(msLogger,"both_sides_close_test alloc ret = %s" + (std::string)ret);

   ret = iviLink::ChannelSupervisor::deallocateChannel(chid, tag);
   LOG4CPLUS_ERROR(msLogger,"both_sides_close_test deallocateChannel err = %s" + (std::string)ret);

   // mDeleteCond.lock();
   // while (mDeletedChannels.empty())
   //    mDeleteCond.wait();
   // assert(mDeletedChannels.size() == 1 && *mDeletedChannels.begin() == chid);
   // mDeleteCond.unlock();


   sync_test("both_sides_close_test dealloc sync");

}

void CClient::one_side_close_test()
{
   //TRACE();

   mDeleteCond.lock();
   mDeletedChannels.clear();
   mDeleteCond.unlock();

   const char tag[] = "one_side_close_test";
   UInt32 chid = 0;

   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this, tag, chid);
   sync_test("one_side_close_test alloc sync");

   LOG4CPLUS_ERROR(msLogger,"one_side_close_test alloc ret = %s" + (std::string)ret);
   if (!ret.isNoError())
      return;

   ret = iviLink::ChannelSupervisor::deallocateChannel(chid, tag);
   LOG4CPLUS_ERROR(msLogger,"one_side_close_test deallocateChannel err = %s" + (std::string)ret);

   sync_test("one_side_close_test dealloc sync");

   mDeleteCond.lock();
   assert(mDeletedChannels.empty());
   mDeleteCond.unlock();
}


void CClient::one_side_close_same_thread_test()
{
   // test duplicates one_side_close_test() on this side
   //TRACE();

   mDeleteCond.lock();
   mDeletedChannels.clear();
   mDeleteCond.unlock();

   const char tag[] = "one_side_close_same_thread_test";
   UInt32 chid = 0;

   BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this, tag, chid);
   sync_test("one_side_close_same_thread_test alloc sync");

   LOG4CPLUS_ERROR(msLogger,"one_side_close_same_thread_test alloc ret = %s" + (std::string)ret);
   if (!ret.isNoError())
      return;

   ret = iviLink::ChannelSupervisor::deallocateChannel(chid, tag);
   LOG4CPLUS_ERROR(msLogger,"one_side_close_same_thread_test deallocateChannel err = %s" + (std::string)ret);

   sync_test("one_side_close_same_thread_test dealloc sync");

   mDeleteCond.lock();
   assert(mDeletedChannels.empty());
   mDeleteCond.unlock();
}

void CClient::multiple_open_close_same_tag_test()
{
   //TRACE();

   const char tag[] = "multiple_open_close_same_tag_test";
   

   for (int i = 0 ; i < 10; ++i)
   {
      UInt32 chid = 0;

      BaseError ret =  iviLink::ChannelSupervisor::allocateChannel(this, tag, chid);
      sync_test("multiple_open_close_same_tag_test alloc sync");

      LOG4CPLUS_ERROR(msLogger,"multiple_open_close_same_tag_test alloc ret = %s"+ (std::string)ret);

      ret = iviLink::ChannelSupervisor::deallocateChannel(chid, tag);
      LOG4CPLUS_ERROR(msLogger,"multiple_open_close_same_tag_test deallocateChannel err = %s" + (std::string)ret);

      sync_test("multiple_open_close_same_tag_test dealloc sync");
   }
}
