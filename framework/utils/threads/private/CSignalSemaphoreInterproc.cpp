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


#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>

#include "CSignalSemaphoreInterproc.hpp"

Logger CSignalSemaphoreInterproc::logger = Logger::getInstance(LOG4CPLUS_TEXT("utils.threads.CSignalSemaphoreInterproc"));

CSignalSemaphoreInterproc::CSignalSemaphoreInterproc (char const* name, int count/* = 0*/) :
   mpSem(SEM_FAILED),
   mpName(strdup(name))
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   while ((mpSem = sem_open(mpName, O_CREAT, 00777, count)) == SEM_FAILED)
   {
      int saved_errno = errno;
      LOG4CPLUS_WARN(logger, "CSignalSemaphoreInterproc(): sem_open failed: " + convertIntegerToString(saved_errno)
         + " - " + std::string(strerror(saved_errno)));  
   }
}

CSignalSemaphoreInterproc::~CSignalSemaphoreInterproc (void)
{
   if (sem_close(mpSem) == -1)
   {
      int saved_errno = errno;
      LOG4CPLUS_WARN(logger, "~CSignalSemaphoreInterproc(): sem_close failed: " + convertIntegerToString(saved_errno)
         + " - " + std::string(strerror(saved_errno)));  
   }

   unlink(mpName);

   free(mpName);
}


void CSignalSemaphoreInterproc::wait (void)
{
   while (sem_wait(mpSem) < 0)
   {
      int saved_errno = errno;
      switch (saved_errno)
      {
         case EDEADLK:
         case ENOSYS:
         case EINVAL:
            {
               
               LOG4CPLUS_WARN(logger, "wait(): sem_wait failed: " + convertIntegerToString(saved_errno)
                                    + " - " + std::string(strerror(saved_errno)));
               break;
            }
            // all other cases must wait again
         default :
            {
            }
      }
   }
}

int CSignalSemaphoreInterproc::tryWait (void)
{
   if (sem_trywait(mpSem) == 0)
   {
      return 0;
   }
   else
   {
      int saved_errno = errno;
      switch (saved_errno)
      {
      case EAGAIN:
            break;
      case EDEADLK:
      case EINTR:
      case EINVAL:
         {
            LOG4CPLUS_WARN(logger, "tryWait(): sem_trywait failed: " + convertIntegerToString(saved_errno)
                                 + " - " + std::string(strerror(saved_errno)));
            break;
         }
         /* // all other cases must wait again */
      default :
         {
             LOG4CPLUS_WARN(logger, "tryWait(): sem_trywait failed: " + convertIntegerToString(saved_errno)
                                  + " - " + std::string(strerror(saved_errno)));
             break;
         }
      }
      return -1;
   }
}

int CSignalSemaphoreInterproc::waitTimeout(const time_t miliSec)
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
   return sem_timedwait(mpSem, &ts);
}

void CSignalSemaphoreInterproc::signal (void)
{
   if (sem_post(mpSem) < 0)
   {
      int saved_errno = errno;
      LOG4CPLUS_WARN(logger, "signal(): sem_post failed: " + convertIntegerToString(saved_errno)
                                  + " - " + std::string(strerror(saved_errno)));
   }
}


int CSignalSemaphoreInterproc::unlink(char const* pName)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   int res = sem_unlink(pName);
   if (-1 == res)
   {
      int saved_errno = errno;
      LOG4CPLUS_WARN(logger, "~CSignalSemaphoreInterproc(): sem_unlink failed: " + convertIntegerToString(saved_errno)
         + " - " + std::string(strerror(saved_errno)));  
   }

   return res;
}
