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


#include <cassert>
#include <cstring>
#include <unistd.h>

#include "ConnectivityAgentBuffer.hpp"
#include "IBufferConsumer.hpp"

#include "CSourceAgent.hpp"
#include "Logger.hpp"
 
using namespace iviLink::ConnectivityAgent::L1;
using namespace iviLink::ConnectivityAgent;
Logger CSourceAgent::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L1.CSourceAgent"));

CSourceAgent::CSourceAgent(UInt32 channelID) :
   mChannelID(channelID), mpBufferConsumer(NULL)
{
   LOG4CPLUS_TRACE_METHOD(logger, "CSourceAgent::CSourceAgent(channelID = " + convertIntegerToString(channelID) + ")");
   for (UInt32 i = 0; i < QUEUE_SIZE; ++i)
   {
      Buffer* buf = new Buffer();
      mBufferQueue.push_back(buf);
   }
}

CSourceAgent::~CSourceAgent()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   for (std::list<Buffer*>::iterator it = mBufferQueue.begin();
         it != mBufferQueue.end();
         ++it)
   {
      delete *it;
   }
   mBufferQueue.clear();
}

void CSourceAgent::registerConsumer(IBufferConsumer* pConsumer)
{
   assert(!mpBufferConsumer);
   assert(pConsumer);
   mpBufferConsumer = pConsumer;
}

void CSourceAgent::unregisterConsumer(IBufferConsumer* pConsumer)
{
   assert(mpBufferConsumer);
   assert(pConsumer);
   assert(mpBufferConsumer == pConsumer);

   mpBufferConsumer->flushBuffers();
   mpBufferConsumer = 0;

   this->destroy();
}

void CSourceAgent::returnBufferBack(iviLink::ConnectivityAgent::Buffer* buffer)
{
   LOG4CPLUS_TRACE(logger, "CSourceAgent::returnBufferBack " + convertIntegerToString((intptr_t)buffer));
   mBufferQueueMutex.lock();
   mBufferQueue.push_back(buffer);
   mBufferQueueMutex.unlock();
}

void CSourceAgent::destroy()
{
   this->~CSourceAgent();
}

ConnectivityAgentError CSourceAgent::fillBuffer(iviLink::ConnectivityAgent::Buffer& buffer)
{
   ConnectivityAgentError result;
   if (mpBufferConsumer)
   {
      mBufferQueueMutex.lock();

      if (!mBufferQueue.empty())
      {
         Buffer* buf = mBufferQueue.front();
         mBufferQueue.pop_front();
         assert(buf);

         memcpy(buf, &buffer, sizeof(iviLink::ConnectivityAgent::Buffer));
         buffer.forgetData();

         mpBufferConsumer->consumeBuffer(buf);

         result.setNoError();
      }
      else
      {
         result.setErrorCode(ConnectivityAgentError::ERROR_NO_FREE_BUFFERS_LEFT);
      }

      mBufferQueueMutex.unlock();
   }

   return result;
}

bool CSourceAgent::isQueueEmpty() const
{
   mBufferQueueMutex.lock();
   bool res = mBufferQueue.empty();
   mBufferQueueMutex.unlock();
   return res;
}
