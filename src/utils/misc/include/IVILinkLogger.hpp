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





#ifndef IVI_LINK_LOGGER_HPP_
#define IVI_LINK_LOGGER_HPP_

#include <string>

namespace IVILinkLog
{
   using namespace std;

   class Logger
   {
   public:
      static Logger& getInstance()
      {
         static Logger log;
         return log;
      }

      static Logger& getInstance(std::string)
      {
         return getInstance();
      }
   };

   #define LOG4CPLUS_TEXT(string)
   #define LOG4CPLUS_TRACE(logger, logEvent)
   #define LOG4CPLUS_TRACE_METHOD(logger, logEvent)
   #define LOG4CPLUS_DEBUG(logger, logEvent)
   #define LOG4CPLUS_INFO(logger, logEvent)
   #define LOG4CPLUS_WARN(logger, logEvent)
   #define LOG4CPLUS_ERROR(logger, logEvent)
   #define LOG4CPLUS_FATAL(logger, logEvent)
}

namespace PropertyConfigurator
{
   void doConfigure();
}

#endif /* IVI_LINK_LOGGER_HPP_ */
