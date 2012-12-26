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
 

#include <cstring>
#include <cerrno>
#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <stdlib.h>

#include "CTriggerQueue.hpp"

using namespace std;

namespace SystemController
{

Logger CTriggerQueue::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.CSystemStateMachine.CTriggerQueue"));

#ifndef ANDROID
const string CTriggerQueue::QUEUE_NAME = "/ssm.trigger.queue";
mqd_t CTriggerQueue::Q_DESCRIPTOR = -1;
#else
CCondVar CTriggerQueue::mCondVar;
CMutex CTriggerQueue::mQueueMutex;
bool CTriggerQueue::isQueueEmpty = true;
deque<eSSMTriggers> CTriggerQueue::mQueue = std::deque<eSSMTriggers>();  
#endif //ANDROID

CTriggerQueue::CTriggerQueue()
{}


CTriggerQueue::~CTriggerQueue()
{}

#ifndef ANDROID
mqd_t CTriggerQueue::getQueueDescriptor()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   if(-1 == Q_DESCRIPTOR)
   {
      mqd_t qDescriptor = mq_open(QUEUE_NAME.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, NULL);

      if(-1 == qDescriptor)
      {
         LOG4CPLUS_ERROR(sLogger, "Error creating message queue: " + string(strerror(errno)));
         return -1;
      }
      else
      {
         Q_DESCRIPTOR = qDescriptor;
      }
   }

   LOG4CPLUS_INFO(sLogger, "return message descriptor = " + convertIntegerToString(Q_DESCRIPTOR));

   return Q_DESCRIPTOR;
}
#endif //ANDROID

void CTriggerQueue::sendTrigger(eSSMTriggers trigger)
{
   LOG4CPLUS_TRACE(sLogger, "sendTrigger(" +  convertIntegerToString(static_cast<int>(trigger)) + ")");
#ifndef ANDROID
   if(-1 == mq_send(getQueueDescriptor(), (const char*)&trigger, sizeof(trigger), 0))
   {
	   LOG4CPLUS_ERROR(sLogger, "Error writing to queue: " + string(strerror(errno)));
   }
#else
   mQueueMutex.lock();
   mQueue.push_back(trigger);
   mQueueMutex.unlock();
   
   mCondVar.lock();
   isQueueEmpty = false;
   mCondVar.signal();
   mCondVar.unlock();
#endif
}

eSSMTriggers CTriggerQueue::receiveTrigger()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   eSSMTriggers trigger;
#ifndef ANDROID
   uint priority;
   struct mq_attr attr;

   mq_getattr(getQueueDescriptor(), &attr);

   if(-1 == mq_receive(getQueueDescriptor(), (char*)&trigger, attr.mq_msgsize, &priority))
   {
      LOG4CPLUS_ERROR(sLogger,"Error reading from queue: " + string(strerror(errno)));
      return EMPTY_TRIGGER_QUEUE;
   }
#else
   bool tempQState;
   mCondVar.lock();
   while(isQueueEmpty)
   {
      mCondVar.wait();
   }
   mCondVar.unlock();
   
   mQueueMutex.lock();
   if(!mQueue.empty())
   {
      trigger = mQueue.front();
      mQueue.pop_front();
   }
   else
   {
      trigger = EMPTY_TRIGGER_QUEUE; //this should never happen actually
      LOG4CPLUS_ERROR(sLogger, "reading from empty queue for some reason");
   }
   tempQState = mQueue.empty();
   mQueueMutex.unlock();
   
   mCondVar.lock();
   if(mQueue.empty())
   {
      isQueueEmpty=tempQState;
   }
   mCondVar.unlock();
#endif //ANDROID

   return trigger;
}

void CTriggerQueue::resetTriggerQueue()
{
#ifndef ANDROID
   mq_close(Q_DESCRIPTOR);
   Q_DESCRIPTOR = -1;
   mq_unlink(QUEUE_NAME.c_str());
#else
   mCondVar.lock();
   isQueueEmpty = true;
   mCondVar.unlock();
   mQueueMutex.lock();
   mQueue.clear();
   mQueueMutex.unlock();
#endif //ANDROID
}

} /* namespace SystemController */
