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


#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "CThread.hpp"

using namespace std;

Logger CThread::logger = Logger::getInstance(LOG4CPLUS_TEXT("utils.threads.CThread"));

CThread::CThread(const char* const pThreadName, bool joinable/* = false*/) :
   mpThreadName(0),
   mStopFlag(false),
   mIsRunning(false),
   mThread(0),
   mpSleepSemaphore(0),
   mJoinable(joinable)
{
   if (pThreadName)
   {
      mpThreadName = new char[strlen(pThreadName) + 1];
      strcpy(mpThreadName, pThreadName);
   }

   pthread_attr_init(&mAttr);

   if (!mJoinable)
   {
      pthread_attr_setdetachstate(&mAttr,PTHREAD_CREATE_DETACHED);
   }

   mpSleepSemaphore = new CSignalSemaphore(0);
}

CThread::~CThread()
{
   stop();
   pthread_attr_destroy(&mAttr);
   if (mpThreadName)
      delete[] mpThreadName;
   if(mpSleepSemaphore)
      delete mpSleepSemaphore;
}

bool CThread::start()
{
   mStopFlagMutex.lock();
   mStopFlag = false;
   mStopFlagMutex.unlock();

   int rc = pthread_create((pthread_t*) &mThread, &mAttr, launchInterface, this);
   if (rc != 0)
   {
      pthread_attr_destroy(&mAttr);
      LOG4CPLUS_ERROR(logger, "CThread::start() Unable to start thread " + string(mpThreadName) + ", error: " + strerror(errno));
      return false;
   }
   return true;
}

bool CThread::stop()
{
   bool ret = false;

   mStopFlagMutex.lock();
   mStopFlag = true;
   mStopFlagMutex.unlock();

   if (mJoinable)
   {
      mpSleepSemaphore->signal();
      if (join()) 
      {
         mThread = 0;
         ret = true;
      }
   }
   
   return ret;
}

bool CThread::stop(CSignalSemaphore *const pSem)
{
   bool ret = false;

   mStopFlagMutex.lock();
   mStopFlag = true;
   mStopFlagMutex.unlock();

   if (pSem)
   {
      pSem->signal();
   }

   if (mJoinable)
   {
      mpSleepSemaphore->signal();
      if (join()) 
      {
         mThread = 0;
         ret = true;
      }
   }
   
   return ret;
}

bool CThread::join()
{
   if (mThread)
   {
      if (isThisThread())
      {
          LOG4CPLUS_FATAL(logger, "CThread::join(" + string(mpThreadName) + ") : joining oneself" );
//        pthread_detach(mThread);
//        pthread_exit(0);
      }
      
      LOG4CPLUS_INFO(logger, "CThread::join() " + convertIntegerToString(pthread_self())
                           + " -> " + convertIntegerToString(mThread));
      int rc = pthread_join(mThread, 0);
      if (0 == rc)
         return true;
      else
      {
         LOG4CPLUS_INFO(logger, "CThread::join() Unable to join thread " + string(mpThreadName)
                              + " error:" + convertIntegerToString(rc) + strerror(rc));
      }
   }
   return false;
}

void* CThread::launchInterface(void* pMyself)
{
   CThread* const pThread = static_cast<CThread*> (pMyself);

   if (pThread)
   {
      LOG4CPLUS_INFO(logger, string(pThread->mpThreadName) + 
         " mThread = " + convertIntegerToString(pThread->mThread));

      pThread->mIsRunning = true;
      pThread->threadFunc();
      pThread->mIsRunning = false;
   }
   return NULL;
}

void CThread::sleep(UInt32 ms)
{
   if (true == mIsRunning)
   {
      mpSleepSemaphore->waitTimeout(ms);
   }
}

bool CThread::isThisThread() const
{
   return pthread_self() == mThread;
}
