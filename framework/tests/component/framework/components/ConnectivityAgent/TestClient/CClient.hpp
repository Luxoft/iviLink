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


#ifndef CONNECTIVITY_AGENT_CLIENT_HPP
#define CONNECTIVITY_AGENT_CLIENT_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <map>
 /********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/threads/CThread.hpp"
#include "utils/threads/CCondVar.hpp"
#include "utils/threads/CSignalSemaphoreInterproc.hpp"
#include "framework/components/ConnectivityAgent/generic/common/IChannelObserver.hpp"


///CClient class
/**
 * The CClient incapsulates testing features of Connectivity Agent. It uses Agent's API for basic functionality and also 
 * implements IChannelObserver interface to receive callbacks from Agent.It implements CThread as well to perform testing 
 * activity in separate thread
 */

class CClient: public IChannelObserver, public CThread
{
private:
   static Logger logger;

public:

   /**
    * Constructor
    */
   CClient();

   /**
    * Destructor
    */
   ~CClient();

   // from IChannelObserver

   /**
    * Callback for data receiving. Informs client about readable data and free space in buffer.
    * Client should implement this callback
    * @param channel_id channel number which has received data
    * @param read_size data size available for read
    * @param free_size free size left
    */
   virtual void dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size);//, const UInt32 free_size);

   /**
    * Callback for channel buffer overflow.
    * Client should implement this callback
    * @param channel_id channel number which data has been lost
    */
   virtual void bufferOverflowCallback(const UInt32 channel_id);

   /**
    * Callback for channel deletion initiated by other side application
    * Client should implement this callback
    * @param channel_id channel number which has been deallocated
    */
   virtual void channelDeletedCallback(const UInt32 channel_id);

   /**
    * Callback for lost connection with the other side Connectivity Agent
    * Client should implement this callback
    */
   virtual void connectionLostCallback();

   // from CThread
   /**
    * Thread routine
    */

   virtual void threadFunc();

   //Test Cases 

   /*
    * Simple channel allocation
    */
   void ConnectivityAgent_01();

   /*
    * Channel allocation with eSafetyCritical priority (0)
    */
   void ConnectivityAgent_02();

   /*
    * Channel allocation with eRealTime priority (1)
    */
   void ConnectivityAgent_03();

   /*
    * Allocation of channel with id = 0
    */
   void ConnectivityAgent_04();

   /*
    * Allocation of channel with id = MAX VALUE
    */
   void ConnectivityAgent_05();

   /*
    * Allocation of channel with id = MAX VALUE + 1
    */
   void ConnectivityAgent_06();

   /*
    * Allocation of channel which is already in use
    */
   void ConnectivityAgent_07();

   /*
    * Channel allocation with NULL-pointer observer
    */
   void ConnectivityAgent_08();

   /*
    * Test isn't yet implemented
    */
   void ConnectivityAgent_09();

   /*
    * Test isn't yet implemented
    */
   void ConnectivityAgent_10();

   /*
    * Test isn't yet implemented
    */
   void ConnectivityAgent_11();

   /*
    * Test isn't yet implemented
    */
   void ConnectivityAgent_12();

   /*
    * Close channel which id = 0
    */
   void ConnectivityAgent_13();

   /*
    * Close channel which id = MAX VALUE
    */
   void ConnectivityAgent_14();

   /*
    * Close not allocated channel
    */
   void ConnectivityAgent_15();

   /*
    * Test isn't yet implemented
    */
   void ConnectivityAgent_16();

   /*
    * Simple send data
    */
   void ConnectivityAgent_17();

   /*
    * Send data to channel with id = 0
    */
   void ConnectivityAgent_18();

   /*
    * Send data to channel with id = MAX VALUE
    */
   void ConnectivityAgent_19();

   /*
    * Send data to channel with id = MAX VALUE + 1
    */
   void ConnectivityAgent_20();

   /*
    * Send data to not allocated channel
    */
   void ConnectivityAgent_21();

   /*
    * Send data. Data is NULL-pointer
    */
   void ConnectivityAgent_22();

   /*
    * Send data. Size isn't greater than 0
    */
   void ConnectivityAgent_23();

   /*
    * Send data. Size = MAX VALUE
    */
   void ConnectivityAgent_24();

   /*
    * Send data. Size = MAX VALUE + 1
    */
   void ConnectivityAgent_25();

   /*
    * Simple send data. Size is normal value
    */
   void ConnectivityAgent_26();

   /*
    * Test isn't yet implemented
    */
   void ConnectivityAgent_27();

   /*
    * Simple receive data
    */
   void ConnectivityAgent_28();

   /*
    * Receive from channel with id = 0
    */
   void ConnectivityAgent_29();

   /*
    * Receive data from channel with id = MAX VALUE
    */
   void ConnectivityAgent_30();

   /*
    * Receive from channel with id = MAX VALUE + 1
    */
   void ConnectivityAgent_31();

   /*
    * Receive data from channel with id = MAX VALUE and receive size = MIN VALUE
    */
   void ConnectivityAgent_32();

   /*
    * Receive data from correctly-allocated channel
    */
   void ConnectivityAgent_33();

   /*
    * Receive data from non-allocated channel
    */
   void ConnectivityAgent_34();

   /*
    * Receive data. Channel id = NORMAL VALUE
    */
   void ConnectivityAgent_35();

   /*
    * Receive from channel with id = NULL
    */
   void ConnectivityAgent_36();

   /*
    * Receive data. Data is a null-pointer
    */
   void ConnectivityAgent_37();

   /*
    * Receive data. Received size is 0
    */
   void ConnectivityAgent_38();

   /*
    * Receive data. Received size is NULL
    */
   void ConnectivityAgent_39();

   /*
    * Receive data. Max size = 0
    */
   void ConnectivityAgent_40();

   /*
    * Receive data. Max size = large is number
    */
   void ConnectivityAgent_41();

   /*
    * Receive data. Received_size = MAX SIZE
    */
   void ConnectivityAgent_42();

   /*
    * Test isn't yet implemented
    */
   void ConnectivityAgent_43();

   /*
    * Get free size. All params are OK
    */
   void ConnectivityAgent_44();

   /*
    * Get free size. Channel id = 0
    */
   void ConnectivityAgent_45();

   /*
    * Get free size. Channel id = MAX VALUE
    */
   void ConnectivityAgent_46();

   /*
    * Get free size. Channel id = MAX VALUE + 1
    */
   void ConnectivityAgent_47();

   /*
    * Get free size. Free size = 0
    */
   void ConnectivityAgent_48();

   /*
    * Test isn't yet implemented
    */
   void ConnectivityAgent_49();

   /*
    * getMaxBufSize
    */
   void ConnectivityAgent_50();

   /*
    * Test isn't yet implemented
    */
   void ConnectivityAgent_51();

   /*
    * Get connection address. All params are OK
    */
   void ConnectivityAgent_52();

   /*
    * Get connection address. Type is null-pointer
    */
   void ConnectivityAgent_53();

   /*
    * Get connection address. Local address is null-pointer
    */
   void ConnectivityAgent_54();

   /*
    * Get connection address. Remote address is null-pointer
    */
   void ConnectivityAgent_55();

private:

   void sync_test(const char* msg = "");

   UInt8*                                       mDataBufSend;
   UInt8*                                       mDataBufReceive;
   CCondVar                                     mWaitCond;
   CMutex								 			      mDataMutex;
   std::map<UInt32, UInt32>				         data_avail;

   CSignalSemaphoreInterproc mSrvSyncSem;
   CSignalSemaphoreInterproc mCliSyncSem;
};
#endif
