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
 

#ifndef IREQHANDLERFORAPPCOM_HPP_
#define IREQHANDLERFORAPPCOM_HPP_

#include <sys/types.h>
#include <string>

namespace iviLink
{
   namespace AMP
   {
      /**
       * Interface of request handler for applications
       */
      class IReqHandlerForAppCom
      {
      public:
         /**
          * Sets launch info to CRunApp object in request handler
          * @param pid of process
          * @param launchInfo is launching information
          */
         virtual void setProcAppLanchInfo(pid_t pid, const std::string &launchInfo) = 0;

         /**
          * Virtual destructor
          */
         virtual ~IReqHandlerForAppCom() {}
      };
   }
}

#endif /* IREQHANDLERFORAPPCOM_HPP_ */
