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


/********************************************************************
 *
 * System includes
 *
 ********************************************************************/

/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/

/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CCondVar.hpp"

CCondVar::CCondVar()
{
   pthread_mutex_init(&mMutex, 0);
   pthread_cond_init(&mCond, 0);
}

CCondVar::~CCondVar()
{
   pthread_mutex_destroy(&mMutex);
   pthread_cond_destroy(&mCond);
}

void CCondVar::signal()
{
   pthread_cond_signal(&mCond);
}

void CCondVar::broadcast()
{
   pthread_cond_broadcast(&mCond);
}

void CCondVar::wait()
{
   pthread_cond_wait(&mCond, &mMutex);
}

int CCondVar::waitTimeout(const time_t miliSec)
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

   return pthread_cond_timedwait(&mCond, &mMutex, &ts);
}

timespec CCondVar::calcAbsTimeout(const time_t miliSec)
{
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

   return ts;
}

int CCondVar::waitTimeout(timespec const& ts)
{
   return pthread_cond_timedwait(&mCond, &mMutex, &ts);
}

int CCondVar::lock()
{
   return pthread_mutex_lock(&mMutex);
}

int CCondVar::unlock()
{
   return pthread_mutex_unlock(&mMutex);
}
