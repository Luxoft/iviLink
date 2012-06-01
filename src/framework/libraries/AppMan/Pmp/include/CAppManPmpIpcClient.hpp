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
 * @file             CAppManPmpIpcClient.hpp
 * @brief            Header file for AppMan PMP-client
 * @author           VPlachkov
 ***************************************************************************/

#ifndef CAPPMANPMPIPCCLIENT_HPP_
#define CAPPMANPMPIPCCLIENT_HPP_

#include "framework/libraries/AppMan/Pmp/include/IPmpHandler.hpp"
#include "framework/libraries/AppMan/Pmp/include/IPmpRequest.hpp"
#include "utils/ipc/include/CIpc.hpp"
#include "utils/ipc/include/ICallbackHandler.hpp"
#include "utils/threads/include/CSignalSemaphore.hpp"
#include "utils/misc/include/Logger.hpp"

namespace AXIS
{
   namespace AppMan
   {
      namespace Ipc
      {

         /**
          * Class implements Ipc
          */
         class CAppManPmpIpcClient  : public IPmpRequest
                                    , public AXIS::Ipc::ICallbackHandler
         {
         public:
            /**
             * Constructor
             */
            CAppManPmpIpcClient();

            /**
             * Virtual destructor
             */
            virtual ~CAppManPmpIpcClient();

            /**
             * inits handler for requests
             * @param pHandler is pointer to incoming requests handler
             */
            void init(IPmpHandler * pHandler);

            /**
             * Main loop of IPC client
             */
            void loop();

            /**
             * Disconnects with AMP
             */
            void disconnect();

            /**
             * Returnes true if connection with AMP is OK
             */
            bool checkConnection() const;

         private:
            /**
             * Callback is used when connection starts
             */
            virtual void OnConnection(AXIS::Ipc::DirectionID dirId);

            /**
             * Callback is called when PMP loses connection with AMP
             */
            virtual void OnConnectionLost(AXIS::Ipc::DirectionID dirId);

            /**
             * Callback is used to handle incoming requests to PMP from AMP
             */
            virtual void OnRequest(AXIS::Ipc::MsgID id, UInt8 const* pPayload,
                  UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize,
                  AXIS::Ipc::DirectionID dirId);

            /**
             * Request to AMP to start new session with founded by AppMan application
             * @param session is UID of session
             * @param service is UID of service
             */
            virtual CError applicationRequest(AXIS::Service::SessionUid session,
                  AXIS::Service::Uid service);

            /**
             * Generates ids for IPC requests
             */
            AXIS::Ipc::MsgID genId();

            IPmpHandler * mpHandler;      ///< pointer to incoming requests handler
            AXIS::Ipc::CIpc * mpIpc;      ///< pointer to IPC
            AXIS::Ipc::MsgID mId;         ///< id of last IPC request
            CSignalSemaphore mConLostSem; ///< signals in case of connection lost events
            bool mBe;                     ///< is false if needs to exit from thread
            volatile bool mNoConnection;  ///< true if is no connection with AMP

            static Logger msLogger;       ///< object of logger
         };
      }

   }

}

#endif /* CAPPMANPMPIPCCLIENT_HPP_ */
