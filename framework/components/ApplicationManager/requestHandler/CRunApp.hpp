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


#ifndef CRUNAPP_HPP_
#define CRUNAPP_HPP_

#include <string>
#include <set>

#include <sys/types.h>

#include "UID.hpp"
#include "Logger.hpp"

namespace iviLink
{

   namespace AMP
   {

      /**
       * Class is used to represent running application (process) in AMP
       */
      class CRunApp
      {
      public:
         /**
          * Constructor
          */
         CRunApp();

         /**
          * Virtual destructor
          */
         virtual ~CRunApp();

         /**
          * Sets id of application in database
          */
         void setId(int id);

         /**
          * Sets pid of running process
          */
         void setPid(pid_t pid);

         /**
          * Returns id of app in database
          */
         int id() const;

         /**
          * returns pid of running app
          */
         pid_t pid() const;

         /**
          * Used to activate/deactivate ability to start service in process
          * @param service is UID of service
          * @param use is true to activate service and false to deactivate it
          */
         void useService(const Service::Uid & service, bool use);

         /**
          * Returns true if app can start this service
          */
         bool hasService(const Service::Uid & service) const;

         /**
          * Sets session UID to process
          */
         void setSession(const Service::SessionUid & session);

         /**
          * Returnes ression of process
          */
         const Service::SessionUid & session() const;

         /**
          * Prints state of object
          */
         void print() const;

         /**
          * Returns set of supported services
          */
         const std::set<Service::Uid> & getServices() const;

         /**
          * Used to add info that app's been registered in AMP
          */
         void registerApp();

         /**
          * If @param send is true it'll be used to make session request to this app
          */
         void sendSessionRequest(bool send);

         /**
          * returnes true if process's been registered in AMP
          */
         bool isRegistred() const;

         /**
          * Retunes true if AMP needs to make session request to this process
          */
         bool needSessionRequest() const;

      private:
         int mAppId; ///< ID of app in database
         pid_t mPid; ///< pid of running app
         std::set<Service::Uid> mServices; ///<set of supported services
         Service::SessionUid mSession; ///< session UID of process
         bool mRegistred; ///< true if process's been registered in AMP
         bool mNeedsSessionRequest; ///< true it it has session request and it'll be send in future

         static Logger msLogger; ///< object of logger
      };

   }

}

#endif /* CRUNAPP_HPP_ */
