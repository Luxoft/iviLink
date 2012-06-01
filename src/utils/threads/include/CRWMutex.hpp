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

#ifndef CRWMUTEX_HPP
#define CRWMUTEX_HPP
/***************************************************************************
 * Project           AXIS
 * (c) copyright     2011
 * Company           LUXOFT
 * @file             CRWMutex.hpp
 * @brief            The CRWMutex class header file
 * @author           KSkidanov
 ***************************************************************************/
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

#if defined(ANDROID)

#include "CMutex.hpp"

typedef CMutex CRWMutex;

#else

 
 
 
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
    * Unlocks the mutex
    */
   void unlock();

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
   pthread_rwlockattr_t    mAttr;
   pthread_rwlock_t        mMutex;
};

#endif // ANDROID

#endif // CRWMUTEX_HPP
