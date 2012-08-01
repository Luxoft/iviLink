/* 
 * 
 * iviLINK SDK, version 1.0
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
 * 
 */






/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <cassert>
#include <unistd.h>

/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "framework/components/ConnectivityAgent/generic/common/Buffer.hpp"
#include "IBufferProducer.hpp"
#include "L1InterfaceStub.hpp"

/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CTargetAgent.hpp"

/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "utils/misc/Logger.hpp"

using namespace iviLink::ConnectivityAgent::L1;
Logger CTargetAgent::logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.L1.CTargetAgent"));

CTargetAgent::CTargetAgent(UInt32 channelID) :
   mChannelID(channelID),
   mpBufferProducer(0)
{

}

CTargetAgent::~CTargetAgent()
{

}


ERROR_CODE CTargetAgent::flushBuffers()
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

   return ERR_OK;
}

ERROR_CODE CTargetAgent::consumeBuffer(iviLink::ConnectivityAgent::Buffer* pBuffer)
{
   LOG4CPLUS_INFO(logger, "CTargetAgent::consumeBuffer " + convertIntegerToString((intptr_t)pBuffer));
   mBufferQueueMutex.lock();
   mBufferQueue.push_back(pBuffer);
   mBufferQueueMutex.unlock();
   return visitStub();
}

ERROR_CODE CTargetAgent::registerProducer(IBufferProducer* pProducer)
{
   assert(!mpBufferProducer);
   assert(pProducer);

   mpBufferProducer = pProducer;

   return ERR_OK;
}

ERROR_CODE CTargetAgent::unregisterProducer(IBufferProducer* pProducer)
{
   assert(mpBufferProducer);
   assert(pProducer);
   assert(pProducer == mpBufferProducer);

   mpBufferProducer = NULL;

   return ERR_OK;
}

void CTargetAgent::destroy()
{
   this->~CTargetAgent();
}

ERROR_CODE CTargetAgent::visitStub()
{
   ERROR_CODE ret = ERR_FAIL;

   LOG4CPLUS_TRACE(logger, "CTargetAgent::visitStub()");

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
      ret = L1InterfaceStub::getInstance()->receiveData(*buf, mChannelID );
      mpBufferProducer->returnBufferBack(buf);
   }

   return ret;
}

