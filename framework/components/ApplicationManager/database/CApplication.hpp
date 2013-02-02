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


#ifndef CAPPLICATION_HPP_
#define CAPPLICATION_HPP_

#include <string>
#include <list>

#include "Logger.hpp"
#include "UID.hpp"

namespace iviLink
{
   namespace AMP
   {

      /**
       * Class is used to represent application in AppMan database
       */
      class CApplication
      {
      public:
         /**
          * Constructor of CApplication
          */
         CApplication();

         /**
          * Constructor of CApplication with launch info parameter
          */
         CApplication(const std::string & launchInfo);

         /**
          * Constructor of CApplication with launch info and list of services
          */
         CApplication(const std::string & launchInfo, const std::list<Service::Uid> & services);

         /**
          * Destructor of CApplication
          */
         virtual ~CApplication();

         /**
          * Function returns launch information of application
          */
         const std::string & launchInfo() const;

         /**
          * Sets launch information
          */
         void setLaunchInfo(std::string & launchInfo);

         /**
          * Function returns list of services
          */
         const std::list<Service::Uid> services() const;

         /**
          * Function returns true if application has given service
          * @param service is UID of service
          */
         bool hasService(Service::Uid service) const;

         /**
          * Function adds service to list of supported services by application
          * @param service is UID of service
          * @return true if success
          */
         bool addService(Service::Uid service);

         /**
          * Function removes service from list of supported services by application
          * @param service is UID of service
          * @return true if success
          */
         bool removeService(Service::Uid service);

      private:
         std::string mLaunchInfo; ///< application launch information
         std::list<Service::Uid> mServices; ///< list of supported services by application

         static Logger msLogger; ///< object of logger
      };

   }

}

#endif /* CAPPLICATION_HPP_ */
