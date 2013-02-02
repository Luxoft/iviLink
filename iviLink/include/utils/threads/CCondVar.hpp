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


#ifndef COND_VAR_HPP
#define COND_VAR_HPP

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <pthread.h>
#include <time.h>

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/

/// Class uses pthread_cond (with pthread_mutex).
/// For explanation of usage, see pthread_cond documentation.
class CCondVar
{
public:
   // Methods section

   /**
    * Default constructor
    */
   CCondVar();

   /**
    * Destructor
    */
   virtual ~CCondVar();

   /**
    * Signals the condition variable and wakes up one waiting thread (if there
    * are any).
    * Condvar must be locked with lock() before calling signal()
    */
   virtual void signal();

   /**
    * Signals the condition variable and wakes up all waiting thread (if there
    * are any).
    * Condvar must be locked with lock() before calling broadcast()
    */
   virtual void broadcast();

   /**
    * Waits for a signal.
    * Condvar must be locked with lock() before calling wait()
    */
   virtual void wait();

   /**
    * Waits for a signal with a timeout.
    * Be very accurate to use it in cycle - can lead to longer waiting that 
    * specified.
    * Condvar must be locked with lock() before calling wait().
    *
    * @param miliSec Count of miliseconds to wait
    * @return see pthread_cond_timedwait
    */
   int waitTimeout(const time_t miliSec);

   /**
    * Calculates the point time when waiting must be stopped from current time.
    * @param miliSec Count of miliseconds to wait
    */
   static timespec calcAbsTimeout(const time_t miliSec);

   /**
    * Waits for a signal with a timeout.
    * Preferred to be used in cycle.
    * Condvar must be locked with lock() before calling wait().
    *
    * @param ts point in time when witing must be stopped
    * @return see pthread_cond_timedwait
    */
   int waitTimeout(timespec const& ts);


   /**
    * Locks the mutex which used with condvar.
    * Must be called before calling wait(), signal() and broadcast() or there
    * will be undefined behaviour. 
    * See pthread cond var documentation.
    *
    * @return see pthread_mutex_lock documentation
    */
   virtual int lock();

   /**
    * Unlocks the mutex which used with condvar.
    * Must be called after calling wait(), signal() and broadcast() or there
    * will be undefined behaviour. 
    * See pthread cond var documentation.
    *
    * @return see pthread_mutex_lock documentation
    */
   virtual int unlock();

private:
   // Methods section

   /**
    * Copying constructor
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is constructing on
    */
   CCondVar(const CCondVar& classRef);

   /**
    * Assigning operator
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is assiging from
    */
   CCondVar& operator=(const CCondVar& classRef);

private:
   // Members section
   pthread_mutex_t   mMutex;
   pthread_cond_t    mCond;
};
#endif 
