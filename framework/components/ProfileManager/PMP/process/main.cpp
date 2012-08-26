/* 
 * 
 * iviLINK SDK, version 1.0.1
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









#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <unistd.h>

#include <tr1/array>



#include "framework/components/ProfileManager/PMP/componentManager/CPmpComponentManager.hpp"
#include "utils/threads/CSignalSemaphore.hpp"
#include "utils/configurator/configurator.h"
#include "SystemControllerMsg.hpp"
#include "framework/libraries/AppMan/Pmp/CAppManPmpController.hpp"

static struct option long_options[] =
   {
      { "cs_unix_addr",   required_argument, 0, 'x' },
      { "ca_unix_addr",   required_argument, 0, 'u' },
      { "repo_unix_addr", required_argument, 0, 'r' },
      { "pmp_unix_addr", required_argument, 0, 'p' },
      { 0, 0, 0, 0 }
   };

std::tr1::array<char, 1024> ca_sock_path   = {{0}};
std::tr1::array<char, 1024> cs_sock_path   = {{0}};
std::tr1::array<char, 1024> repo_sock_path = {{0}};
std::tr1::array<char, 1024> pmp_sock_path  = {{0}};



int main(int argc, char **argv)
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.process.main"));
   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

   int option_index = 0;
   int c;

   while (true)
   {
      c = getopt_long(argc, argv, "x:u:r:p:", long_options, &option_index);

      if (-1 == c)
         break;

      switch (c)
      {
      case 'x':
         strncpy(cs_sock_path.begin(), optarg, cs_sock_path.size());
         cs_sock_path[cs_sock_path.size() - 1] = 0;
         break;
      case 'u':
         strncpy(ca_sock_path.begin(), optarg, ca_sock_path.size());
         ca_sock_path[ca_sock_path.size() - 1] = 0;
         break;
      case 'r':
         strncpy(repo_sock_path.begin(), optarg, repo_sock_path.size());
         repo_sock_path[repo_sock_path.size() - 1] = 0;
         break;
      case 'p':
         strncpy(pmp_sock_path.begin(), optarg, pmp_sock_path.size());
         pmp_sock_path[pmp_sock_path.size() - 1] = 0;
         break;
      default:
         LOG4CPLUS_FATAL(logger, "unknown param");
         exit(1);
         break;
      }
   }

   if (ca_sock_path[0] != 0)
   {
      extern char const * gpCA_UNIX_SOCK_PATH;
      gpCA_UNIX_SOCK_PATH = ca_sock_path.begin();
   }

   if (ca_sock_path[0] != 0)
   {
      extern char const * gpNEGOTIATOR_IPC_ADDR;
      gpNEGOTIATOR_IPC_ADDR = cs_sock_path.begin();
   }

   iviLink::conf::Configurator config("");

   if (repo_sock_path[0] != 0)
   {
      config.setParam("pmp_repo_ipc_address", repo_sock_path.begin());
   }

   if (pmp_sock_path[0] != 0)
   {
      config.setParam("pmp_ipc_address", pmp_sock_path.begin());
   }

   CSignalSemaphore sem;

#if !defined(NO_SYSTEM_CONTROLLER_LAUNCH)
   iviLink::PMP::SystemControllerMsg sysCtrl(&sem);
#endif // NO_SYSTEM_CONTROLLER_LAUNCH

   iviLink::PMP::CPmpComponentManager::instance(&config);

   while (true)
   {
      iviLink::AppMan::Ipc::CAppManPmpController * appctrl = iviLink::AppMan::Ipc::CAppManPmpController::instance();
      if (!appctrl)
         return 20;
      if (appctrl->checkConnection())
         break;
      if (0 == sem.tryWait())
         return 0;

      // waiting for app man connection
      sleep(1);
   }
#if !defined(NO_SYSTEM_CONTROLLER_LAUNCH)
   sysCtrl.requestConnected();
#endif // NO_SYSTEM_CONTROLLER_LAUNCH

   sem.wait();

   return 0;
}
