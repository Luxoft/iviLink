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


#ifndef SIGNALSEMAPHOREINTERPROC_HPP
#define SIGNALSEMAPHOREINTERPROC_HPP

#include <pthread.h>
#include <semaphore.h>

#include "Logger.hpp"


/**
 * WARNING: Please, think twice before use this class.
 * The state of posix named semaphore is persistent between launches
 * in case if no one called unlink - it can be when application caught a signal 
 * and crashed. So semaphore can be left in closed or opened state. And next
 * launch of application will use it without recereating. 
 * This is fundamental flaw of posix named semaphores. SysV semaphores are better 
 * in this, but they are not supported on android etc.
 * Probably, this can be fixed using some other means like lock-files 
 * (syncronisation of semaphore creation - pretty stupid).
 */
class CSignalSemaphoreInterproc
{
private:
   static Logger logger;

public:

   /**
    * @param name must be in form "/some_semaphore_name_with_no_slashes_except_the_first_one"
    *    For detailed information see "man 7 sem_overview"
    * @param count is initial value of semaphore
    */
   explicit CSignalSemaphoreInterproc (char const* name, int count = 0);
   
   ~CSignalSemaphoreInterproc ();
   
   void wait ();

   /**
    * @return see sem_trywait()
    */
   int tryWait ();

   /**
    * @param miliSec
    * @return see sem_timedwait()
    */
   int waitTimeout(const time_t miliSec);

   void signal ();
   
   static int unlink(char const* name);

private:
   CSignalSemaphoreInterproc(CSignalSemaphoreInterproc &);
   CSignalSemaphoreInterproc& operator= (CSignalSemaphoreInterproc &);

protected:
   sem_t* mpSem;
   char* mpName;
};

#endif // SIGNALSEMAPHOREINTERPROC_HPP

