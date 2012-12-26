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
 

#include "CTimeoutManager.hpp"
#include "CMutex.hpp"
#include "CThreadPool.hpp"

#include <sys/time.h>


CMutex gsInstanceMutex;
CTimeoutManager * CTimeoutManager::msInstance = 0;


CTimeoutManager * CTimeoutManager::getInstance()
{
   if (0 == CTimeoutManager::msInstance)
   {
      gsInstanceMutex.lock();
      if (0 == CTimeoutManager::msInstance)
      {
         CTimeoutManager::msInstance = new CTimeoutManager();
      }
      gsInstanceMutex.unlock();
   }
   return CTimeoutManager::msInstance;
}

void CTimeoutManager::deleteInstance()
{
   gsInstanceMutex.lock();
   if (msInstance)
   {
      msInstance->finish();
      delete msInstance;
      msInstance = 0;
   }
   gsInstanceMutex.unlock();
}

CTimeoutManager::CTimeoutManager()
   : CThread("CTimeoutManager", true)
   , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("utils")))
   , mpSubscriberMapMutex(new CMutex())
   , mpThreadSemaphore(new CSignalSemaphore())
   , mBe (true)
   , mpThreadPool(new CThreadPool(5))
   , mpStartSemaphore(new CSignalSemaphore())
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

   start();
   mpStartSemaphore->wait();
}

CTimeoutManager::~CTimeoutManager()
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

   mpSubscriberMapMutex->lock();
   for (tSubscribersMap::iterator it = mSubscriberMap.begin();
           mSubscriberMap.end() != it; ++it)
   {
      delete it->second;
   }
   mSubscriberMap.clear();
   mpSubscriberMapMutex->unlock();

   delete mpStartSemaphore;
   delete mpThreadPool;
   delete mpThreadSemaphore;
   delete mpSubscriberMapMutex;
}

void CTimeoutManager::addSubscriber(ITimeoutSubscriber * subscriber, UInt32 ms)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

   Int64 curTime = getCurMs();
   Int64 timeoutTime = curTime + ms;

   bool needReSetTimer = false;
   mpSubscriberMapMutex->lock();

   if (mSubscriberMap.empty() ||
         (timeoutTime < mSubscriberMap.begin()->first && mSubscriberMap.empty()))
   {
      needReSetTimer = true;
   }

   mSubscriberMap.insert(make_pair(timeoutTime, subscriber));
   mpSubscriberMapMutex->unlock();

   if (needReSetTimer)
   {
      mpThreadSemaphore->signal();
   }
}

void CTimeoutManager::removeSubscriber(ITimeoutSubscriber * subscriber)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

   bool needReSetTimer = false;
   mpSubscriberMapMutex->lock();
   if (mSubscriberMap.empty())
   {
      needReSetTimer = true;
   }
   else
   {
      for (tSubscribersMap::iterator it = mSubscriberMap.begin();
            mSubscriberMap.end() != it; ++it)
      {
         if (subscriber == it->second)
         {
            if (it == mSubscriberMap.begin())
            {
               needReSetTimer = true;
            }
            mSubscriberMap.erase(it);
            break;
         }
      }
   }
   mpSubscriberMapMutex->unlock();

   if (needReSetTimer)
   {
      mpThreadSemaphore->signal();
   }
}

Int64 CTimeoutManager::getCurMs()
{
   timeval now;
   gettimeofday(&now, NULL);
   Int64 nowi = ((Int64)now.tv_sec) * 1000;
   nowi += (Int64)now.tv_usec/1000;
   return nowi;
}

void CTimeoutManager::threadFunc()
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
   mpStartSemaphore->signal();

   while (mBe)
   {
      processTimeout();
      smartWait();
   }
}

void CTimeoutManager::smartWait()
{
   bool empty = false;
   Int64 timeout = 0;
   mpSubscriberMapMutex->lock();
   if ( mSubscriberMap.empty())
   {
      empty = true;
   }
   else
   {
      timeout = mSubscriberMap.begin()->first - getCurMs();
   }
   mpSubscriberMapMutex->unlock();
   if (empty)
   {
      mpThreadSemaphore->wait();
   }
   else if (timeout > 0)
   {
      mpThreadSemaphore->waitTimeout(timeout);
   }
}

void CTimeoutManager::processTimeout()
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

   Int64 now = getCurMs();
   while (processTopTimeout(now));
}

class CJobHolder : public IThreadPoolJobData
{
public:
   ITimeoutSubscriber * mpSubscriber;
   CJobHolder(ITimeoutSubscriber * pSubscriber)
      : mpSubscriber(pSubscriber)
   {}
   virtual ~CJobHolder()
   {
      delete mpSubscriber;
   }
};

void jobPerformer(IThreadPoolJobData const* job)
{
   const CJobHolder * jh = static_cast<const CJobHolder*>(job);
   jh->mpSubscriber->onTimeout();
}

bool CTimeoutManager::processTopTimeout(Int64 nowMs)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

   mpSubscriberMapMutex->lock();
   LOG4CPLUS_INFO(mLogger, "now: " + convertIntegerToString(nowMs) + ", first: "
         + convertIntegerToString(mSubscriberMap.begin()->first));
   if (mSubscriberMap.empty() || nowMs < mSubscriberMap.begin()->first)
   {
      mpSubscriberMapMutex->unlock();
      LOG4CPLUS_INFO(mLogger, "return false");
      return false;
   }
   ITimeoutSubscriber * pSubscriber = mSubscriberMap.begin()->second;
   mSubscriberMap.erase(mSubscriberMap.begin());
   mpSubscriberMapMutex->unlock();
   CThreadPoolJob job(jobPerformer,new CJobHolder(pSubscriber));
   mpThreadPool->addJob(job);
   return true;
}

void CTimeoutManager::finish()
{
   LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

   mBe = false;
   mpThreadSemaphore->signal();
   stop();
}

