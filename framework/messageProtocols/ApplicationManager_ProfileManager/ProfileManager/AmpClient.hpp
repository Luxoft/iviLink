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


#ifndef AMPCLIENT_HPP_
#define AMPCLIENT_HPP_

#include "IAmpRequest.hpp"
#include "CIpc.hpp"
#include "ICallbackHandler.hpp"
#include "CSignalSemaphore.hpp"
#include "Logger.hpp"

namespace iviLink
{
   namespace AppMan
   {
      class IAmpHandler;

      namespace Ipc
      {

         /**
          * Class implements Ipc
          */
         class AmpClient  : public iviLink::AppMan::IAmpRequest
                          , public iviLink::Ipc::ICallbackHandler
         {
         public:
            /**
             * Constructor
             */
             AmpClient();

            /**
             * Virtual destructor
             */
            virtual ~AmpClient();

            /**
             * inits handler for requests
             * @param pHandler is pointer to incoming requests handler
             */
            void init(IAmpHandler * pHandler);

            bool connect();

            /**
             * Disconnects with AMP
             */
            void disconnect();

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
                  iviLink::Ipc::DirectionID dirId);

            /**
             * Request to AMP to start new session with founded by AppMan application
             * @param session is UID of session
             * @param service is UID of service
             */
            virtual bool applicationRequest(const iviLink::Service::SessionUid & session,
                  const iviLink::Service::Uid & service);

            virtual bool waitingServiceLoadRequest(const iviLink::Service::Uid & service);

            /**
             * Generates ids for IPC requests
             */
            iviLink::Ipc::MsgID genId();

            IAmpHandler * mHandler;      ///< pointer to incoming requests handler
            iviLink::Ipc::CIpc * const mIpc;      ///< pointer to IPC
            iviLink::Ipc::MsgID mId;         ///< id of last IPC request

            static Logger msLogger;       ///< object of logger
         };
      }

   }

}

#endif /* AMPCLIENT_HPP_ */
