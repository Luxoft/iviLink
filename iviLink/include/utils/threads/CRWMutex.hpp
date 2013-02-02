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


#ifndef CRWMUTEX_HPP
#define CRWMUTEX_HPP

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <pthread.h>
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/

/* Implementation of RWMutex for Android 2.2 and earlier from:
   http://stackoverflow.com/questions/244316/reader-writer-locks-in-c/244376#244376
*/
#ifndef ANDROID
#else
struct rwlock {
    pthread_mutex_t lock;
    pthread_cond_t read, write;
    unsigned readers, writers, read_waiters, write_waiters;
};
#endif //ANDROID
 
/// CRWMutex class
/**
 * The CRWMutex class
 * @class CRWMutex MutexRW.hpp Framework/Threads/MutexRW.hpp
 */
 ///CRWMutex class
/**
 * The CRWMutex class implements read/write mutex
 */
class CRWMutex
{
public:
   // Methods section

   /**
    * Default constructor
    */
   CRWMutex();

   /**
    * Destructor
    */
   virtual ~CRWMutex();

   /**
    * Locks the mutex on writtings
    */
   void lockWrite();

   /**
    * Locks the mutex on readings
    */
   void lockRead();

   /**
    * Unlocks the mutex on writings
    */
   void unlockWrite();

   /**
    * Unlocks the mutex on readings
    */
   void unlockRead();

private:
   // Methods section

   /**
    * Copying constructor
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is constructing on
    */
   CRWMutex(const CRWMutex& classRef);

   /**
    * Assigning operator
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is assiging from
    */
   CRWMutex& operator=(const CRWMutex& classRef);

   // Members section
#ifndef ANDROID
   pthread_rwlockattr_t    mAttr;
   pthread_rwlock_t        mMutex;
#else
   rwlock                  mMutex;
#endif //ANDROID
};

#endif // CRWMUTEX_HPP
