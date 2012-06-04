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





#include <assert.h>
#include <cstdarg>
#include <ctime>
#include <cstdio>
#include <pthread.h>
#include <stdint.h>

#include "utils/misc/include/Types.hpp"
#include "utils/misc/include/logging.hpp"
#include <unistd.h>

#if defined(ANDROID)
   #include <sys/syscall.h>
   #include <android/log.h>
#else
   #include <syscall.h>
#endif

static const char gDefaultPrefix[] = "";
static char const* gpPrefix = gDefaultPrefix;

void Log_SetPrefix(char const* const pPrefix)
{
   if (pPrefix)
      gpPrefix = pPrefix;
   else
      gpPrefix = gDefaultPrefix;
}

#if defined(ANDROID)
void _Log(const char* tag, const char* format, ...)
#else
void _Log(const char* format, ...)
#endif
{
   time_t curTime;
   tm tmCurTime;
   const size_t bufSize = 1024;
   char buf[bufSize] = "";
   size_t size = 0;

   size += snprintf(buf + size, bufSize - size, "%s", gpPrefix);

#if !defined(ANDROID)
   struct timespec acttime;

   clock_gettime(CLOCK_REALTIME, &acttime);

   if (time(&curTime) != (time_t) (-1))
   {
      if (localtime_r(&curTime, &tmCurTime) != NULL)
      {
         size += strftime(buf + size, bufSize - size, "[%T", &tmCurTime);
         //printf("[%s] ", buf);
      }
   }
   size += snprintf(buf + size, bufSize - size, ":%03ld]",
            acttime.tv_nsec / 1000000L);
#endif

#if defined(ANDROID)
   size += snprintf(buf + size, bufSize - size, "{TID %lu}", gettid());
#else
   size += snprintf(buf + size, bufSize - size, "{TID %lu:%i}", pthread_self(), getpid());
#endif // ANDROID

   va_list ap;
   va_start(ap, format);
   vsnprintf(buf + size, bufSize - size, format, ap);
   va_end(ap);

#if defined(ANDROID)
   __android_log_write(ANDROID_LOG_DEBUG, tag, buf);
#else
   fprintf(stderr, "%s\n", buf);
   fflush(stderr);
#endif
}

#if defined(ANDROID)
void _hex_print(const char* tag, const void* arr, unsigned int size)
#else
void _hex_print(const void* arr, unsigned int size)
#endif
{
   const UInt8* a = (const UInt8*) arr;
   const size_t bufSize = 1024;
   //assert(size * 8 <= bufSize);
   if (size * 8 >= bufSize)
      return;
   char buf[bufSize] = "";

   size_t cnt;
   size_t buf_c = 0;
   size_t ln_beg;

   for (ln_beg = cnt = 0; cnt < size; cnt += 1)
   {
      char b = (a[cnt] & 0xf0) >> 4;
      buf[buf_c++] = b > 9 ? 'a' + b - 10 : '0' + b;
      b = a[cnt] & 0x0f;
      buf[buf_c++] = b > 9 ? 'a' + b - 10 : '0' + b;

      buf[buf_c++] = ' ';
      if ((cnt & 0x03) == 0x03)
         buf[buf_c++] = ' ';

      if ((cnt & 0x07) == 0x07 || cnt + 1 == size)
      {
         buf[buf_c++] = ' ';
         for (; ln_beg <= cnt; ln_beg++)
         {
            buf[buf_c++] = a[ln_beg] < 32 ? '.' : a[ln_beg];
         }
         ln_beg = cnt + 1;

         buf[buf_c++] = '\n';
      }
   }
   buf[buf_c++] = 0;

#if defined(ANDROID)
   __android_log_write(ANDROID_LOG_DEBUG, tag, buf);
#else
   fwrite(buf, 1, buf_c, stderr);
   fflush(stderr);
#endif
}

