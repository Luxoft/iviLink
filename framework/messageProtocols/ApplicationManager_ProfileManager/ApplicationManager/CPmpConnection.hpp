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


#ifndef CPMPCONNECTION_HPP_
#define CPMPCONNECTION_HPP_

#include "Logger.hpp"
#include "CThread.hpp"
#include "CAppManPmpIpcServer.hpp"

namespace iviLink
{

   namespace AppMan
   {

      namespace Ipc
      {

         /**
          * Class is used to controll connection with PMP
          */
         class CPmpConnection : public CThread
         {
         public:
            /**
             * Constructor
             * @param pServer is handler of requests from PMP
             */
            explicit CPmpConnection(CAppManPmpIpcServer * pServer);

            /**
             * Virtual destructor
             */
            virtual ~CPmpConnection();

            /**
             * Begins connection with PMP
             */
            void connect();

            /**
             * disconnects from PMP
             */
            void disconnect();

         private:
            /**
             * This's main function of thread
             */
            virtual void threadFunc();

            CAppManPmpIpcServer * mpServer;  ///< mpServer is pointer to handler of PMP requests

            static Logger msLogger;          ///< object of logger
         };

      }

   }

}

#endif /* CPMPCONNECTION_HPP_ */
