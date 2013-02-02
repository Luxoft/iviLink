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


#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <deque>
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "Types.hpp"
#include "CThread.hpp"
#include "CThreadPoolJob.hpp"

#include "Logger.hpp"


class CSignalSemaphore;
class CMutex;

///CThreadPool class
/**
 * CThreadPool's task is to execute jobs in separate threads.
 * Client creates a job and put it to the pool's job queue.
 * Free worker from pool thread catches the job and executes callback.
 */
class CThreadPool
{
private:
   static Logger logger;

public:
   // Methods section

   /**
    * Constructor
    * @param num_threads is the number of worker threads to be spawned
    */
   explicit CThreadPool(UInt32 num_threads = 5);

   /**
    * Destructor
    */
   ~CThreadPool();

   /**
    * Interface function to add a job to the queue.
    * @param job is the description of the job to perform. Object will be copied
    *    in this function.
    */
   void addJob(CThreadPoolJob& job);

private:
   // Types section

   ///CWorkerThread class
   /**
    * The CWorkerThread implements a thread functionality.
    * It's task is to get the job from queue and execute RPC callback
    */
   class CWorkerThread: public CThread
   {
   private:
      static Logger logger;

   public:
      // Methods section
      /**
       * Constructor
       * @param pName threads name passing to CThread constructor
       * @param sem pointer to the signal semaphore that counts job entries and
       *    blocks the worker if there are none
       * @param jobMutex pointer to the mutex that performs exclusive access
       *    to the job's queue from worker threads
       * @param mpJobQueue is the pointer to the queue containing job structs
       */
      CWorkerThread(const char* pName,
            CSignalSemaphore * sem,
            CMutex * jobMutex,
            std::deque<CThreadPoolJob> * mpJobQueue);

      /**
       * Destructor
       */
      ~CWorkerThread();

      /**
       * A threading routine method inherited from CThread class
       */
      virtual void threadFunc();

   private:
      // Members section

      /// Pointer to the signal semaphore that counts job entries
      CSignalSemaphore * mpAvailableJobs;

      /// Pointer to the mutex that performs exclusive access to the job's queue
      /// from worker threads
      CMutex* mpJobMutex;

      /// Pointer to the queue containing job structs
      std::deque<CThreadPoolJob> * mpJobQueue;
      CSignalSemaphore mStartSemaphore;
   };

private:
   // Methods section

   /**
    * Copying constructor
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is constructing on
    */
   CThreadPool(const CThreadPool&);

   /**
    * Assigning operator
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is assiging from
    */
   CThreadPool& operator=(const CThreadPool&);

private:

   // Members section
  
   typedef std::deque<CWorkerThread*> tThreadsQueue;
   tThreadsQueue mThreadsQueue; ///< Queue containing worker threads

   typedef std::deque<CThreadPoolJob> tJobQueue;
   tJobQueue mJobQueue;        ///< Queue containing job structs
   
   CSignalSemaphore mAvailableJobs; ///< Signal semaphore that counts job entries
   CMutex mJobMutex; ///< Mutex that performs exclusive access to the job's queue
};
#endif
