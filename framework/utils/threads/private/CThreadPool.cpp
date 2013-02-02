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


/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
 #include <cstdio>
 /********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "CSignalSemaphore.hpp"
#include "CMutex.hpp"
 /********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CThreadPool.hpp"
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/

Logger CThreadPool::logger = Logger::getInstance(LOG4CPLUS_TEXT("utils.threads.CThreadPool"));
Logger CThreadPool::CWorkerThread::logger = Logger::getInstance(LOG4CPLUS_TEXT("utils.threads.CThreadPool.CWorkerThread"));

CThreadPool::CThreadPool(UInt32 num_threads): mAvailableJobs(0)
{
   LOG4CPLUS_TRACE(logger, "CThreadPool()");
   for(UInt32 i =0;i<num_threads;i++)
   {
      char name[20] = "";
      sprintf(name, "WorkerThread %d", i);
      CWorkerThread* worker = new CWorkerThread(name,&mAvailableJobs, &mJobMutex, &mJobQueue);
      mThreadsQueue.push_back(worker);
   }
}

CThreadPool::~CThreadPool()
{
   mJobMutex.lock();
   mJobQueue.clear();
   mJobMutex.unlock();

   // Setting stop flag for all threads
   for (tThreadsQueue::iterator it = mThreadsQueue.begin(); it != mThreadsQueue.end(); ++it)
   {
      (*it)->setStopFlag();
   }

   // Signalling for all threads. Each thread will stop because we have set stop flag
   for (size_t i = 0; i < mThreadsQueue.size(); ++i)
   {
      mAvailableJobs.signal();
   }

   // Destroying all threads. They have already stopped or stopping at the moment. 
   while(!mThreadsQueue.empty())
   {
      std::deque<CWorkerThread* >::iterator iter = mThreadsQueue.begin();
      delete *iter;
      mThreadsQueue.erase(iter);
   }

   // All threads have stopped.
}

void CThreadPool::addJob(CThreadPoolJob& job)
{
   mJobMutex.lock();
   mJobQueue.push_back(job);
   mJobMutex.unlock();
   mAvailableJobs.signal();
}

CThreadPool::CWorkerThread::CWorkerThread(const char* pName,CSignalSemaphore * sem, CMutex * jobMutex, std::deque<CThreadPoolJob> * jobQueue):
   CThread(pName, true),
   mpAvailableJobs(sem),
   mpJobMutex(jobMutex),
   mpJobQueue(jobQueue)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   start();
   mStartSemaphore.wait();
}

CThreadPool::CWorkerThread::~CWorkerThread()
{
   // Thread must be stopped by thread pool
}

void CThreadPool::CWorkerThread::threadFunc()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   mStartSemaphore.signal();
   while (false == getStopFlag())
   {
      mpAvailableJobs->wait();

      if (true == getStopFlag())
      {
         break;
      }

      mpJobMutex->lock();
      if (mpJobQueue->empty())
      {
         mpJobMutex->unlock();
      }
      else
      {
         CThreadPoolJob job = mpJobQueue->front();
         mpJobQueue->pop_front();
         mpJobMutex->unlock();

         job.perform();
      }
   } // while
}


