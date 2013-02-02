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


#ifndef CHANNEL_SUPERVISOR_CLIENT_HPP
#define CHANNEL_SUPERVISOR_CLIENT_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <map>
#include <set>
 /********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "CThread.hpp"
#include "CCondVar.hpp"
#include "IChannelSupervisorObserver.hpp"
#include "CSignalSemaphoreInterproc.hpp"
#include "Logger.hpp"

///CClient class
/**
 * The CClient incapsulates testing features of Connectivity Agent. It uses Agent's API for basic functionality and also 
 * implements IChannelSupervisorTubeObserver interface to receive callbacks from Agent.It implements CThread as well to perform testing 
 * activity in separate thread
 */

using iviLink::ChannelSupervisor::IChannelSupervisorTubeObserver;
class CClient: public IChannelSupervisorTubeObserver, public CThread
{

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
   virtual void dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size);

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

    /**
    * Wrong reserved channel value “0” trying to use
    */
   void ChannelSupervisor_03();

   /**
    * Trying to send by channel already in use
    */
   void ChannelSupervisor_04();

   /**
    * Sending data to not allocated channel 
    */
   void ChannelSupervisor_05();


   /**
    * Sending data to with Null pointer to data 
    */
   void ChannelSupervisor_07();

   /**
    * Sending data with correct data pointer and minimum data size value
    */
   void ChannelSupervisor_08();

   /**
    * Send “0”size data to sendData()
    */
   void ChannelSupervisor_09();

    /**
    * Wrong null value sending as a channel number
    */
   void ChannelSupervisor_12();

    /**
    * Recieve data from correctly allocated channel
    */
   void ChannelSupervisor_15();

   /**
    * Recieve data to not allocated channel
    */
   void ChannelSupervisor_16();

   /**
    * Check for unable to send request with null pointer to data
    */
   void ChannelSupervisor_18();

   /**
    * Check for data recieved size is UINT_MAX
    */
   void ChannelSupervisor_21();

   /**
    * Check for data buffer size isn't “0” value for reciveData()
    */
   void ChannelSupervisor_22();

   /**
    * Correct channel allocating
    */
   void ChannelSupervisor_28();

   /**
    * Allocating channel with null pointer to observer object 
    */
   void ChannelSupervisor_31();

   /**
    * Closing reserved channel value "0"
    */
   void ChannelSupervisor_32();

   /**
    * Successful channel close 
    */
   void ChannelSupervisor_33();

   /**
    * Trying to close not allocated channel 
    */
   void ChannelSupervisor_34();

   /**
    * Close channel with wrong channel_id "null" 
    */
   void ChannelSupervisor_36();

   /**
    * Close channel with empty TAG
    */
   void ChannelSupervisor_40();

   /**
    * Close channel with TAG = 0
    */
   void ChannelSupervisor_41();

   /**
    * sendData with all params OK
    */
   void ChannelSupervisor_42();

   /**
    * sendData with empty TAG
    */
   void ChannelSupervisor_43();

   /**
    * sendData with 0 TAG
    */
   void ChannelSupervisor_44();

   /**
    * receiveData with channel id = 0
    */
   void ChannelSupervisor_45();

   /**
    * receiveData with empty TAG
    */
   void ChannelSupervisor_46();

   /**
    * receiveData with 0 TAG
    */
   void ChannelSupervisor_47();

   /**
    * receiveData with received size 0
    */
   void ChannelSupervisor_48();

   /**
    * receiveData with max size 0
   */
   void ChannelSupervisor_49();

   /**
    * receiveData with received size > max size
   */
   void ChannelSupervisor_50();

   /**
    * receiveData with received size = max size
   */
   void ChannelSupervisor_51();

   void both_sides_close_test();
   void one_side_close_test();
   void one_side_close_same_thread_test();
   void multiple_open_close_same_tag_test();


private:

   void sync_test(const char* msg = "");

   UInt8*                                       mDataBufSend;
   UInt8*                                       mDataBufReceive;
   CCondVar                                     mWaitCond;
   //CMutex								 			      mDataMutex;
   std::map<UInt32, UInt32>				         data_avail;

   CCondVar                               mDeleteCond;
   std::set<UInt32>                       mDeletedChannels;

   CSignalSemaphoreInterproc              mSrvSyncSem;
   CSignalSemaphoreInterproc              mCliSyncSem;

private:

   static Logger msLogger;
};
#endif
