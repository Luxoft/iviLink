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
#include "CRWMutex.hpp"


#ifndef ANDROID
#else

void reader_lock(struct rwlock *self) 
{
   pthread_mutex_lock(&self->lock);
   if (self->writers || self->write_waiters) 
   {
      self->read_waiters++;
      do 
      {
         pthread_cond_wait(&self->read, &self->lock);
      }
      while (self->writers || self->write_waiters);
      self->read_waiters--;
   }
   self->readers++;
   pthread_mutex_unlock(&self->lock);
}

void reader_unlock(struct rwlock *self) 
{
   pthread_mutex_lock(&self->lock);
   self->readers--;
   if (self->write_waiters)
      pthread_cond_signal(&self->write);
   pthread_mutex_unlock(&self->lock);
}

void writer_lock(struct rwlock *self) 
{
   pthread_mutex_lock(&self->lock);
   if (self->readers || self->writers) 
   {
      self->write_waiters++;
      do 
      {
         pthread_cond_wait(&self->write, &self->lock);
      }
      while (self->readers || self->writers);
      self->write_waiters--;
   }
   self->writers = 1;
   pthread_mutex_unlock(&self->lock);
}

void writer_unlock(struct rwlock *self) 
{
   pthread_mutex_lock(&self->lock);
   self->writers = 0;
   if (self->write_waiters)
      pthread_cond_signal(&self->write);
   else if (self->read_waiters)
      pthread_cond_broadcast(&self->read);
   pthread_mutex_unlock(&self->lock);
}

void rwlock_init(struct rwlock *self) 
{
   self->readers = self->writers = self->read_waiters = self->write_waiters = 0;
   pthread_mutex_init(&self->lock, NULL);
   pthread_cond_init(&self->read, NULL);
   pthread_cond_init(&self->write, NULL);
}

void rwlock_deinit(struct rwlock *self) 
{
   pthread_cond_destroy(&self->write);
   pthread_cond_destroy(&self->read);
   pthread_mutex_destroy(&self->lock);
}

#endif //ANDROID


CRWMutex::CRWMutex()
{
#ifndef ANDROID
   pthread_rwlockattr_init(&mAttr);
   pthread_rwlock_init(&mMutex, &mAttr);
#else
   rwlock_init(&mMutex);
#endif //ANDROID
}

CRWMutex::~CRWMutex()
{
#ifndef ANDROID
   pthread_rwlock_destroy(&mMutex);
   pthread_rwlockattr_destroy(&mAttr);
#else
   rwlock_deinit(&mMutex);
#endif //ANDROID
}

void CRWMutex::lockWrite()
{
#ifndef ANDROID
   pthread_rwlock_wrlock(&mMutex);
#else
   writer_lock(&mMutex);
#endif //ANDROID
}

void CRWMutex::lockRead()
{
#ifndef ANDROID
   pthread_rwlock_rdlock(&mMutex);
#else
   reader_lock(&mMutex);
#endif //ANDROID
}

void CRWMutex::unlockWrite()
{
#ifndef ANDROID
   pthread_rwlock_unlock(&mMutex);
#else
   writer_unlock(&mMutex);
#endif //ANDROID
}

void CRWMutex::unlockRead()
{
#ifndef ANDROID
   pthread_rwlock_unlock(&mMutex);   
#else
   reader_unlock(&mMutex);
#endif //ANDROID
}
