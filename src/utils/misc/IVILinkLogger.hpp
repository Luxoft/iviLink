/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#ifndef IVI_LINK_LOGGER_HPP_
#define IVI_LINK_LOGGER_HPP_

#ifndef ANDROID
#else

#include <string>
#include <sstream>
#include <map>
#include <android/log.h>
#include "Types.hpp"

#define LOG4CPLUS_TEXT(string) string

#ifndef LATRACE 
#define LOG4CPLUS_TRACE(logger, logEvent) 
#define LOG4CPLUS_TRACE_METHOD(logger, logEvent) 
#else
#define LOG4CPLUS_TRACE(logger, logEvent) Logger::trace(logger, logEvent)
#define LOG4CPLUS_TRACE_METHOD(logger,logEvent) Tracer tracer(logger, logEvent);
#endif //LATRACE

#ifndef LADEBUG
#define LOG4CPLUS_DEBUG(logger, logEvent) 
#else
#define LOG4CPLUS_DEBUG(logger, logEvent)  Logger::debug(logger, logEvent)
#endif //LADEBUG

#define LOG4CPLUS_INFO(logger, logEvent) Logger::info(logger, logEvent)
#define LOG4CPLUS_WARN(logger, logEvent) Logger::warn(logger,logEvent)
#define LOG4CPLUS_ERROR(logger, logEvent) Logger::error(logger, logEvent)
#define LOG4CPLUS_FATAL(logger, logEvent) Logger::fatal(logger, logEvent)

#undef assert
// actual assert that will print error and crash process - standard assert just causes crash and no print whatsoever
// #define assert(e) ((e) ? (void)0 : __android_log_assert(0,"iviLink.assert","%s(%s:%d) >> %s ",__func__ ,__FILE__, __LINE__, #e))
// just print of error
#define assert(e) ((e) ? (void)0 : (void)__android_log_print(ANDROID_LOG_ERROR,"iviLink ASSERT","Assertion failed: %s(%s:%d) >> %s ",__func__ ,__FILE__, __LINE__, #e))

namespace IVILinkLog
{
   std::string convertIntegerToString(UInt32 intVal);
   
   class Logger
   {
   public:
   
   
      std::string tag;
      
      static Logger getInstance()
      {
         return getInstance(std::string("default"));
      }
      
      static Logger getInstance(std::string str)
      {
         Logger log(str);
         return log;
      }
      
      
      /*static wrappers*/
      static void trace(Logger log, std::string logEvent)
      {
         log.trace(logEvent);
      }
   
      static void trace_method(Logger log, std::string logEvent)
      {
         log.trace(logEvent);
      }
   
      static void debug(Logger log, std::string logEvent)
      {
         log.debug(logEvent);
      }
      
      static void info(Logger log, std::string logEvent)
      {
         log.info(logEvent);
      }
      
      static void warn(Logger log, std::string logEvent)
      {
         log.warn(logEvent);
      }
      
      static void error(Logger log, std::string logEvent)
      {
         log.error(logEvent);
      }
      
      static void fatal(Logger log, std::string logEvent)
      {
         log.fatal(logEvent);
      }

      /*member methods*/
      void trace(std::string logEvent)
      {
         __android_log_print(ANDROID_LOG_VERBOSE, tag.c_str(), logEvent.c_str()); 
      }
   
      void debug(std::string logEvent)
      {
         __android_log_print(ANDROID_LOG_DEBUG, tag.c_str(), logEvent.c_str());
      }
      
      void info(std::string logEvent)
      {
         __android_log_print(ANDROID_LOG_INFO, tag.c_str(), logEvent.c_str());
      }
      
      void warn(std::string logEvent)
      {
         __android_log_print(ANDROID_LOG_WARN, tag.c_str(), logEvent.c_str());
      }
      
      void error(std::string logEvent)
      {
         __android_log_print(ANDROID_LOG_ERROR, tag.c_str(), logEvent.c_str());
      }
      
      void fatal(std::string logEvent)
      {
         std::string both=std::string("FATAL: ")+logEvent;
         error(both);
      }
        
   private:
      Logger(std::string tagStr)
      {
         tag=tagStr;
      }
   };
   
   
   class Tracer
   {
      friend class Logger; 
      
      std::string mEvent;
      Logger mLogger;
      public:
         Tracer(Logger logger, std::string logEvent)
         :  mLogger(Logger::getInstance(logger.tag))
         ,  mEvent(logEvent)
         {
            std::string msg = std::string("ENTER: ")+mEvent;
            Logger::trace(mLogger, msg);   
         }  
         
         ~Tracer()
         {
            std::string msg = std::string("EXIT: ")+mEvent;
            Logger::trace(mLogger, msg);   
         }
   
   }; // Tracer
}

namespace PropertyConfigurator
{
   void doConfigure();
   void doConfigure(const char * properties);
}

#endif //ANDROID
#endif /* IVI_LINK_LOGGER_HPP_ */
