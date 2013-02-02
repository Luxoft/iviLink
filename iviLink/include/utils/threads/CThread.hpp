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


#ifndef THREAD_HPP
#define THREAD_HPP

#include <pthread.h>

#include "Types.hpp"
#include "CMutex.hpp"
#include "CSignalSemaphore.hpp"

#include "Logger.hpp"

class CSignalSemaphore;

class CThread
{
private:
   static Logger logger;

public:
   explicit CThread(const char* const pThreadName, bool joinable = false);

   virtual ~CThread();

   virtual bool start();

   virtual bool stop();

   virtual bool stop(CSignalSemaphore *const pSem);

   virtual bool join();

   inline bool getStopFlag() const;
  
   inline void setStopFlag();

   inline const char* getName() const;

   void sleep(UInt32 ms);

   /**
    * Checks current thread.
    * @retval true current thread id and mThread equals
    * @retval false otherwise
    */
   bool isThisThread() const;

private:
   virtual void threadFunc() = 0;

   static void* launchInterface(void* pMyself);
   CThread();
   CThread(const CThread& );

   CThread& operator=(const CThread& );

   char*                   mpThreadName; 
   volatile bool           mStopFlag;
   volatile bool           mIsRunning;
   pthread_t               mThread;
   pthread_attr_t          mAttr;

   mutable CMutex          mStopFlagMutex;
   CSignalSemaphore*       mpSleepSemaphore;

   const bool mJoinable;
};

bool CThread::getStopFlag() const
{
   mStopFlagMutex.lock();
   bool stop = mStopFlag;
   mStopFlagMutex.unlock();
   return stop;
}

void CThread::setStopFlag()
{
   mStopFlagMutex.lock();
   mStopFlag = true;
   mStopFlagMutex.unlock();
}

const char* CThread::getName() const
{
   return mpThreadName;
}

#endif
