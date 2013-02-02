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
#include <unistd.h>

#include "ConnectivityAgentBuffer.hpp"
#include "IBufferProducer.hpp"
#include "L1InterfaceStub.hpp"
#include "CTargetAgent.hpp"

#include "Logger.hpp"

using namespace iviLink::ConnectivityAgent::L1;
using namespace iviLink::ConnectivityAgent;
Logger CTargetAgent::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L1.CTargetAgent"));

CTargetAgent::CTargetAgent(UInt32 channelID) :
   mChannelID(channelID),
   mpBufferProducer(0)
{

}

CTargetAgent::~CTargetAgent()
{

}


void CTargetAgent::flushBuffers()
{
   assert(mpBufferProducer);

   mBufferQueueMutex.lock();
   for (tBufferQueue::iterator it = mBufferQueue.begin();
         it != mBufferQueue.end();
         ++it)
   {
      assert(*it);
      mpBufferProducer->returnBufferBack(*it);
   }

   mBufferQueue.clear();
   mBufferQueueMutex.unlock();
}

ConnectivityAgentError CTargetAgent::consumeBuffer(iviLink::ConnectivityAgent::Buffer* pBuffer)
{
   LOG4CPLUS_TRACE(logger, "CTargetAgent::consumeBuffer " + convertIntegerToString((intptr_t)pBuffer));
   mBufferQueueMutex.lock();
   mBufferQueue.push_back(pBuffer);
   mBufferQueueMutex.unlock();
   return visitStub();
}

void CTargetAgent::registerProducer(IBufferProducer* pProducer)
{
   assert(!mpBufferProducer);
   assert(pProducer);

   mpBufferProducer = pProducer;
}

void CTargetAgent::unregisterProducer(IBufferProducer* pProducer)
{
   assert(mpBufferProducer);
   assert(pProducer);
   assert(pProducer == mpBufferProducer);

   mpBufferProducer = NULL;
}

void CTargetAgent::destroy()
{
   this->~CTargetAgent();
}

ConnectivityAgentError CTargetAgent::visitStub()
{
   ConnectivityAgentError result = ConnectivityAgentError::NoError();

   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   iviLink::ConnectivityAgent::Buffer* buf = NULL;
   mBufferQueueMutex.lock();
   if (!mBufferQueue.empty())
   {
      buf = mBufferQueue.front();
      mBufferQueue.pop_front();
   }
   mBufferQueueMutex.unlock();

   if (buf)
   {
      result = L1InterfaceStub::getInstance()->receiveData(*buf, mChannelID );
      mpBufferProducer->returnBufferBack(buf);
   }
   else
   {
      result.setErrorCode(ConnectivityAgentError::ERROR_NO_FREE_BUFFERS_LEFT);
   }

   return result;
}

