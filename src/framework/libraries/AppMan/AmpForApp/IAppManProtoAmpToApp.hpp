/* 
 * 
 * iviLINK SDK, version 1.1.2
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











#ifndef IAPPMANPROTOAMPTOAPP_HPP_
#define IAPPMANPROTOAMPTOAPP_HPP_

#include <sys/types.h>

#include "utils/misc/UID.hpp"
#include "utils/misc/CError.hpp"

namespace iviLink
{
   namespace AppMan
   {
      namespace Ipc
      {

         const UInt8 C_PROTO_SES_REQ = 0;
         const UInt8 C_PROTO_APP_LAN = 1;
         const UInt8 C_PROTO_CHK_CON = 2;

         /**
          * Interface of requests from AMP to app
          */
         class IAppManProtoAmpToApp
         {
         public:
            /**
             * Sends request to application to start session
             * @param session is UID of session
             * @param service is UID of service
             */
            virtual CError sessionRequest(pid_t pid, iviLink::Service::SessionUid session,
                  iviLink::Service::Uid service) = 0;

            /**
             * Sends request to application to get launch information
             */
            virtual CError getAppLaunchInfo(pid_t pid, std::string & launchInfo) = 0;

            /**
             * Virtual destructor
             */
            virtual ~IAppManProtoAmpToApp() {}
         };
      }
   }
}

#endif /* IAPPMANPROTOAMPTOAPP_HPP_ */
