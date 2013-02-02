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


#ifndef CHANNEL_SUPERVISOR_SERVER_HPP
#define CHANNEL_SUPERVISOR_SERVER_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <time.h>
#include <unistd.h>
#include <cstring>
#include <limits.h>
#include <set>
 /********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/threads/CThread.hpp"
#include "framework/components/ChannelSupervisor/Tube/IChannelSupervisorObserver.hpp"
#include "utils/threads/CCondVar.hpp"
#include "utils/misc/Types.hpp"
#include "utils/threads/CSignalSemaphoreInterproc.hpp"

using iviLink::ChannelSupervisor::IChannelSupervisorTubeObserver;

class CServer: public IChannelSupervisorTubeObserver, public CThread
{

  public:
   /**
    * Constructor
    */
   CServer();

   /**
    * Destructor
    */
  ~CServer();

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

   void ChannelSupervisor_04();

   void ChannelSupervisor_07();

   void ChannelSupervisor_08();

   void ChannelSupervisor_09();

   /**
    * Allocates channel 1 and send data 
    */
   void ChannelSupervisor_15();
   void ChannelSupervisor_18();

     /**
    * Allocates channel 1 and send data 
    */
   void ChannelSupervisor_21();

   /**
    * Allocates channel 1 and send data 
    */
   void ChannelSupervisor_22();

   void ChannelSupervisor_23();

   /**
    * Allocates channel 25
    */
   void ChannelSupervisor_24();

   /**
    * Try allocate channel 1 and waits for channel close if it is existing.
    * Then reopens channel 1 
    */
   //void ChannelSupervisor_25();

   /**
    * Try allocate channel 0xFFFE and waits for channel close if it is existing.
    * Then reopens channel 0xFFFE
    */
   //void ChannelSupervisor_27();

   /**
    * Try allocate channel 1 with Safety Critical priority and waits for channel close if it is existing.
    * Then reopens channel 1 with Safety Critical priority
    */
   void ChannelSupervisor_28();

   void ChannelSupervisor_33();

   /*
    * Allocate chanel with empty TAG
    */
   void ChannelSupervisor_37();

   /*
    * Allocate chanel with null TAG
    */
   void ChannelSupervisor_38();

   /*
    * sent null as channel_id ref
   */
   void ChannelSupervisor_39();

   /*
    * init with all params OK
    */
   void ChannelSupervisor_52();

   /*
    * init with observer null pointer
    */
   void ChannelSupervisor_53();

   /*
    * init with 0 TAG
    */
   void ChannelSupervisor_54();
   /*
    * init with all params OK
    */
   void ChannelSupervisor_55();

   void both_sides_close_test();
   void one_side_close_test();
   void one_side_close_same_thread_test();
   void multiple_open_close_same_tag_test();

   private:

   void sync_test(const char* msg = "");


   UInt8*                                 mDataBuf;
   UInt8*                                 mDataBufReceive;
   
   CCondVar                               mDeleteCond;
   std::set<UInt32>                       mDeletedChannels;

   CSignalSemaphoreInterproc              mSrvSyncSem;
   CSignalSemaphoreInterproc              mCliSyncSem;

};

#endif
