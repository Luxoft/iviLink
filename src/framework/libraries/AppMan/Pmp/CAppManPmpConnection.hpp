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











#ifndef CAPPMANPMP_HPP_
#define CAPPMANPMP_HPP_

#include "utils/misc/Logger.hpp"
#include "utils/threads/CThread.hpp"
#include "CAppManPmpIpcClient.hpp"

namespace iviLink
{
   namespace AppMan
   {
      namespace Ipc
      {

         /**
          * Class is used to handle IPC connection with AMP
          */
         class CAmpPmpConnection : public CThread
         {
         public:
            /**
             * Constructor
             * @param pClient is pointer to AMP requests handler
             */
            explicit CAmpPmpConnection(CAppManPmpIpcClient * pClient);

            /**
             * Virtual destructor
             */
            virtual ~CAmpPmpConnection();

            /**
             * Begin connection with AMP
             */
            void connect();

            /**
             * Disconnect connection IPC connection with AMP
             */
            void disconnect();

         private:
            /**
             * Main function of PMP connection thread
             */
            virtual void threadFunc();

            CAppManPmpIpcClient * mpClient; ///< pointer to AMP protocol client

            static Logger msLogger; ///< logger object
         };

      }

   }

}

#endif /* CAPPMANPMP_HPP_ */
