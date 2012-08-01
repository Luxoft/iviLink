/* 
 * 
 * iviLINK SDK, version 1.0
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






#include <climits>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <signal.h>

#include "utils/misc/Logger.hpp"
#include "framework/components/AppMan/controller/CController.hpp"
#include "SystemControllerMsg.hpp"
#include "framework/libraries/AppMan/AmpForPmp/CPmpController.hpp"

using iviLink::AppMan::Ipc::CPmpController;


int main(int argc, char **argv)
{
   /**
    * Inits logger
    */
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.main"));

   /**
    * Gets logger properties from file
    */
   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

   LOG4CPLUS_INFO(logger, "AMP started...");

   signal(SIGCHLD, SIG_IGN);

   char cwd[PATH_MAX] = "";
   chdir(APPLICATION_MANAGER_REPOSITORY);
   LOG4CPLUS_INFO(logger, std::string("cwd = '") +  getcwd(cwd, PATH_MAX) + "'");

   CSignalSemaphore sem;

   iviLink::AMP::CController ampController;

   /**
    * Inits AMP controller
    */
   ampController.init();

#if !defined(NO_SYSTEM_CONTROLLER_LAUNCH)
   /**
    * Inits system controller library
    */
   iviLink::AMP::SystemControllerMsg sysCtrl(&sem);
#endif // NO_SYSTEM_CONTROLLER_LAUNCH

   bool connEstablished = false;
   bool needExit = false;
   while (true)
   {
      CPmpController* pmpctrl = CPmpController::instance();
      if (!pmpctrl)
      {
         needExit = true;
         break;
      }
      if (pmpctrl->checkConnection())
      {
         connEstablished = true;
         break;
      }
      if (0 == sem.tryWait())
      {
         needExit = true;
         break;
      }

      // waiting for app man connection
      sleep(1);
   }
#if !defined(NO_SYSTEM_CONTROLLER_LAUNCH)
   if (connEstablished)
      sysCtrl.requestConnected();
#endif // NO_SYSTEM_CONTROLLER_LAUNCH 

   /**
    * waiting for exit by signal of system controller
    */
   if (!needExit)
      sem.wait();

   /**
    * uinints AMP controller
    */
   ampController.uninit();

   return 0;
}
