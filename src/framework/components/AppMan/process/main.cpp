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




#include <climits>
#include <cstdlib>
#include <string>

#include "utils/misc/include/Logger.hpp"
#include "framework/components/AppMan/controller/include/CController.hpp"
#include "framework/components/AppMan/process/SystemControllerMsg.hpp"

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

   char cwd[PATH_MAX] = "";
   chdir(APPLICATION_MANAGER_REPOSITORY);
   LOG4CPLUS_INFO(logger, std::string("cwd = '") +  getcwd(cwd, PATH_MAX) + "'");

   CSignalSemaphore sem;

   AXIS::AMP::CController ampController;

   /**
    * Inits AMP controller
    */
   ampController.init();

#if !defined(NO_SYSTEM_CONTROLLER_LAUNCH)
   /**
    * Inits system controller library
    */
   // AXIS::AMP::SystemControllerMsg sysCtrl(&sem);
#endif // NO_SYSTEM_CONTROLLER_LAUNCH

   /**
    * waiting for exit by signal of system controller
    */
   sem.wait();

   /**
    * uinints AMP controller
    */
   ampController.uninit();

   return 0;
}
