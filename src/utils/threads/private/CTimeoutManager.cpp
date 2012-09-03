/* 
 * 
 * iviLINK SDK, version 1.1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 * 
 */











#include "CTimeoutManager.hpp"
#include <sys/time.h>
#include <cstring>

UInt32 CCallbackThread::sCounter = 0;
char CCallbackThread::sCounterString[24]="CallbackThread0\0\0\0\0\0\0\0\0";
CTimeoutManager * CTimeoutManager::msInstance = 0;

CTimeoutManager * CTimeoutManager::getInstance()
{
   if (0 == CTimeoutManager::msInstance)
   {
      CTimeoutManager::msInstance = new CTimeoutManager();
   }
   return CTimeoutManager::msInstance;
}

/**
 * function itoa isn't in C++ standard, so next two functions (itoa and reverse) were copied from http://en.wikipedia.org/wiki/Itoa
 */
 /* reverse:  reverse string s in place */
 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

 /* itoa:  convert n to characters in s */
 void itoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

CCallbackThread::CCallbackThread(ITimeoutSubscriber * subscriber, UInt32 timeout) :
   CThread(sCounterString), mSubscriber(subscriber), mTimeout(timeout), mThreadEnded(false)
{
   char str[10];
   ++sCounter;
   itoa(sCounter,str);
   sCounterString[14]='\0';
   std::strcat(sCounterString,str);
   start();
}

void CCallbackThread::threadFunc()
{
   sleep(mTimeout);
   mSubscriber->onTimeout();
   mThreadEnded = true;
}

bool CCallbackThread::isEnded() const
{
   return mThreadEnded;
}

CTimeoutManager::CTimeoutManager() :
   CThread("CTimeoutManager") 
{
}

CTimeoutManager::~CTimeoutManager()
{
   mMutex.lock();
   for(std::list<CCallbackThread*>::iterator it = mCallbackThreads.begin(); mCallbackThreads.end() != it; ++it)
   {
      delete *it;
   }
   mCallbackThreads.clear();
   mMutex.unlock();
}

void CTimeoutManager::addSubscriber(ITimeoutSubscriber * subscriber, UInt32 timeout)
{
   mMutex.lock();
   mCallbackThreads.push_back(new CCallbackThread(subscriber,timeout));
   if(mCallbackThreads.size() == 1)
   {
      start();
   }
   mMutex.unlock();
}

void CTimeoutManager::threadFunc()
{
   for(;;)
   {
      mMutex.lock();
      cleanEndedThreads();
      bool needBreak = mCallbackThreads.empty();
      mMutex.unlock();
      if (needBreak)
      {
         break;
      }
      sleep(1000);
   }
}

void CTimeoutManager::cleanEndedThreads()
{
   for(std::list<CCallbackThread*>::iterator it = mCallbackThreads.begin(); mCallbackThreads.end() != it; ++it)
   {
      if((*it)->isEnded())
      {
         std::list<CCallbackThread*>::iterator delIt = it;
         --it;
         delete (*delIt);
         mCallbackThreads.erase(delIt);
      }
   }
}

