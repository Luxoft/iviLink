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


#ifndef CONNECTIVITY_AGENT_SERVER_HPP
#define CONNECTIVITY_AGENT_SERVER_HPP
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
 * Other includes
 *
 ********************************************************************/
#include "utils/threads/CThread.hpp"
#include "framework/components/ConnectivityAgent/generic/common/IChannelObserver.hpp"
#include "utils/threads/CSignalSemaphore.hpp"
#include "utils/misc/Types.hpp"
#include "utils/threads/CSignalSemaphoreInterproc.hpp"

class CServer: public IChannelObserver, public CThread
{
private:
   static Logger logger;

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
   virtual void dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size/*, const UInt32 free_size*/);

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

   void ConnectivityAgent_28();
   void ConnectivityAgent_29();
   void ConnectivityAgent_30();
   void ConnectivityAgent_31();
   void ConnectivityAgent_32();
   void ConnectivityAgent_33();
   void ConnectivityAgent_35();
   void ConnectivityAgent_36();
   void ConnectivityAgent_37();
   void ConnectivityAgent_38();
   void ConnectivityAgent_39();
   void ConnectivityAgent_40();
   void ConnectivityAgent_41();
   void ConnectivityAgent_42();

private:

   void sync_test(const char* msg = "");

   UInt8*                                 mDataBuf;
   CSignalSemaphore                       mDeleteSem;


   CSignalSemaphoreInterproc mSrvSyncSem;
   CSignalSemaphoreInterproc mCliSyncSem;
};

#endif
