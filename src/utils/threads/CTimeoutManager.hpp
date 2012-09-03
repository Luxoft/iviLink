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











#ifndef CTIMEOUTMANAGER_HPP_
#define CTIMEOUTMANAGER_HPP_

#include <map>
#include <list>

#include "utils/misc/Types.hpp"
#include "utils/threads/CThread.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/threads/ITimeoutSubscriber.hpp"

/// CCallbackThread
/**
 * CCallbackThread is helper class for CTimeoutManager. It's used for making callbacks in new threads
 */

class CCallbackThread : public CThread
{
public:
   CCallbackThread(ITimeoutSubscriber * subscriber, UInt32 timeout);
   bool isEnded () const;

private:
   virtual void threadFunc();

   ITimeoutSubscriber * mSubscriber;
   UInt32 mTimeout;
   bool mThreadEnded;
   /**
    * static members
    */
   static UInt32 sCounter;
   static char sCounterString[24];
};

/// CTimeoutManager class
/**
 * CTimeoutManager is used for calling onTimeout() function in subscribers when time is passed
 *
 * @todo Reimplement timeout manager. PIlin, 31.08.12
 * 1) Timeout manager can quickly create a lot of threads and creation of new threads
 *    will be blocked by the system. It is dangerous for applicaton execution. There 
 *    must be fixed number of threads dedicated to timeout execution.
 *    ConnectivityAgent 
 *
 *    I propose next arcithecture: one thread with sorted queue of timeout subscribers.
 *    They can be called from this thread or from the threads of thread pool.
 *
 * 2) It will be good, if it will be possible to remove timeouts before time.
 *    It will decrease load.
 *
 * 3) Current implementation uses sleep(). sleep() can exit before actual sleeping
 *    time alarmed by some signal. It must be fixed.
 */

class CTimeoutManager : public CThread
{
public:
   static CTimeoutManager * getInstance();

   /**
    * Destructor
    */
   ~CTimeoutManager();

   /**
    * Adds subscriber to mSubscribers. It calls start() if mSubscribers was empty before adding
    * @param subscriber - interface for callback
    * @param timeout - time in miliseconds 
    */
   void addSubscriber(ITimeoutSubscriber * subscriber, UInt32 timeout);

private:
   // Methods section
   /**
    * Constructor
    */
   CTimeoutManager();

   /**
    * It calls callback-functions if needed
    */

   virtual void threadFunc();
   /**
    * Destructs CCallbackThread objects from mCallbackThreads if is was ended
    */
   void cleanEndedThreads();

   CMutex mMutex;
   std::list<CCallbackThread*> mCallbackThreads;
   static CTimeoutManager * msInstance;
};

#endif /* CTIMEOUTMANAGER_HPP_ */
