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


#ifndef CAPPMANPROTOSERVER_HPP_
#define CAPPMANPROTOSERVER_HPP_

#include <map>
#include <tr1/functional>

#include "Logger.hpp"
#include "CIpc.hpp"
#include "ICallbackHandler.hpp"
#include "IAppManProto.hpp"
#include "IAppManProtoAmpToApp.hpp"

class CMutex;

namespace iviLink
{

   namespace AppMan
   {

      namespace Ipc
      {

         class IConnectionHandler;

         /**
          * Class implements server side of communication protocol betwenn AMP and applications.
          * This server side is used in AMP
          */
         class CAppManProtoServer   : public IAppManProtoAmpToApp
                                    , public iviLink::Ipc::ICallbackHandler
         {
         public:
            /**
             * Constructor
             */
            CAppManProtoServer();

            /**
             * Virtual destructor
             */
            virtual ~CAppManProtoServer();

            /**
             * Inits server side protocol. It's a handler for app requests to AMP
             * @param pServer is pointer to object of IPC protocol
             */
            void init (IAppManProto * pServer);

            /**
             * Inits connectin handler
             */
            void initConnectionHandler(IConnectionHandler * pHandler);

            /**
             * Function begin IPC communication with applications (starts waiting for
             * requests from applications
             */
            bool startWaitingForClients();

             // Connectivity callbacks
             void set_link_state_getter( std::tr1::function<bool ()> getter );
             virtual void onLinkUpNotify();
             virtual void onLinkDownNotify();
             bool isLinkAlive();
         private:
            /**
             * Callback is called in case of new connection
             * @param dirId is ID of application in IPC library
             */
            virtual void OnConnection(iviLink::Ipc::DirectionID dirId);

            /**
             * Callback is called in case of losing connection
             * @param dirId is ID of application in IPC library
             */
            virtual void OnConnectionLost(iviLink::Ipc::DirectionID dirId);

            /**
             * Callback is called in case of incoming requests
             */
            virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
                  UInt32 payloadSize, UInt8* const pResponseBuffer,
                  UInt32& bufferSize, iviLink::Ipc::DirectionID dirId);
            virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload,
                  UInt32 payloadSize, iviLink::Ipc::DirectionID dirId) {}

            /**
             * Makes request to application to start session
             * @param pid of application
             * @param session is UID of session
             * @param service is UID of service
             */
            virtual BaseError sessionRequest(pid_t pid, const iviLink::Service::SessionUid & session,
                  const iviLink::Service::Uid & service);

            virtual BaseError waitingServiceRequest(pid_t pid, const iviLink::Service::Uid & service);

            /**
             * Makes request to app to get launch information
             * @param pid of application
             * @param launchInfo is string with launch information
             */
            virtual BaseError getAppLaunchInfo(pid_t pid, std::string & launchInfo);

            /**
             * Function is used to generate IDs for IPC messages to applications
             */
            iviLink::Ipc::MsgID genId();

             void notify( iviLink::Ipc::DirectionID id, UInt8 notification );
             void notify_all( UInt8 notification );

            IAppManProto * mpServer; ///< Pointer to server side handler of request from applications
            IConnectionHandler * mpConnectionHandler; ///< Pointer to connection status handler
            iviLink::Ipc::CIpc * mpIpc; ///< Pointer to object of IPC connection
            iviLink::Ipc::MsgID mId; ///< ID of last IPC message
             typedef std::map<pid_t, iviLink::Ipc::DirectionID> clients_map_t;
             clients_map_t mClients; ///< map of DirIDs of applications connected to AMP
            CMutex * mpClientsMutex; ///< Mutex for mClients

            static Logger msLogger; ///< Object of logger

             std::tr1::function<bool ()> link_state_getter;
         };

      }

   }

}

#endif /* CAPPMANPROTOSERVER_HPP_ */
