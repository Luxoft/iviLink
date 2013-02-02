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


#ifndef IAPPCOMTHREAD_HPP_
#define IAPPCOMTHREAD_HPP_

#include "UID.hpp"

namespace iviLink
{
   namespace AMP
   {
      /*
       * It's an interface of thread with requests to PMP and applications.
       * All requests are handled in special thread, this functions are non-blocking
       */
      class IAppComThread
      {
      public:
         /*
          * Request to PMP to say about session problems (error while session is starting)
          * @param session is UID of session
          */
         virtual void addRequest(iviLink::Service::SessionUid session) = 0;

         /*
          * Request to app to start new session
          * @param pid is pid of app process
          * @param session is UID of session
          * @param service is UID of service
          */
         virtual void addSessionRequest(pid_t pid, const iviLink::Service::SessionUid & session,
               const iviLink::Service::Uid & service) = 0;

         virtual void addWaitingServiceRequest(pid_t pid, const iviLink::Service::Uid & service) = 0;

         /*
          * Makes request to app to get launch info
          * pid is pid of app process
          */
         virtual void addAppLaunchInfoRequest(pid_t pid) = 0;

         virtual ~IAppComThread() {}
      };
   }
}


#endif /* IAPPCOMTHREAD_HPP_ */
