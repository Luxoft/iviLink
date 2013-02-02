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


#ifndef IAMPREQUEST_HPP_
#define IAMPREQUEST_HPP_

#include "UID.hpp"

namespace iviLink
{
   namespace AppMan
   {

      /**
       * Interface for PMP to communicate with Application Manager
       */
      class IAmpRequest
      {
      public:
         /**
          * Request to begin Service session (start app or find stated app and make request to start
          * service
          */
         virtual bool applicationRequest(const iviLink::Service::SessionUid & session,
                 const iviLink::Service::Uid & service) = 0;
         virtual bool waitingServiceLoadRequest(const iviLink::Service::Uid & service) = 0;

      protected:
         /**
          * Virtual destructor
          */
         virtual ~IAmpRequest() { }
      };

   }
}
#endif /* IPMPREQUEST_HPP_ */
