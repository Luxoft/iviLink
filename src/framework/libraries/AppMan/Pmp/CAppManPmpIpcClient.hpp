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
 

#ifndef CAPPMANPMPIPCCLIENT_HPP_
#define CAPPMANPMPIPCCLIENT_HPP_

#include "IPmpRequest.hpp"
#include "CIpc.hpp"
#include "ICallbackHandler.hpp"
#include "CSignalSemaphore.hpp"
#include "Logger.hpp"

namespace iviLink
{
   namespace AppMan
   {
      class IPmpHandler;

      namespace Ipc
      {

         /**
          * Class implements Ipc
          */
         class CAppManPmpIpcClient  : public IPmpRequest
                                    , public iviLink::Ipc::ICallbackHandler
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
            virtual void OnConnection(iviLink::Ipc::DirectionID dirId);

            /**
             * Callback is called when PMP loses connection with AMP
             */
            virtual void OnConnectionLost(iviLink::Ipc::DirectionID dirId);

            /**
             * Callback is used to handle incoming requests to PMP from AMP
             */
            virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
                  UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize,
                  iviLink::Ipc::DirectionID dirId);


            // stub not used
            virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
                  UInt32 payloadSize,
                  iviLink::Ipc::DirectionID dirId) {}

            /**
             * Request to AMP to start new session with founded by AppMan application
             * @param session is UID of session
             * @param service is UID of service
             */
            virtual CError applicationRequest(iviLink::Service::SessionUid session,
                  iviLink::Service::Uid service);

            /**
             * Generates ids for IPC requests
             */
            iviLink::Ipc::MsgID genId();

            IPmpHandler * mpHandler;      ///< pointer to incoming requests handler
            iviLink::Ipc::CIpc * mpIpc;      ///< pointer to IPC
            iviLink::Ipc::MsgID mId;         ///< id of last IPC request
            CSignalSemaphore mConLostSem; ///< signals in case of connection lost events
            bool mBe;                     ///< is false if needs to exit from thread
            volatile bool mNoConnection;  ///< true if is no connection with AMP

            static Logger msLogger;       ///< object of logger
         };
      }

   }

}

#endif /* CAPPMANPMPIPCCLIENT_HPP_ */
