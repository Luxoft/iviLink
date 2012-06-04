/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */





#ifndef CTIMEOUTMANAGER_HPP_
#define CTIMEOUTMANAGER_HPP_

#include <map>
#include <list>

#include "utils/misc/include/Types.hpp"
#include "utils/threads/include/CThread.hpp"
#include "utils/threads/include/CMutex.hpp"
#include "utils/threads/include/ITimeoutSubscriber.hpp"

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
