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
 

#ifndef CTIMEOUTMANAGER_HPP_
#define CTIMEOUTMANAGER_HPP_

#include <map>

#include "CMutex.hpp"
#include "CThread.hpp"
#include "ITimeoutSubscriber.hpp"
#include "Logger.hpp"


class CSignalSemaphore;
class CThreadPool;


class CTimeoutManager : public CThread
{
public:
   typedef std::multimap<Int64,ITimeoutSubscriber*> tSubscribersMap ;

   /**
    * Returns and creates instance of CTimeoutManager
    */
   static CTimeoutManager * getInstance();

   /**
    * Deletes instance of singleton
    */
   static void deleteInstance();

   /**
    * Adds subscriber to mSubscribers. It calls start() if mSubscribers was empty before adding
    * @param subscriber - interface for callback. After performing timout object will be deleted
    * @param timeout - time in miliseconds 
    */
   void addSubscriber(ITimeoutSubscriber * subscriber, UInt32 ms);

   /**
    * Revomes subscriber from timeout manager
    * @param subscriber pointer to object with ITimeoutSubscriber interface
    */
   void removeSubscriber(ITimeoutSubscriber * subscriber);

   /**
    * Returns time value as int64 value in ms
    */
   static Int64 getCurMs();

private:
   // Methods section
   /**
    * Constructor
    */
   CTimeoutManager();

   /**
    * Destructor
    */
   virtual ~CTimeoutManager();

   /**
    * Is called by SIGALRM handler to handle onTimout of subscribers
    */
   void onTimer();

   /**
    * Main function of Timaout Manager thread. Is called after start
    */
   virtual void threadFunc();

   /**
    * Calls processTopTimeout while it returns true
    */
   void processTimeout();

   /**
    * Processed top elements, if timeout passed
    */
   bool processTopTimeout(Int64 nowMs);

   /**
    * stops its thread
    */
   void finish();

   /**
    * Uses signal semaphore to wait for the next timeout and timeout adding
    */
   void smartWait();

   Logger mLogger;                        ///< Is used for logging
   CMutex * mpSubscriberMapMutex;         ///< Mutes for subscribers map
   tSubscribersMap mSubscriberMap;        ///< Map with subsrcibers, time is a key
   CSignalSemaphore * mpThreadSemaphore;  ///< Semaphore for signal handling
   volatile bool mBe;                              ///< if false - thread exit
   CThreadPool * mpThreadPool;            ///< thead pool is used to perform callbacks of subscribers
   CSignalSemaphore * mpStartSemaphore;   ///< is used to block thread that calls constructor for signal from threadFunc

   static CTimeoutManager * msInstance;   ///< instance of CTimeoutManager
};

#endif /* CTIMEOUTMANAGER_HPP_ */
