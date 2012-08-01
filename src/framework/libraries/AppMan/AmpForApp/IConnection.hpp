/* 
 * 
 * iviLINK SDK, version 1.0
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







#ifndef ICONNECTION_HPP_
#define ICONNECTION_HPP_

#include <sys/types.h>

namespace iviLink
{
   namespace AppMan
   {
      namespace Ipc
      {

         /**
          * Interface of connection control functionality
          */
         class IConnection
         {
         public:
            /**
             * Returns true is connection is OK, otherwise false
             */
            virtual bool checkConnection(pid_t pid) = 0;

            /**
             * Virtual destructor
             */
            virtual ~IConnection () {}
         };
      }
   }
}

#endif /* ICONNECTION_HPP_ */
