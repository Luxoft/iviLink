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


#ifndef IDATABASE_HPP_
#define IDATABASE_HPP_

#include <string>
#include <list>

#include "UID.hpp"
#include "BaseError.hpp"

namespace iviLink
{
   namespace AMP
   {
      class IDatabase
      {
      public:
         /**
          * @param launchInfo - string with information needed for launch application
          * @return true if AppMan database has current application
          */
         virtual bool hasApp(const std::string & launchInfo) const = 0;

         /**
          * @param launchInfo - string with information needed for launch application
          * @return true application's been added to database
          */
         virtual int addApp(const std::string & launchInfo) = 0;

         /**
          * @param launchInfo - string with information needed for launch application
          * @param services is list of services that app support
          * @return true if success
          */
         virtual int addApp(const std::string & launchInfo, const std::list<Service::Uid> & services) = 0;

         /**
          * @param launchInfo - string with information needed for launch application
          * @return true if success
          */
         virtual bool removeApp(const std::string & launchInfo) = 0;

         /**
          * @param id is key of app in map
          * @return true if success
          */
         virtual bool removeApp(int id) = 0;

         /**
          * @param launchInfo - string with information needed for launch application
          * @param service is UID of service to add
          * @return true if success
          */
         virtual bool addService(const std::string & launchInfo, const Service::Uid & service) = 0;
         virtual bool addService(int id, const Service::Uid & service) = 0;

         /**
          * @param launchInfo - string with information needed for launch application
          * @param service is UID of service to remove
          * @return true if success
          */
         virtual bool removeService(const std::string & launchInfo, const Service::Uid & service) = 0;
         virtual bool removeService(int id, const Service::Uid & service) = 0;

         /**
          * @param launchInfo - string with information needed for launch application
          * @param[out] services is list of found service UIDs
          * @return true if success
          */
         virtual bool getServices(const std::string & launchInfo, std::list<Service::Uid> & services) const = 0;
         virtual bool getServices(int, std::list<Service::Uid> & services) const = 0;

         /**
          * @param service is UID of needed service
          * @param[out] is list of app launch infos of applications, that support service
          * @return true if AppMan database has current application
          */
         virtual void getApps(const Service::Uid & service, std::list<std::string> & applications) const = 0;
         virtual void getApps(const Service::Uid & service, std::list<int> & appIds) const = 0;

         /**
          * @param id is a key of app in map
          * @param service id UID of service
          * @return true if app has this app
          */
         virtual bool hasService(int id, const Service::Uid & service) const = 0;

         /**
          * Returns launch information by id of application in database
          * @param id is id of app in database
          * @param[out] launchInfo is result of function
          */
         virtual void appLaunchInfo(int id, std::string & launchInfo) const = 0;

         /**
          * @param launchInfo of application in database
          * @return id of app in database if success of -1 if error
          */
         virtual int getId(const std::string & launchInfo) const = 0;

         /**
          * virtual destructor
          */
         virtual ~IDatabase () {}
      };
   }
}

#endif /* IDATABASE_HPP_ */
