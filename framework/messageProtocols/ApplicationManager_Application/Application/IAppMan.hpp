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


#ifndef IAPPMAN_HPP_
#define IAPPMAN_HPP_

#include <list>

#include "UID.hpp"
#include "BaseError.hpp"
#include <tr1/functional>

namespace iviLink
{
   namespace AppMan
   {

      /**
       * This enum is used in initApplication function to get result of initialization
       */
      enum EInitResult
      {
         STARTED_BY_APPMAN,   ///< app started by Application Manager
         STARTED_BY_USER,     ///< app started by user
         ERROR_IPC,           ///< IPC connection error with AMP
         ERROR_NOT_INITED,    ///< IPC is not inited
         ERROR_AMP            ///< Error in AMP
      };

       typedef std::tr1::function<void ()> notify_t;

      /**
       * AppMan Interface for Applications
       */
      class IAppMan
      {
      public:

         /**
          * Inits application in AppMan on startup
          * @retval STARTED_BY_APP if app's been started by AppMan
          * @retval STARTED_BY_USER if app hasn't been stated by AppMan
          * @retval ERROR_IPC in case of IPC error in communication with AppMan Process
          * @retval ERROR_NOT_INITED if IPC client hasn't been inited
          * @retval ERROR_AMP in case of AppMan process errors
          */
         virtual iviLink::AppMan::EInitResult initApplication(std::list<Service::Uid> listOfSupportedServices) = 0;

         /**
          * enables/disables service while application is running
          */
         virtual BaseError useService(Service::Uid service, bool use = true) = 0;

         /**
          * Registers service for application in ApplicationManager
          */
         virtual BaseError registerService(Service::Uid service) = 0;

          virtual bool isLinkAlive() = 0;
          virtual void setLinkCallbacks( notify_t link_up_, notify_t link_down_ ) =0;

         /**
          * Virtual destructor
          */
         virtual ~IAppMan() {}
      };
   }
}

#endif /* IAPPMAN_HPP_ */
