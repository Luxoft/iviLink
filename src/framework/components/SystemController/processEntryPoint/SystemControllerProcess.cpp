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





#include <getopt.h>
#include "framework/components/SystemController/ssm/stateMachine/include/CSystemStateMachine.hpp"

#include "utils/misc/include/Logger.hpp"

using namespace SystemController;

int main(int argc, char* argv[])
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.main"));

   PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

   int next_option;
   const char* gender = NULL;

   const char* const short_options = "hg:";
   const struct option long_options[] = {
         { "help",     0, NULL, 'h' },
         { "gender",   0, NULL, 'g' },
         { NULL,       0, NULL, 0   }
   };

   do
   {
      next_option = getopt_long(argc, argv, short_options,
                                long_options, NULL);

      switch(next_option)
      {
      case 'h':
         LOG4CPLUS_WARN(logger, "Print Ussage");
         return 0;
      case 'g':
         gender = optarg;
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

   if(gender == NULL)
   {
      LOG4CPLUS_WARN(logger, "Wrong input");
      return 0;
   }

   CSystemStateMachine* ssm = new CSystemStateMachine();

   if(string(gender) == "server")
   {
      LOG4CPLUS_INFO(logger, "Start as Server");
      ssm->init(1);
   }
   else if(string(gender) == "client")
   {
      LOG4CPLUS_INFO(logger, "Start as Client");
      ssm->init(2);
   }

   while(true)
   {}

   return 0;
}
