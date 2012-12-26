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
 

#include "Logger.hpp"
#include "CLogger.hpp"

namespace iviLink
{
   namespace App
   {

      CLogger::CLogger()
         : mpLogger(0)
      {
         PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
         mpLogger = new Logger(Logger::getInstance(LOG4CPLUS_TEXT("appLib")));
      }

      CLogger::~CLogger()
      {
         delete mpLogger;
      }

      Logger * CLogger::logger()
      {
         static CLogger sInstance;
         return sInstance.mpLogger;
      }
   }
}
