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


#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "CSignalSemaphore.hpp"

using namespace std;

Logger CSignalSemaphore::logger = Logger::getInstance(LOG4CPLUS_TEXT("utils.threads.CSignalSemaphore"));

void CSignalSemaphore::init ( int count )
{

   while( sem_init( &mSemaphore, 0, count) < 0 )
   {
      LOG4CPLUS_WARN(logger, "init(): sem_init failed: " + convertIntegerToString(errno)
                           + " - " + string(strerror(errno)));
   }
}


void CSignalSemaphore::clear(void)
{
   while( sem_init( &mSemaphore, 0, 0) < 0 )
   {
      LOG4CPLUS_WARN(logger, "clear(): sem_init failed: " + convertIntegerToString(errno)
                           + " - " + string(strerror(errno)));
   }
}


void CSignalSemaphore::signal (void)
{
   sem_post ( &mSemaphore );
}


int CSignalSemaphore::tryWait (void)
{

   if (sem_trywait ( &mSemaphore ) == 0)
   {
      return 0;
   }
   else
   {
      switch (errno)
      {
      case EAGAIN:
            break;
      case EDEADLK:
      case EINTR:
      case EINVAL:
         {
            LOG4CPLUS_WARN(logger, "tryWait(): sem_trywait failed: " + convertIntegerToString(errno)
                                 + " - " + string(strerror(errno)));
            break;
         }
         /* // all other cases must wait again */
      default :
         {
             LOG4CPLUS_WARN(logger, "tryWait(): sem_trywait failed: " + convertIntegerToString(errno)
                                  + " - " + string(strerror(errno)));
             break;
         }
      }
      return -1;
   }
}


void CSignalSemaphore::wait (void)
{

   while (sem_wait ( &mSemaphore ) < 0)
   {
      switch (errno)
      {
         case EDEADLK:
         case ENOSYS:
         case EINVAL:
            {
               LOG4CPLUS_WARN(logger, "wait(): sem_wait failed: " + convertIntegerToString(errno)
                                    + " - " + string(strerror(errno)));
               break;
            }
            // all other cases must wait again
         default :
            {
            }
      }
   }
}

CSignalSemaphore::~CSignalSemaphore (void)
{
   sem_destroy ( &mSemaphore );
}

CSignalSemaphore::CSignalSemaphore ( int count)
{
   CSignalSemaphore::init (count);
}

int CSignalSemaphore::waitTimeout(const time_t miliSec)
{
   const time_t sec = miliSec / 1000;
   const time_t msec = miliSec % 1000;

   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   ts.tv_sec 	+= sec;
   if (1000000000 <= ts.tv_nsec + msec * 1000000)
   {
      ++ts.tv_sec;
      ts.tv_nsec = ts.tv_nsec + msec * 1000000 - 1000000000;
   } else
   {
      ts.tv_nsec	+= msec * 1000000;
   }
   return sem_timedwait(&mSemaphore, &ts);
}
