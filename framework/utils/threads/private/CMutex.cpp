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
#include <cstring>

#include "CMutex.hpp"
#include "Logger.hpp"

namespace {
  Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("utils.threads.CMutex"));
}

CMutex::CMutex()
{
   int err = pthread_mutex_init(&mMutex, NULL);
   if (err)
   {
      LOG4CPLUS_ERROR(logger, "CMutex::CMutex() init err = " + convertIntegerToString(err)
                            + " " + string(strerror(err)));
      /// @todo probably, we need to abort() here
   }
}

CMutex::~CMutex()
{
   int err = pthread_mutex_destroy(&mMutex);
   if (err)
   {
      LOG4CPLUS_ERROR(logger, "~CMutex() destroy err = " + convertIntegerToString(err)
                            + " " + string(strerror(err)));
      /// @todo probably, we need to abort() here
   }
}

void CMutex::lock()
{
   int res = pthread_mutex_lock(&mMutex);
   if (res)
   {
      LOG4CPLUS_ERROR(logger, "lock() ret = " + convertIntegerToString(res)
                            + " " + string(strerror(res)));
      /// @todo probably, we need to abort() here
   }
}

void CMutex::unlock()
{
   int res = pthread_mutex_unlock(&mMutex);
   if (res)
   {
      LOG4CPLUS_ERROR(logger, "unlock() ret = " + convertIntegerToString(res)
                            + " " + string(strerror(res)));
      /// @todo probably, we need to abort() here
   }
}

int CMutex::tryLock()
{
   int res =  pthread_mutex_trylock(&mMutex);
   if (res)
   {
      LOG4CPLUS_ERROR(logger, "tryLock() ret = " + convertIntegerToString(res)
                            + " err " + string(strerror(res)));
   }
   return res;
}

int CMutex::timedLock(const time_t miliSec)
{
#ifndef ANDROID
   const time_t sec = miliSec / 1000;
   const time_t msec = miliSec % 1000;

   struct timespec ts;
   clock_gettime(CLOCK_REALTIME, &ts);
   ts.tv_sec   += sec;
   if (1000000000 <= ts.tv_nsec + msec * 1000000)
   {
      ++ts.tv_sec;
      ts.tv_nsec = ts.tv_nsec + msec * 1000000 - 1000000000;
   } else
   {
      ts.tv_nsec  += msec * 1000000;
   }
   return pthread_mutex_timedlock(&mMutex, &ts);
#else
   unsigned int microSec = miliSec*1000;
   return pthread_mutex_lock_timeout_np(&mMutex, microSec);
#endif //ANDROID
}
