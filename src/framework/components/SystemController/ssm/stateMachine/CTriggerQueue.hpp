/* 
 * 
 * iviLINK SDK, version 1.1.2
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









#ifndef TRIGGERQUEUE_HPP_
#define TRIGGERQUEUE_HPP_
#ifndef ANDROID
#include <mqueue.h>
#else
#include "utils/threads/CCondVar.hpp"
#include "utils/threads/CMutex.hpp"
#include <deque>
#endif //ANDROID
#include <string>
#include "SSMTriggers.hpp"
#include "utils/misc/Logger.hpp"

namespace SystemController
{

class CTriggerQueue
{
private:
   static Logger sLogger;

   #ifndef ANDROID
   static const std::string QUEUE_NAME;
   static mqd_t Q_DESCRIPTOR;

   static mqd_t getQueueDescriptor();
   
   #else
   static CCondVar mCondVar;
   static CMutex mQueueMutex;
   static std::deque<eSSMTriggers> mQueue;
   static bool isQueueEmpty;
   #endif //ANDROID

   CTriggerQueue();
   virtual ~CTriggerQueue();

public:
   static void sendTrigger(eSSMTriggers trigger);
   static eSSMTriggers receiveTrigger();

   static void resetTriggerQueue();
};

} /* namespace SystemController */
#endif /* TRIGGERQUEUE_HPP_ */
