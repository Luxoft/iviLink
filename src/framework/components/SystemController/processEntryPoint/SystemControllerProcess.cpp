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










#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <unistd.h>
#include "framework/components/SystemController/ssm/stateMachine/CSystemStateMachine.hpp"

#include "utils/misc/Logger.hpp"

#include "reset.hpp"

using namespace SystemController;

void help(char const* argv0)
{
   printf("%s: iviLink system controller\n", argv0);
   puts("Usage:");
   puts("\t--help, -h\tThis message");
   puts("\t--reset, -r\tPerforms the reset of running daemon");
   puts("\t--stop, -s\tStops the running daemon");
   puts("\t--noauth, -n\tLaunches iviLink stack without authentication");
   exit(0);
}

int main(int argc, char* argv[])
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.main"));

   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

   setupReset(argv[0]);

   int next_option;
   bool authOn = true;

   const char* const short_options = "hrsn";
   const struct option long_options[] = {
         { "help",     0, NULL, 'h' },
         { "reset",    0, NULL, 'r' },
         { "stop",     0, NULL, 's' },
         { "noauth",   0, NULL, 'n' },
         { NULL,       0, NULL, 0   }
   };

   do
   {
      next_option = getopt_long(argc, argv, short_options,
                                long_options, NULL);

      switch(next_option)
      {

      case 'h':
         help(argv[0]);
         break;
      case 'r':
         LOG4CPLUS_INFO(logger, "Resetting, stopping existing daemon");
         hardReset(false);
         break;
      case 's':
         LOG4CPLUS_INFO(logger, "Stopping existing daemons");
         return hardStop(false);
      case 'n':
         authOn = false;
         break;
      case '?':
         LOG4CPLUS_WARN(logger, "Wrong input");
         return 0;
      case -1:
         break;
      }
   }
   while (next_option != -1);

   LOG4CPLUS_INFO(logger, "SystemController started...");

   CSystemStateMachine* ssm = new CSystemStateMachine();

   ssm->init(authOn);

   while(true)
   {
      sleep(100);
   }

   return 0;
}
