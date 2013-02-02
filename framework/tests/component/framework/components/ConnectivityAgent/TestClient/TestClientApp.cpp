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


#include <cstring>
#include <cstdlib>
#include <getopt.h>
#include <tr1/array>

#include <time.h>
#include <unistd.h>
#include "CClient.hpp"
#include "utils/misc/Logger.hpp"

static struct option long_options[] =
   {
      { "ca_unix_addr",   required_argument, 0, 'u' },
      { 0, 0, 0, 0 }
   };

std::tr1::array<char, 1024> ca_sock_path   = {{0}};

int main(int argc, char* argv[])
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("ConnectivityAgent.test.client.main"));

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
      case 'u':
         //Log("option u = '%s'", optarg);
         strncpy(ca_sock_path.begin(), optarg, ca_sock_path.size());
         ca_sock_path[ca_sock_path.size() - 1] = 0;
         break;
      default:
         //Log("unknown param %c (%d)", c, c);
         exit(1);
         break;
      }
   }

   if (ca_sock_path[0] != 0)
   {
      extern char const * gpCA_UNIX_SOCK_PATH;
      gpCA_UNIX_SOCK_PATH = ca_sock_path.begin();
   }

   // please run client first
   CSignalSemaphoreInterproc::unlink("/connectivity_agent_test_server");
   CSignalSemaphoreInterproc::unlink("/connectivity_agent_test_client");

   CClient client;
   client.threadFunc();
   // client.start();
   // while(true)
   // {	

   //    sleep(1000);
   // };
}
