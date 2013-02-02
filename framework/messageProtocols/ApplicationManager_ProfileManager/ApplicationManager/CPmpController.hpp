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


#ifndef CPMPCONTROLLER_HPP_
#define CPMPCONTROLLER_HPP_

#include "Logger.hpp"
#include "IPmpHandler.hpp"
#include "IPmpRequest.hpp"
#include "CPmpConnection.hpp"
#include "IPmpConnectionStatus.hpp"

namespace iviLink
{

   namespace AppMan
   {

      namespace Ipc
      {
         class CAppManPmpIpcServer;

         /**
          * Controller of IPC communication with PMP
          */
         class CPmpController
         {
         public:
            /**
             * Returns instance of controller
             */
            static CPmpController * instance();

            /**
             * Returns instance of class that implements IPC protocol with PMP
             */
            IPmpHandler * pmpHandler();

            /**
             * Inits IPC handlers of IPC communication
             * @param pRequest is pointer to handler of incoming requests
             * @param pConnectionStatus is pointer to handler of connection status
             */
            void init(IPmpRequest * pRequest, IPmpConnectionStatus * pConnectionStatus);

            /**
             * Uninits IPC communication with PMP
             */
            void uninit();

            /**
             * Returns true if connection with PMP is OK
             */
            bool checkConnection () const;

         private:
            /**
             * Private constructor because of it's singleton object
             */
            CPmpController();

            /**
             * Virtual destructor
             */
            virtual ~CPmpController();

            /// Not implemented
            CPmpController(const CPmpController &);
            CPmpController & operator = (const CPmpController &);

            CAppManPmpIpcServer * mpServer;  ///< Pointer to handler of incoming requests
            CPmpConnection * mpConnection;   ///< Pointer to handler of connection status

            static Logger msLogger;          ///< logger object
         };

      }

   }

}

#endif /* CPMPCONTROLLER_HPP_ */
