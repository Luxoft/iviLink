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


#ifndef CAPPMANCONNECTCONTROLLER_HPP_
#define CAPPMANCONNECTCONTROLLER_HPP_

#include "AppInfo.hpp"
#include "Logger.hpp"
#include <tr1/functional>

namespace iviLink
{
namespace App
{
class CAppManProxy;
}
   namespace AppMan
   {
      class IAppManHandler;
      class IAppMan;

      namespace App
      {
         class CCore;
      }

      namespace Ipc
      {
         class CAppManConnect;
         class CAppManProtoClient;
      }

      /**
       * Class is used to organize client side (app side) of communication between AppMan and applications
       */
      class CAppManConnectController
      {
      public:
         /**
          * Returns instance of controller
          */
         static CAppManConnectController * instance(Android::AppInfo appInfo);

         static void deleteInstance();

         /**
          * Returns object of IPC protocol client
          */
         IAppMan * applicationManager();

         /**
          * Inits handler of requests from AMP
          */
          typedef std::tr1::function<void ()> notify_t;

          void init(IAppManHandler * pHandler);

          void registerAMProxy(iviLink::App::CAppManProxy * amProxy);

         /**
          * Uninits IPC
          */
         void uninit();

         /**
          * Returns true if connection is OK
          */
         bool checkConnection () const;

         /**
          * Is used to get call same function in AppManProto
          */
         void checkSessionRequest();

      private:
         /**
          * Private constructor because of singleton object
          */
         CAppManConnectController(Android::AppInfo appInfo);

         /**
          * Virtual destructor
          */
         virtual ~CAppManConnectController();

         /// Not implemented
         CAppManConnectController(const CAppManConnectController &);
         CAppManConnectController & operator = (const CAppManConnectController &);

         Ipc::CAppManProtoClient * mpClient; ///< Pointer to IPC requests handler
         Ipc::CAppManConnect * mpConnection; ///< pointer to connection status handler
         App::CCore * mpCore;                ///< pointer to core object

         static Logger msLogger;             ///< logger object
         static CAppManConnectController * mspInstance; ///< instance of controller

      };

   }

}

#endif /* CAPPMANCONNECTCONTROLLER_HPP_ */
