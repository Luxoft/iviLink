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
#ifndef TRIGGER_QUEUE_HPP
#define TRIGGER_QUEUE_HPP

#include <tr1/memory>
#include <deque>

#include "CMutex.hpp"
#include "CCondVar.hpp"
#include "Logger.hpp"

namespace iviLink
{

template<typename T>
class TriggerQueue
{	
	template<typename Y>
 	friend void deleterFunc(TriggerQueue<Y> * queue);
 public:
    typedef T Trigger;

   static TriggerQueue<Trigger>* getInstance();
   void sendTrigger(Trigger trigger);
   Trigger receiveTrigger();
   void resetTriggerQueue();

private:
	static CMutex singletonMutex;
   	static std::tr1::shared_ptr< TriggerQueue<Trigger> > instance;
   	static Logger sLogger;

   	CCondVar mCondVar;
   	bool mIsQueueEmpty;

   	CMutex mQueueMutex;
   	std::deque<Trigger> mQueue;

	TriggerQueue() 
   	{	
   		LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   		mIsQueueEmpty = true;
   	}

   	~TriggerQueue()
   	{
   		LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   	}
};


template<typename T>
CMutex TriggerQueue<T>::singletonMutex;

template<typename T>
std::tr1::shared_ptr< TriggerQueue<T> > TriggerQueue<T>::instance;

template<typename T>
Logger TriggerQueue<T>::sLogger = Logger::getInstance("iviLink.libraries.TriggerQueue");


template<typename T>
void deleterFunc(TriggerQueue<T> * queue)
{
	delete queue;
}

template<typename T>
TriggerQueue<T>* TriggerQueue<T>::getInstance()
{
   	LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
	MutexLocker lock(singletonMutex);
	if(instance.get() == NULL)
	{
		instance = std::tr1::shared_ptr< TriggerQueue<T> > (new TriggerQueue<T>(), deleterFunc<T>);
	}
	assert(instance.get() != NULL);
	return instance.get();
}

template<typename T>
void TriggerQueue<T>::sendTrigger(TriggerQueue<T>::Trigger trigger)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, "sendTrigger(" +  convertIntegerToString(static_cast<int>(trigger)) + ")");
   mQueueMutex.lock();
   mQueue.push_back(trigger);
   mQueueMutex.unlock();
   
   mCondVar.lock();
   mIsQueueEmpty = false;
   mCondVar.signal();
   mCondVar.unlock();
}

template<typename T>
T TriggerQueue<T>::receiveTrigger()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   Trigger trigger;
   bool queueState;
   mCondVar.lock();
   while(mIsQueueEmpty)
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
      LOG4CPLUS_ERROR(sLogger, "reading from empty queue for some reason");
      assert(false);
   }
   queueState = mQueue.empty();
   mQueueMutex.unlock();
   
   mCondVar.lock();
   if(mQueue.empty())
   {
      mIsQueueEmpty=queueState;
   }
   mCondVar.unlock();

   return trigger;
}

template<typename T>
void TriggerQueue<T>::resetTriggerQueue()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   mCondVar.lock();
   mIsQueueEmpty = true;
   mCondVar.unlock();
   mQueueMutex.lock();
   mQueue.clear();
   mQueueMutex.unlock();
}

} // namespace iviLink
#endif //TRIGGER_QUEUE_HPP 
