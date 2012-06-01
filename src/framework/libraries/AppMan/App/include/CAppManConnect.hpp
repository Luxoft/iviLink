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

/***************************************************************************
 * Project           AXIS
 * (c) copyright     2012
 * Company           LUXOFT
 * @file             CAppManConnect.hpp
 * @brief            Header file for CAppManConnect
 * @author           VPlachkov
 ***************************************************************************/

#ifndef CAPPMANCONNECT_HPP_
#define CAPPMANCONNECT_HPP_

#include "utils/misc/include/Logger.hpp"
#include "utils/threads/include/CThread.hpp"
#include "framework/libraries/AppMan/App/include/CAppManProtoClient.hpp"

namespace AXIS
{

   namespace AppMan
   {

      namespace Ipc
      {

         /**
          * Class is used to handle connection events of connection with AMP
          */
         class CAppManConnect : public CThread
         {
         public:
            /**
             * Constructor
             * @param pClient is pointer to instance of of IPC protocolclient
             */
            explicit CAppManConnect(CAppManProtoClient * pClient);

            /**
             * Virtual destructor
             */
            virtual ~CAppManConnect();

            /**
             * try to connect with AMP
             */
            void connect();

            /**
             * disconnects with AMP
             */
            void disconnect();

         private:
            /**
             * Main function of connection thread
             */
            virtual void threadFunc();

            CAppManProtoClient * mpClient;   ///< pointer to instance of IPC Protocol client

            static Logger msLogger;  ///< object of logger
         };

      }

   }

}

#endif /* CAPPMANCONNECT_HPP_ */
