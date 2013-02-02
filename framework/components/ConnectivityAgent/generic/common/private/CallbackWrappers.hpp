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


#ifndef CALLBACK_WRAPPERS_HPP
#define CALLBACK_WRAPPERS_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <cassert>
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "Types.hpp"
#include "IChannelObserver.hpp"


class CCallbackWrapper
{
   public:

      virtual void execute() = 0;
      virtual ~CCallbackWrapper(){ mpObserver = NULL;}
   protected:

      explicit CCallbackWrapper( IChannelObserver* pObserver): mpObserver(pObserver){ assert(pObserver); }

   protected:

      IChannelObserver* mpObserver;
};

class CDataReceivedCallback :public CCallbackWrapper
{
 
   public:

      CDataReceivedCallback( IChannelObserver* pObserver, const UInt32 channel_id, const UInt32 read_size):
         CCallbackWrapper(pObserver), mChannelID(channel_id),mReadSize(read_size)
         {}

      virtual ~CDataReceivedCallback(){}

      virtual void execute()
      {
         LOG4CPLUS_TRACE_METHOD(Logger::getInstance("CDataReceivedCallback"), __PRETTY_FUNCTION__);
         mpObserver->dataReceivedCallback(mChannelID, mReadSize);
      }

   private:

      UInt32               mChannelID;
      UInt32               mReadSize;
};

class CBufferOverflowCallback :public CCallbackWrapper
{
 
   public:

      CBufferOverflowCallback( IChannelObserver* pObserver, const UInt32 channel_id):
         CCallbackWrapper(pObserver), mChannelID(channel_id){}

      virtual ~CBufferOverflowCallback() { }

      virtual void execute()
      {
         mpObserver->bufferOverflowCallback(mChannelID);
      }

   private:

      UInt32               mChannelID;
};


class CChannelDeletedCallback :public CCallbackWrapper
{
 
   public:

      CChannelDeletedCallback( IChannelObserver* pObserver, const UInt32 channel_id):
         CCallbackWrapper(pObserver), mChannelID(channel_id){}

      virtual ~CChannelDeletedCallback() { }
      virtual void execute()
      {
         mpObserver->channelDeletedCallback(mChannelID);
      }

   private:
      UInt32               mChannelID;
};

class CConnectionLostCallback :public CCallbackWrapper
{
 
   public:

      explicit CConnectionLostCallback( IChannelObserver* pObserver):CCallbackWrapper(pObserver){}

      virtual ~CConnectionLostCallback() { }

      virtual void execute()
      {
         mpObserver->connectionLostCallback();
      }
};
#endif
