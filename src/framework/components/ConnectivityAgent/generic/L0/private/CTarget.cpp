/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <cassert>
#include <cstring>
#include <unistd.h>
 
/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "IBufferConsumer.hpp"
#include "Frame.hpp"
#include "Buffer.hpp"
#include "IBufferProducer.hpp"
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CTarget.hpp"
/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "Logger.hpp"

using iviLink::ConnectivityAgent::Buffer;
using namespace iviLink::ConnectivityAgent::L0;
Logger CTarget::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L0.CTarget"));

CTarget::CTarget(UInt32 channel_id) :
   mChannelID(channel_id),
   mpBufferConsumer(NULL),
   mLastFrameNr(0),
   mCarrierReplaced(false)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(logger, "CTarget::CTarget chID = " + convertIntegerToString(channel_id));

   mBufferQueueMutex.lock();
   for (UInt32 i = 0; i < QUEUE_SIZE; ++i)
   {
      Buffer* buf = new Buffer();
      assert(buf);
      mBufferQueue.push_back(buf);
   }
   mBufferQueueMutex.unlock();
}

CTarget::~CTarget()
{
   LOG4CPLUS_TRACE(logger, "CTarget::~CTarget() chID = " + convertIntegerToString(mChannelID));
   if (mpBufferConsumer)
   {
      mpBufferConsumer->flushBuffers();
      mpBufferConsumer->unregisterProducer(this);
   }

   mBufferQueueMutex.lock();
   for (tBufferQueue::iterator it = mBufferQueue.begin();
         it != mBufferQueue.end();
         ++it)
   {
      assert(*it);
      delete *it;
   }
   mBufferQueue.clear();
   mBufferQueueMutex.unlock();
}

ERROR_CODE CTarget::registerConsumer(IBufferConsumer* pConsumer)
{
   assert(!mpBufferConsumer);
   mpBufferConsumer = pConsumer;
   return ERR_OK;
}

ERROR_CODE CTarget::unregisterConsumer(IBufferConsumer* pConsumer)
{
   assert(mpBufferConsumer);
   assert(pConsumer);
   assert(pConsumer == mpBufferConsumer);

   mpBufferConsumer->flushBuffers();
   mpBufferConsumer = 0;

   return ERR_OK;
}

ERROR_CODE CTarget::receiveFrame(const iviLink::ConnectivityAgent::HAL::Frame& frame)
{
   LOG4CPLUS_TRACE(logger, " CTarget::receiveFrame(): channel " + convertIntegerToString(mChannelID));
   static bool firstFlag = true;
   ERROR_CODE ret = ERR_FAIL;
   assert(mChannelID == frame.mFrameHeader.channel_id);

   if((!firstFlag)&&(!mCarrierReplaced)&&(frame.mFrameHeader.number != mLastFrameNr + 1))
   {
      LOG4CPLUS_ERROR(logger, "CTarget::receiveFrame error - ERR_WRONG_FRAME_SEQUENCE: frame.mFrameHeader.number = "
          + convertIntegerToString(frame.mFrameHeader.number) + ", mLastFrameNr + 1 = "
          + convertIntegerToString(mLastFrameNr + 1));
      if(frame.mFrameHeader.number<=mLastFrameNr)
      {
         ret = ERR_RESEND_ACK;
      }
      else
      {
         ret = ERR_WRONG_FRAME_SEQUENCE;
      }   
   }
   else if (mBufferQueue.empty())
   {
      /// @todo: proper error
      LOG4CPLUS_ERROR(logger, "CTarget::receiveFrame error - mBufferQueue.empty()");
      ret =  ERR_FAIL;
   }
   else
   {
      mBufferQueueMutex.lock();
      Buffer* buf = mBufferQueue.front();
      mBufferQueue.pop_front();
      mBufferQueueMutex.unlock();
      assert(buf);

      copyFromFrame(frame, *buf);

      assert(mpBufferConsumer);
      ret = mpBufferConsumer->consumeBuffer(buf);
      if (ret == ERR_OK)
      {
         if(firstFlag) 
         {
            firstFlag =false;
         }
         if(mCarrierReplaced)
         {
            mCarrierReplaced = false;
         }
         mLastFrameNr = frame.mFrameHeader.number;
      }

   }
   return ret;
}

ERROR_CODE CTarget::returnBufferBack(Buffer* pBuffer)
{
   LOG4CPLUS_TRACE(logger, "CTarget::returnBufferBack " + convertIntegerToString((intptr_t)pBuffer));

   assert(pBuffer);

   mBufferQueueMutex.lock();
   mBufferQueue.push_back(pBuffer);
   mBufferQueueMutex.unlock();

   return ERR_OK;
}

ERROR_CODE CTarget::copyFromFrame(const iviLink::ConnectivityAgent::HAL::Frame& frame, Buffer& buffer)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   UInt16 frame_size = frame.getSize();

   buffer.reserveSize(frame_size);

   assert(buffer.getData());
   memcpy(buffer.getData(), frame.data, frame_size);
   buffer.getFilledSize() = frame_size;

   /// @todo proper error messages
   return ERR_OK;
}

