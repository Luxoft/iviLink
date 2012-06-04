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





#ifndef CAPPMANPROTOCLIENT_HPP_
#define CAPPMANPROTOCLIENT_HPP_

#include "framework/libraries/AppMan/AmpForApp/include/IAppManProto.hpp"
#include "framework/libraries/AppMan/AmpForApp/include/IAppManProtoAmpToApp.hpp"
#include "utils/misc/include/Logger.hpp"
#include "utils/ipc/include/CIpc.hpp"
#include "utils/ipc/include/ICallbackHandler.hpp"
#include "utils/threads/include/CSignalSemaphore.hpp"

namespace AXIS
{

   namespace AppMan
   {

      namespace Ipc
      {

         /**
          * Class implements client side of IPC library
          */
         class CAppManProtoClient   : public IAppManProto
                                    , public AXIS::Ipc::ICallbackHandler
         {
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
            void loop();

            /**
             * Disconnects IPC
             */
            void disconnect();

            /**
             * Returns true if connection is OK
             */
            bool checkConnection() const;

         private:
            /**
             * Callback is called in case of new connection
             */
            virtual void OnConnection(AXIS::Ipc::DirectionID dirId);

            /**
             * Callback is callse in case of connection losing
             */
            virtual void OnConnectionLost(AXIS::Ipc::DirectionID dirId);

            /**
             * Callback is called in case of incoming messages
             */
            virtual void OnRequest(AXIS::Ipc::MsgID id, UInt8 const* pPayload,
                  UInt32 payloadSize, UInt8* const pResponseBuffer,
                  UInt32& bufferSize, AXIS::Ipc::DirectionID dirId);

            /**
             * Makes request to AMP to initApplication
             * @param pid of application
             * @param listOfSupportesServices contaions list of UIDs that app can start at moment
             */
            virtual EInitResult initApplication(pid_t pid, std::list<Service::Uid> listOfSupportedServices);

            /**
             * Maket request to AMP to sau use or no given service in application
             */
            virtual CError useService(pid_t pid, Service::Uid service, bool use = true);

            /**
             * Makes request to AMP to register service in AMP Database
             */
            virtual CError registerService(pid_t pid, Service::Uid service);

            /**
             * Function generates IDs for IPC messages to AMP
             */
            AXIS::Ipc::MsgID genId();

            IAppManProtoAmpToApp * mpHandler;   ///< handles incoming requests
            AXIS::Ipc::CIpc * mpIpc;            ///< object of IPC
            AXIS::Ipc::MsgID mId;               ///< ID of last message in IPC
            CSignalSemaphore mConLostSem;       ///< signals in case of connection problems
            bool mBe;                           ///< is false if needs to stop  communication with AMP
            volatile bool mNoConnection;        ///< is true in case of connection problems

            static Logger msLogger;              ///< object of logger
         };

      }

   }

}

#endif /* CAPPMANPROTOCLIENT_HPP_ */
