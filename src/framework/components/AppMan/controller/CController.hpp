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
 

#ifndef CCONTROLLER_HPP_
#define CCONTROLLER_HPP_

#include "Logger.hpp"

#ifndef ANDROID
#else
#include <jni.h>
#endif //ANDROID

#include <tr1/functional>

namespace iviLink
{
   namespace AppMan
   {
      namespace Ipc
      {
         class CAppManProtoServer;
      }
   }
   namespace AMP
   {

      class CDatabase;
      class CLauncher;
      class CAppComThread;
      class CLaunchThread;
      class CRequestHandler;

      /**
       * This class is the controller of Application Manager Process (AMP)
       * It inits all submodules of AppMan
       */
      class CController
      {
         public:
            /**
             * Constructor of AMP Controller
             */
            CController();

            /**
             * Destructor of AMP Controller
             */
            ~CController();

            /**
             * Function is used to init AMP
             */
#ifndef ANDROID
            void init();
#else
            void init(std::string pathToDatabase, JavaVM * jm, jclass launchClass, jmethodID launchMethod);
#endif //ANDROID

            /**
             * Function is used to make uninit AMP
             */
            void uninit();

          void setLinkStateGetter( std::tr1::function<bool ()> getter );
          void onLinkUpNotify();
          void onLinkDownNotify();

         private:
            CAppComThread * mpAppComThread; ///< Pointer to object of AppComThread class instance
            CLaunchThread * mpLaunchThread; ///< Pointer to object of LaunchThread class instance
            AppMan::Ipc::CAppManProtoServer * mpAppManProtoServer; ///< Pointer to instance of AppMan Protocol (server part)
            CDatabase * mpDatabase; ///< Pointer to instance of AppMan Database
            CLauncher * mpLauncher; ///< Pointer to instance of
            CRequestHandler * mpRequestHandler; ///< Pointer to request handler instance

            static Logger msLogger; ///< object of logger

      };

   } /* namespace AMP */
} /* namespace AXIS */
#endif /* CCONTROLLER_HPP_ */
