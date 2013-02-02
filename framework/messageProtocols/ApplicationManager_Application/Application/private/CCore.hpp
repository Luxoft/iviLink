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


#ifndef CCORE_HPP_
#define CCORE_HPP_

#include "AppInfo.hpp"
#include "Logger.hpp"
#include "IAppMan.hpp"
#include "IAppManHandler.hpp"
#include "IAppManProto.hpp"
#include "IAppManProtoAmpToApp.hpp"

namespace iviLink
{
namespace App
{
class CAppManProxy;
}
   namespace AppMan
   {

      namespace App
      {


         /**
          * Class is used to implement app part of AppMan logic
          */
         class CCore : public IAppMan
                     , public iviLink::AppMan::Ipc::IAppManProtoAmpToApp
         {
         public:
            /**
             * Constructor
             */
            CCore(Android::AppInfo appInfo);

            /**
             * Virtual destructor
             */
            virtual ~CCore();

            /**
             * Inits handler for requests
             * @param pHandler is pointer to handler of requests from AMP
             */
            void initHandler(IAppManHandler * pHandler);

            /**
             * Inits IPC
             * @param pProto is pointer to IPC protocol object
             */
            void initIpc(iviLink::AppMan::Ipc::IAppManProto * pProto);

            void init_callbacks(notify_t link_up, notify_t link_down);

            void registerAMProxy(iviLink::App::CAppManProxy * amProxy);

            /**
             * Uninits core
             */
            void unInit();

         private:

            // IAppMan functions
            /**
             * Sends to AMP initApplication request adding pid of current process
             */
            virtual EInitResult initApplication(std::list<Service::Uid> listOfSupportedServices);

            /**
             * Sends useService request to AMP adding pid of current process
             */
            virtual BaseError useService(Service::Uid service, bool use = true);

            /**
             * Sends register service request to AMP adding pid of current process
             */
            virtual BaseError registerService(Service::Uid service);

            // IAppManProtoAmpToApp functions
            /**
             * Callback from AMP to start session
             * @param pid of current process
             * @param session is UID of session
             * @param service is UID of service
             */
            virtual BaseError sessionRequest(pid_t pid, const iviLink::Service::SessionUid & session,
                  const iviLink::Service::Uid & service);

            virtual BaseError waitingServiceRequest(pid_t pid, const iviLink::Service::Uid & service);

            /**
             * Callback from AMP to ask launching information
             * @param pid of current process
             * @param launchInfo is a string that contains information neede to launch application
             */
            virtual BaseError getAppLaunchInfo(pid_t pid, std::string & launchInfo);

             // connectivity
             bool isLinkAlive();
             // connectivity callbacks
             void onLinkUpNotify();
             void onLinkDownNotify();

             void setLinkCallbacks( notify_t link_up_, notify_t link_down_ );
            /**
             * Function returns full name of app in filesystem
             */
#ifndef ANDROID
            void getExeName();
#endif //ANDROID - won't work on android as it will always return zygote's name and not app's name,
                   // that's why we pass the launch string explicitly

            IAppManHandler * mpHandler;                  ///< Pointer to handler of incoming requests
            iviLink::AppMan::Ipc::IAppManProto * mpProto;   ///< Pointer to IPC protocol with AMP
            pid_t mPid;                                  ///< pid of current process
            std::string mLaunchInfo;                     ///< this string contains launch information

            static Logger msLogger;                       ///< object of logger

             notify_t link_up, link_down;

             iviLink::App::CAppManProxy * mAMProxy;

         };

      }

   }

}

#endif /* CCORE_HPP_ */
