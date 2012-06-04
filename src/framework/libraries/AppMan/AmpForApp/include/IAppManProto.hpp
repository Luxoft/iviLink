/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */





#ifndef IAPPMANPROTO_HPP_
#define IAPPMANPROTO_HPP_

#include <list>
#include <sys/types.h>

#include "utils/ipc/include/common.hpp"
#include "utils/misc/include/UID.hpp"
#include "utils/misc/include/CError.hpp"

#include "framework/libraries/AppMan/App/include/IAppMan.hpp"

namespace AXIS
{

   namespace AppMan
   {

      namespace Ipc
      {

         const UInt8 C_PROTO_INIT_APP = 0;
         const UInt8 C_PROTO_USE_SERV = 1;
         const UInt8 C_PROTO_REG_SERV = 2;

         /**
          * Interface of requests from applications to AMP
          */
         class IAppManProto
         {
         public:
            /**
             * Inits application after start
             * @param pid of application
             * @ listOfSupportedServices contains UIDs of services supported by application
             */
            virtual EInitResult initApplication(pid_t pid, std::list<Service::Uid> listOfSupportedServices) = 0;

            /**
             * Says to AMP that app supports/not supports given service
             * @param service is UID of service
             * @param use says support service if true or no if false
             */
            virtual CError useService(pid_t pid, Service::Uid service, bool use = true) = 0;

            /**
             * Registers service in database
             * @param pid of application
             * @param service is UID of service
             */
            virtual CError registerService(pid_t pid, Service::Uid service) = 0;

            /**
             * Virtual destructor
             */
            virtual ~IAppManProto() {}
         };
      }
   }
}

#endif /* IAPPMANPROTO_HPP_ */
