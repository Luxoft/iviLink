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
 

#ifndef CAPPMANPMPCONTROLLER_HPP_
#define CAPPMANPMPCONTROLLER_HPP_

#include "Logger.hpp"
#include "CAppManPmpConnection.hpp"

namespace iviLink
{

   namespace AppMan
   {

      class IPmpHandler;
      class IPmpRequest;

      namespace Ipc
      {
         class CAppManPmpIpcClient;

         /**
          * Controller of connection with AMP
          */
         class CAppManPmpController
         {
         public:
            /**
             * Returns instance of controller
             */
            static CAppManPmpController * instance();

            /**
             * Returns instance of IPC protocol object
             */
            IPmpRequest * pmpRequest();

            /**
             * Inits handler of incoming requests
             * @param pHandler is pointer to handler of incoming requests
             */
            void init(IPmpHandler * pHandler);

            /**
             * Uninits controller
             */
            void uninit();

            /**
             * Returnes true if connection with AMP is OK
             */
            bool checkConnection () const;

         private:
            /**
             * Private constructor because it's a singleton object
             */
            CAppManPmpController();

            /**
             * Virtual destructor
             */
            virtual ~CAppManPmpController();

            /// Not implemented
            CAppManPmpController(const CAppManPmpController &);
            CAppManPmpController & operator = (const CAppManPmpController &);

            CAppManPmpIpcClient * mpClient;           ///< pointer to AppMan PMP client
            CAmpPmpConnection * mpConnection;         ///< pointer to connection thread

            static Logger msLogger;                   ///< object of logger
            static CAppManPmpController * mspInstance;///< pointer to instance of CAppManPmpController
         };

      }

   }

}

#endif /* CAPPMANPMPCONTROLLER_HPP_ */
