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





#ifndef CAPPMANCONNECTCONTROLLER_HPP_
#define CAPPMANCONNECTCONTROLLER_HPP_

#include "utils/misc/include/Logger.hpp"

#include "framework/libraries/AppMan/AmpForApp/include/IAppManProto.hpp"
#include "framework/libraries/AppMan/AmpForApp/include/IAppManProtoAmpToApp.hpp"
#include "framework/libraries/AppMan/App/include/IAppManHandler.hpp"
#include "framework/libraries/AppMan/App/include/CAppManConnect.hpp"
#include "framework/libraries/AppMan/App/include/CCore.hpp"

namespace AXIS
{

   namespace AppMan
   {

      namespace Ipc
      {
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
         static CAppManConnectController * instance();

         /**
          * Returns object of IPC protocol client
          */
         IAppMan * applicationManager();

         /**
          * Inits handler of requests from AMP
          */
         void init(IAppManHandler * pHandler);

         /**
          * Uninits IPC
          */
         void uninit();

         /**
          * Returns true if connection is OK
          */
         bool checkConnection () const;

      private:
         /**
          * Private constructor because of singleton object
          */
         CAppManConnectController();

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
