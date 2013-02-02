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


#ifndef UBUNTU_LAUNCHER_HPP_
#define UBUNTU_LAUNCHER_HPP_

#include "Logger.hpp"
#include "ILauncher.hpp"
#include "ILauncherHandler.hpp"
#include <sys/wait.h>

namespace iviLink
{

   namespace AMP
   {

      /**
       * This class is used to launch iviLink applications. Implements ILauncher
       * interface. Makes callbacks using ILauncherHandler interface
       */
      class UbuntuLauncher   : public ILauncher
      {
      public:
         /**
          * Constructor
          */
         UbuntuLauncher();

         /**
          * Virtual destructor
          */
         virtual ~UbuntuLauncher();

         /**
          * Inits itself - sets handler for callbacks
          * @param pHandler is object with ILauncherHandler interface
          */
         virtual void init(ILauncherHandler * pHandler);
         
         virtual void deinit();


         /**
          * Functions is used to launch app using given launch information
          * @param is string with information needed to launch app, in linux it's a full path of exe
          */
         virtual void launchApplication(const std::string & launchInfo);

      private:
         ILauncherHandler * mpHandler; ///< pointer on handler of launcher callbacks

         static void onSIGCHLD(int signal_number);
         static void setSIGCHLDHandler();

         static Logger sLogger; ///< logger object
      };

   }

}

#endif /* UbuntuLauncher_HPP_ */
