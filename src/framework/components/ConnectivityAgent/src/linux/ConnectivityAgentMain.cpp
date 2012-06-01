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

/***************************************************************************
 * Project           AXIS
 * (c) copyright     2012
 * Company           LUXOFT
 * @file             ConnectivityAgentMain.cpp
 * @brief            Main func of ConnectivityAgent Process
 * @author           KSkidanov
 ***************************************************************************/

#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <string>
#include <cstdlib>

#include "framework/components/ConnectivityAgent/src/generic/HAL/include/CConnectivityAgent.hpp"
#include "framework/components/ConnectivityAgent/src/generic/HAL/include/CCarrierAdapter.hpp"
#include "framework/components/ConnectivityAgent/src/generic/L0/include/L0InterfaceStub.hpp"

#include "config.hpp"

static struct option long_options[] =
   {
      {"unix_addr", required_argument, 0, 'u'},
      {0, 0, 0, 0}
   };



int main(int argc, char* argv[])
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.main"));
   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

   int option_index = 0;
   int c;

   std::string sock_addr;

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


   bool isServer = true;

   if (getenv("CA_SERVER"))
      isServer = true;
   else if (getenv("CA_CLIENT"))
      isServer = false;
   else
   {
      if (optind == argc)
         return 1;

      if (argv[optind][0] == '1')
         isServer = true;
      else if (argv[optind][0] == '2')
         isServer = false;
      else
         return 2;
   }



   if (sock_addr.empty())
   {
      if (isServer)
         sock_addr = std::string(first_lsn_path);
      else
         sock_addr = std::string(second_lsn_path);
   }

   L0InterfaceStub::getInstance()->start(isServer, sock_addr.c_str());

   while(L0InterfaceStub::getInstance()->getShutdownState() == false)
   {
      sleep(1000);
   };
}

