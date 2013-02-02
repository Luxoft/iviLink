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


#include "CApplication.hpp"

namespace iviLink
{

   namespace AMP
   {
      Logger CApplication::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Database.CApplication"));

      CApplication::CApplication()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      CApplication::CApplication(const std::string & launchInfo)
         : mLaunchInfo(launchInfo)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      CApplication::CApplication(const std::string & launchInfo, const std::list<Service::Uid> & services)
         : mLaunchInfo(launchInfo)
         , mServices(services)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mServices.unique();
      }

      CApplication::~CApplication()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
      }

      const std::string & CApplication::launchInfo() const
      {
         return mLaunchInfo;
      }

      void CApplication::setLaunchInfo(std::string & launchInfo)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mLaunchInfo = launchInfo;
      }

      const std::list<Service::Uid> CApplication::services() const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         return mServices;
      }

      bool CApplication::hasService(Service::Uid service) const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         for (std::list<Service::Uid>::const_iterator it = mServices.begin();
               mServices.end() != it; ++it)
         {
            if ((*it) == service)
            {
               return true;
            }
         }
         return false;
      }

      bool CApplication::addService(Service::Uid service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (hasService(service))
         {
            return false;
         }
         mServices.push_back(service);
         return true;
      }

      bool CApplication::removeService(Service::Uid service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::list<Service::Uid>::iterator it = mServices.begin();
         for ( ; mServices.end() != it; ++it)
         {
            if ((*it) == service)
            {
               break;
            }
         }

         if (it == mServices.end())
         {
            return false;
         }
         mServices.erase(it);
         return true;
      }

   }

}
