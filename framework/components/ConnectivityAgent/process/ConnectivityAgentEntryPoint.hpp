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


#ifndef __CONNECTIVITY_AGENT_ENTRY_POINT__
#define __CONNECTIVITY_AGENT_ENTRY_POINT__


#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <string>
#include <cstdlib>

#include "CConnectivityAgent.hpp"
#include "CCarrierAdapter.hpp"
#include "L1InterfaceStub.hpp"
#include "ConnectivityAgentConfig.hpp"
#include "Components.hpp"
#include "WatchdogThread.hpp"


using namespace iviLink::ConnectivityAgent::L1;

static struct option long_options[] =
   {
      {"unix_addr", required_argument, 0, 'u'},
      {0, 0, 0, 0}
   };


namespace iviLink
{
   namespace ConnectivityAgent
   {
      int connectivityAgentEntryPoint(int argc = 0, char ** argv = NULL)
      {
         bool isServer = true;
         std::string sock_addr;

         WatchdogThread watchdogThread(SystemController::CONNECTIVITY_AGENT);
         watchdogThread.start();
         
#ifndef ANDROID
         Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.main"));
         PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

         int option_index = 0;
         int c;

         while (true)
         {
            c = getopt_long(argc, argv, "u:", long_options, &option_index);

            if (-1 == c)
               break;

            switch (c)
            {
            case 'u':
               sock_addr = std::string(optarg);
               break;
            default:
               exit(1);
               break;
            }
         }

         if (getenv("CA_SERVER"))
            isServer = true;
         else if (getenv("CA_CLIENT"))
            isServer = false;
         else
         {
            if (optind != argc)
            {
               if (argv[optind][0] == '1')
                  isServer = true;
               else if (argv[optind][0] == '2')
                  isServer = false;
               else
                  return 2;
            }
         }
#else
#endif //ANDROID

         if (sock_addr.empty())
         {
            if (isServer)
               sock_addr = std::string(first_lsn_path);
            else
               sock_addr = std::string(second_lsn_path);
         }

         L1InterfaceStub::getInstance()->start(isServer, sock_addr.c_str());
         while(L1InterfaceStub::getInstance()->getShutdownState() == false)
         {
            sleep(1000);
         };
         return 0;
      }
   }
}
#endif // __CONNECTIVITY_AGENT_ENTRY_POINT__
