/* 
 * 
 * iviLINK SDK, version 1.1.2
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
 * 
 */











#ifndef CLAUNCHER_HPP_
#define CLAUNCHER_HPP_

#include "utils/misc/Logger.hpp"
#include "ILauncher.hpp"
#include "ILauncherHandler.hpp"

#ifndef ANDROID
#else
#include <jni.h>
#include "utils/android/JniThreadHelper.hpp"
#endif //ANDROID

namespace iviLink
{

   namespace AMP
   {

      /**
       * This class is used to launch iviLink applications. Implements ILauncher
       * interface. Makes callbacks using ILauncherHandler interface
       */
      class CLauncher   : public ILauncher
      {
      public:
         /**
          * Constructor
          */
         CLauncher();

         /**
          * Virtual destructor
          */
         virtual ~CLauncher();

         /**
          * Inits itself - sets handler for callbacks
          * @param pHandler is object with ILauncherHandler interface
          */
         #ifndef ANDROID
         void init(ILauncherHandler * pHandler);
         #else
         /**
          * Inits itself - sets handler for callbacks
          * @param pHandler is object with ILauncherHandler interface
          * @param jm is a pointer to instance of Java Virtual Machine which SysCrtlAndr runs in
          * @param launchClass is a reference to Launcher class (see SysCtrlAndr and CApplicationManagerWrapper)
          * @param launchMethod is a method id of static method launchApplicaion (Ljava/lang/String;)I of the aforementioned Launcher class
          */
         void init(ILauncherHandler * pHandler, JavaVM * jm, jclass launchClass, jmethodID launchMethod);
         
         void uninit();
         #endif //ANDROID

      private:

         /**
          * Functions is used to launch app using given launch information
          * @param is string with information needed to launch app, in linux it's a full path of exe
          */
         virtual void launche(const std::string & launchInfo);

         ILauncherHandler * mpHandler; ///< pointer on handler of launcher callbacks
         
         #ifndef ANDROID
         #else
         JavaVM * pJm;           // pointer to jvm of SysCtrlAndr
         jclass jLauncher;       // Launcher class
         jmethodID jLaunchMethod;// int Launcher.launchApplication(String launchinfo) - starts an app and returns its pid
         #endif //ANDROID

         static Logger sLogger; ///< logger object
      };

   }

}

#endif /* CLAUNCHER_HPP_ */
