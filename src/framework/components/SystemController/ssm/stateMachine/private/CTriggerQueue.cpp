/* 
 * 
 * iviLINK SDK, version 1.0.1
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

const string CTriggerQueue::QUEUE_NAME = "/ssm.trigger.queue";
mqd_t CTriggerQueue::Q_DESCRIPTOR = -1;

CTriggerQueue::CTriggerQueue()
{}


CTriggerQueue::~CTriggerQueue()
{}

mqd_t CTriggerQueue::getQueueDescriptor()
{
   LOG4CPLUS_TRACE(sLogger, "getQueueDescriptor()");

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

void CTriggerQueue::sendTrigger(eSSMTriggers trigger)
{
   LOG4CPLUS_TRACE(sLogger, "sendTrigger(" +  convertIntegerToString(static_cast<int>(trigger)) + ")");

   if(-1 == mq_send(getQueueDescriptor(), (const char*)&trigger, sizeof(trigger), 0))
   {
	   LOG4CPLUS_ERROR(sLogger, "Error writing to queue: " + string(strerror(errno)));
   }
}

eSSMTriggers CTriggerQueue::receiveTrigger()
{
   LOG4CPLUS_TRACE(sLogger, "receiveTrigger()");

   eSSMTriggers trigger;
   uint priority;
   struct mq_attr attr;

   mq_getattr(getQueueDescriptor(), &attr);

   if(-1 == mq_receive(getQueueDescriptor(), (char*)&trigger, attr.mq_msgsize, &priority))
   {
      LOG4CPLUS_ERROR(sLogger,"Error reading from queue: " + string(strerror(errno)));
      return EMPTY_TRIGGER_QUEUE;
   }

   return trigger;
}

void CTriggerQueue::resetTriggerQueue()
{
   mq_close(Q_DESCRIPTOR);
   Q_DESCRIPTOR = -1;
   mq_unlink(QUEUE_NAME.c_str());
}

} /* namespace SystemController */
