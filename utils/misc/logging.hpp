/* 
 * 
 * iviLINK SDK, version 1.0.1
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









#ifndef LOGGING_HPP_
#define LOGGING_HPP_

#include <string>

#define LOGGING_ENABLED

#if !defined(ANDROID_LOG_TAG)
   #define ANDROID_LOG_TAG "AXIS"
#endif

void Log_SetPrefix(char const* const pPrefix);

#if defined(LOGGING_ENABLED) && !defined(NO_LOG)

   #if defined(ANDROID)
      void _Log(const char* tag, const char* format, ...);
      void _hex_print(const char* tag, const void* arr, unsigned int size);

      #define Log(...)       _Log(ANDROID_LOG_TAG, __VA_ARGS__)
      #define hex_print(...) _hex_print(ANDROID_LOG_TAG, __VA_ARGS__)
   #else
      void _Log(const char* format, ...);
      void _hex_print(const void* arr, unsigned int size);

      #define Log(...) _Log(__VA_ARGS__)
      #define hex_print(...) _hex_print(__VA_ARGS__)
   #endif

   class __CTraceHelper
   {
      std::string mFunc;
   public:
      __CTraceHelper(char const* func) :
         mFunc(func)
      {
         Log("%s", func);
      }

      __CTraceHelper(char const* file, int line, char const* func) :
         mFunc(func)
      {
         Log("%s:%d %s", file, line, func);
      }

      ~__CTraceHelper()
      {
         Log("%s done", mFunc.c_str());
      }
   };

   #define TRACE() __CTraceHelper __traceObj(__PRETTY_FUNCTION__)
   #define TRACEFL() __CTraceHelper __traceObj(__FILE__, __LINE__, __PRETTY_FUNCTION__)

//#define Log(s,...)   fprintf(stderr,"#msg %s <%s:%u> {%ju}:\t" s "\n", __func__,__FILE__,__LINE__,pthread_self(), ##__VA_ARGS__)


#else
   #define Log(...)
   #define hex_print(...)
   #define TRACE()
#endif


#endif /* LOGGING_HPP_ */
