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


#ifndef CREQUESTHANDLER_HPP_
#define CREQUESTHANDLER_HPP_

#include <map>

#include "IConnectionHandler.hpp"
#include "IAppManProto.hpp"
#include "IAppManProtoAmpToApp.hpp"

#include "IPmpConnectionStatus.hpp"
#include "IPmpHandler.hpp"
#include "IPmpRequest.hpp"

#include "CRunApp.hpp"
#include "IDatabase.hpp"
#include "IReqHandlerForLaunch.hpp"
#include "IReqHandlerForAppCom.hpp"
#include "ILaunchThread.hpp"
#include "IAppComThread.hpp"
#include "Logger.hpp"

class CMutex;

namespace iviLink
{

   namespace AMP
   {

       class AmEventFactory;
      /**
       * Class of request handler, supports main functionality
       * of request handler submodule of AppMan
       */
      class CRequestHandler   : public AppMan::Ipc::IConnectionHandler
                              , public AppMan::Ipc::IAppManProto
                              , public AppMan::Ipc::IPmpConnectionStatus
                              , public AppMan::IPmpRequest
                              , public IReqHandlerForLauncher
                              , public IReqHandlerForAppCom
      {
      public:
         /**
          * Constructor
          */
         CRequestHandler();

         /**
          * Virtual destructor
          */
         virtual ~CRequestHandler();

         /**
          * Inits IPC connection for applications
          * @param pAppConnection is pointer to IConnection object
          * @param pApp is pointer to app protocol
          */
         void initApp (AppMan::Ipc::IAppManProtoAmpToApp * pApp);

         /**
          * Inits PMP handler
          * @param pPmp is pointer to PMP protocol handler
          */
         void initPmp ( AppMan::IPmpHandler * pPmp);

         /**
          * Inits AppMan database in Request Handler
          * @param pDatabase is pointer to database object
          */
         void initDatabase(IDatabase * pDatabase);

         /**
          * Inits launch thread in request handler
          * @param pLaunchThread is pointer to launch thread object
          */
         void initLaunchThread(ILaunchThread * pLaunchThread);

         /**
          * Inits AppComThread
          * @param pAppComThread is pointer to object of AppComThread functionality
          */
         void initAppComThread(IAppComThread * pAppComThread);

         void initEventFactory(AmEventFactory * eventFactory);

         /**
          * Uninits request handler
          */
         void uninit();

         /**
          * Prints state of request handler
          */
         void printState() const;


      private:
//       IConnectionHandler
         /**
          * Callback from app protocol that says - application lost connection with AMP
          */
         virtual void onConnectionLost(pid_t pid);

//       IAppManProto
         /**
          * Request from app to init app in AMP
          * @param pid of process
          * @param listOfSupportedServices is list with services that could be start in this process
          */
         virtual AppMan::EInitResult initApplication(pid_t pid,
               std::list<Service::Uid> listOfSupportedServices);

//       IAppManProto
          // connectivity
          bool isLinkAlive();

          /**
          * Application says if it can support given service
          * @param pid of process
          * @param service is UID of service
          * @param use is true if app can start session of given service
          */
         virtual BaseError useService(pid_t pid, Service::Uid service, bool use);

         /**
          * Makes request to database to say tha this app supports given service
          * @param pid of process
          * @param service is UID of service
          */
         virtual BaseError registerService(pid_t pid, Service::Uid service);

//       IPmpConnectionStatus
         /**
          * Is called if AMP loses connection with PMP
          */
         virtual void onConnectionLost();

         virtual void onProfileManagerConnection();

//       IPmpRequest
         /**
          * Request from PMP to start new session
          * @param session is UID of session
          * @param service is UID of service
          */
         virtual BaseError applicationRequest(const iviLink::Service::SessionUid & session,
               const iviLink::Service::Uid & service);

         virtual void waitingServiceReqeust(const iviLink::Service::Uid & service);

// IReqHandlerForLauncher
         /**
          * Request from app to say AMP pid of app
          * @param session is UID of session
          * @param pid of app
          */
         virtual void setSessionPid(Service::SessionUid session, pid_t pid);

// IReqHandlerForAppCom
         /**
          * Sets launch information of application
          * @param pid of process
          * @param launchInfo is launching information of applicaion
          */
         virtual void setProcAppLanchInfo(pid_t pid, const std::string &launchInfo);

         AppMan::Ipc::IAppManProtoAmpToApp * mpApp; ///< pointer to app IPC protocol
         AppMan::IPmpHandler * mpPmp; ///< pointer to PMP protocol
         IDatabase * mpDatabase; ///< pointer to AppMan database
         std::map<pid_t, CRunApp> mApps; ///< map of running applications
         CMutex * mpAppMutex; ///< mutex fot mApps
         ILaunchThread * mpLaunchThread; ///< pointer to obhect of launch thread
         std::map<Service::SessionUid, CRunApp> mSessions; ///< mSessions is map of running apps
         CMutex * mpSessionMutex; ///< mutex is used for mSessions locking
         IAppComThread * mpAppComThread; ///< pointer to AppComThread
         std::map<Service::SessionUid, Service::Uid> mSessionServices; ///< map of services in sessions
         CMutex * mpSessionServicesMutex; ///< mutex for mSessionServices
         AmEventFactory * mEventFactory; ///< event factory

         static Logger msLogger; ///< object of logger
      };

   }

}

#endif /* CREQUESTHANDLER_HPP_ */
