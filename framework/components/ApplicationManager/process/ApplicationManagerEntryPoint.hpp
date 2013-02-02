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
#include <climits>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <signal.h>

#include "Logger.hpp"
#include "CController.hpp"
#include "SystemControllerMsg.hpp"
#include "CPmpController.hpp"

#include "WatchdogThread.hpp"
#include "Components.hpp"

namespace iviLink
{
	namespace AMP
	{
		int appManProcessEntryPoint(CDatabase * database, ILauncher * launcher)
		{
		   /**
    		* Inits logger
    		*/
   		   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.main"));
   		   WatchdogThread watchdogThread(SystemController::APPLICATION_MANAGER);
   		   watchdogThread.start();
		   CSignalSemaphore sem;
		   iviLink::AMP::CController ampController(database, launcher);
		   ampController.init();

#ifndef NO_SYSTEM_CONTROLLER_LAUNCH
		   /**
		    * Inits system controller library
		    */
		   iviLink::AMP::SystemControllerMsg sysCtrl(&sem);

		   { // Connect apps and sys controller to provide correct link info and link state changes callbacks
		       using namespace std::tr1;
		       using namespace iviLink::AMP;
		       ampController.setLinkStateGetter( bind( &SystemControllerMsg::isLinkAlive, ref(sysCtrl) ) );
		       sysCtrl.setLinkCallbacks( bind( &CController::onLinkUpNotify, ref(ampController) ),
		                                   bind( &CController::onLinkDownNotify, ref(ampController) ) );
		   }
#endif // NO_SYSTEM_CONTROLLER_LAUNCH


		   bool connEstablished = false;
		   bool needExit = false;
		   while (true)
		   {
		      iviLink::AppMan::Ipc::CPmpController* pmpctrl = iviLink::AppMan::Ipc::CPmpController::instance();
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


#ifndef NO_SYSTEM_CONTROLLER_LAUNCH
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
	} // AMP
} // iviLink
