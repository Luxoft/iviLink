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


#ifndef CAPPMANPROTOCLIENT_HPP_
#define CAPPMANPROTOCLIENT_HPP_

#include "IAppManProto.hpp"
#include "IAppManProtoAmpToApp.hpp"
#include "Logger.hpp"
#include "CIpc.hpp"
#include "ICallbackHandler.hpp"
#include "CSignalSemaphore.hpp"

#include <cassert>

namespace iviLink
{

   namespace AppMan
   {

      namespace Ipc
      {

         /**
          * Class implements client side of IPC library
          */
         class CAppManProtoClient   : public IAppManProto
                                    , public iviLink::Ipc::ICallbackHandler
         {
            struct tSessionRequestInfo
            {
               pid_t mPid;
               Service::SessionUid mSession;
               Service::Uid mUid;
               tSessionRequestInfo(pid_t pid, Service::SessionUid session, Service::Uid service)
               : mPid(pid)
               , mSession(session)
               , mUid(service)
               {}
            };
            typedef std::list<tSessionRequestInfo> tSessionRequestList;
         public:
            /**
             * Constructor
             */
            CAppManProtoClient();

            /**
             * Virtual destructor
             */
            virtual ~CAppManProtoClient();

            /**
             * Inits handler of requests from AMP
             */
             void init(IAppManProtoAmpToApp * pHandler);

            /**
             * main loop of IPC client thread
             */
            void loop(CSignalSemaphore * pStartSemaphore);

            /**
             * Disconnects IPC
             */
            void disconnect();

            /**
             * Returns true if connection is OK
             */
            bool checkConnection() const;

            /**
             * Is used only in CAppManProtoClient. If sesssionRequest() was made
             * before initDone(), waits initDone
             * and after it will be called to proceed session request
             */
            virtual void checkSessionRequest();

         private:
            /**
             * Callback is called in case of new connection
             */
            virtual void OnConnection(iviLink::Ipc::DirectionID dirId);

            /**
             * Callback is callse in case of connection losing
             */
            virtual void OnConnectionLost(iviLink::Ipc::DirectionID dirId);

            /**
             * Callback is called in case of incoming messages
             */
            virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
                  UInt32 payloadSize, UInt8* const pResponseBuffer,
                  UInt32& bufferSize, iviLink::Ipc::DirectionID dirId);

            virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
                  UInt32 payloadSize,  iviLink::Ipc::DirectionID dirId);

             // connectivity test
             bool isLinkAlive();
            /**
             * Makes request to AMP to initApplication
             * @param pid of application
             * @param listOfSupportesServices contaions list of UIDs that app can start at moment
             */
            virtual EInitResult initApplication(pid_t pid, std::list<Service::Uid> listOfSupportedServices);

            /**
             * Maket request to AMP to sau use or no given service in application
             */
            virtual BaseError useService(pid_t pid, Service::Uid service, bool use = true);

            /**
             * Makes request to AMP to register service in AMP Database
             */
            virtual BaseError registerService(pid_t pid, Service::Uid service);

            /**
             * Function generates IDs for IPC messages to AMP
             */
            iviLink::Ipc::MsgID genId();

             void onLinkUpNotify();
             void onLinkDownNotify();

            IAppManProtoAmpToApp * mpHandler;   ///< handles incoming requests
            iviLink::Ipc::CIpc * mpIpc;         ///< object of IPC
            iviLink::Ipc::MsgID mId;            ///< ID of last message in IPC
            CSignalSemaphore mConLostSem;       ///< signals in case of connection problems
            bool mBe;                           ///< is false if needs to stop  communication with AMP
            volatile bool mNoConnection;        ///< is true in case of connection problems
            bool mAppInited;                    ///< true if app inited
            tSessionRequestList mSessionReqsList;///< list of session requests that were received before initDone()

            static Logger msLogger;             ///< object of logger
         };

      }

   }

}

#endif /* CAPPMANPROTOCLIENT_HPP_ */
