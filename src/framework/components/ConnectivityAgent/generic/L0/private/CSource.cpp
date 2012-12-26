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
 

#include <cassert>
#include <unistd.h>

#include "Buffer.hpp"
#include "IBufferProducer.hpp"

#include "CSource.hpp"

#include "Logger.hpp"

using iviLink::ConnectivityAgent::Buffer;
using iviLink::ConnectivityAgent::L1::IBufferProducer;
using namespace iviLink::ConnectivityAgent::L0;

Logger CSource::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L0.CSource"));

CSource::CSource(const UInt32 channel_id, CSignalSemaphore& onDataSem):
   mChannelID(channel_id),
   mFrameCounter(1),
   mpBufferProducer(0),
   mOnDataSem(onDataSem)
{
   LOG4CPLUS_TRACE(logger, "CSource::CSource( chID = "
         + convertIntegerToString(channel_id) + " ) this = "
         + convertIntegerToString((intptr_t)this));
}
CSource::~CSource()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   if (mpBufferProducer)
   {
      mpBufferProducer->unregisterConsumer(this);
   }
}

ERROR_CODE CSource::registerProducer(IBufferProducer* pProducer)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   assert(!mpBufferProducer);
   assert(pProducer);

   mpBufferProducer = pProducer;
   return ERR_OK;
}

ERROR_CODE CSource::unregisterProducer(IBufferProducer* pProducer)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   assert(mpBufferProducer);
   assert(pProducer);
   assert(mpBufferProducer == pProducer);

   mpBufferProducer = 0;
   return ERR_OK;
}

bool CSource::hasData() const
{
   mBufferQueueMutex.lock();
   bool empty = mBuffersQueue.empty();
   mBufferQueueMutex.unlock();
   return !empty;
}

ERROR_CODE CSource::flushBuffers()
{
   assert(mpBufferProducer);

   mBufferQueueMutex.lock();

   std::list<Buffer*> qcopy(mBuffersQueue.begin(), mBuffersQueue.end());
   mBuffersQueue.clear();
   
   mBufferQueueMutex.unlock();

   for (std::list<Buffer*>::iterator it = qcopy.begin();
         it != qcopy.end();
         ++it)
   {
      assert(*it);
      mpBufferProducer->returnBufferBack(*it);
   }


   return ERR_OK;
}

ERROR_CODE CSource::consumeBuffer(Buffer* pBuffer)
{
   assert(pBuffer);
   LOG4CPLUS_TRACE(logger, "CSource[chID "
        + convertIntegerToString(mChannelID) + "]::consumeBuffer "
        + convertIntegerToString((intptr_t)pBuffer));

   mBufferQueueMutex.lock();
   mBuffersQueue.push_back(pBuffer);
   mBufferQueueMutex.unlock();
   
   mOnDataSem.signal();

   return ERR_OK;
}

ERROR_CODE CSource::OnDispatch(iviLink::ConnectivityAgent::HAL::Frame& frame_to_fill, const UInt32 quote)
{
   LOG4CPLUS_TRACE(logger, "CSource::OnDispatch chID = "
         + convertIntegerToString(mChannelID));

   mBufferQueueMutex.lock();

   // This is possible when flushBuffers() was called between hadData() checking
   // and OnDispatch().
   if (mBuffersQueue.empty())
   {
      mBufferQueueMutex.unlock();

      return ERR_FAIL;
   }

   std::list<Buffer*> released_buffers = CopyToFrame(frame_to_fill, quote);

   std::list<Buffer*>::iterator q_it = mBuffersQueue.begin();
   for (std::list<Buffer*>::iterator it = released_buffers.begin();
         it != released_buffers.end(); ++it)
   {

      // Removing released buffer from queue
      assert(q_it != mBuffersQueue.end() && *q_it == *it);
      q_it = mBuffersQueue.erase(q_it);
   }

   mBufferQueueMutex.unlock();

   assert(mpBufferProducer);

   for (std::list<Buffer*>::iterator it = released_buffers.begin();
         it != released_buffers.end(); ++it)
   {
      mpBufferProducer->returnBufferBack(*it);
   }

   return ERR_OK;
}

std::list<Buffer*> CSource::CopyToFrame(iviLink::ConnectivityAgent::HAL::Frame& frame, const UInt32 quote)
{
      /// @todo: quote ==0 means Safety Critical source->all data available should be transmitted?
   LOG4CPLUS_INFO(logger, "todo: quote ==0 means Safety Critical source->all data available should be transmitted?");

   std::list<Buffer*> released_buffers;

   UInt32 copy_size = 0;

   bool full_copy = false;

   frame.mFrameHeader.channel_id = this->mChannelID;
   frame.mFrameHeader.number = this->mFrameCounter++;
    
    std::list<Buffer*>::iterator it = mBuffersQueue.begin();
    Buffer& buf = **it;

      LOG4CPLUS_INFO(logger, "CSource::CopyToFrame buf = "
           + convertIntegerToString((intptr_t)&buf) + ", buf.data = "
           + convertIntegerToString((intptr_t)buf.getData()) + ", buf.filled_size = "
           + convertIntegerToString(buf.getFilledSize()));

      if ((buf.getFilledSize() <= quote))
      {
         copy_size = buf.getFilledSize();
         full_copy = true;
      }
      else
      {
         copy_size = quote;
         full_copy = false;
      }

      std::copy(buf.getData(), buf.getData() + copy_size, frame.data);

      if (full_copy)
      {
         // Only full-copied buffer are released
         released_buffers.push_back(&buf);
      }
      else
      {
         // If buffer copied to frame not entirely, only the not-copied data is left
         std::copy(buf.getData() + copy_size, buf.getData() + buf.getFilledSize(), buf.getData());
         buf.getFilledSize() -= copy_size;
      }

   frame.setSize(copy_size);

   return released_buffers;
}
