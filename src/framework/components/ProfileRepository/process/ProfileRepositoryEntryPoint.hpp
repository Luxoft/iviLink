/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 
#ifndef __PROFILE_REPOSITORY_ENTRY_POINT__
#define __PROFILE_REPOSITORY_ENTRY_POINT__


#include <climits>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <string>
#include <unistd.h>

#include "CProfileRepoServerClb.hpp"
#include "configurator.h"
#include "Logger.hpp"

namespace iviLink
{
	namespace ProfileRepository
	{

		static struct option long_options[] =
   		{
      		{"ipc_addr", required_argument, 0, 'i'},
      		{"help", required_argument, 0, 'h'},
      		{0, 0, 0, 0}
   		};


		void print_help(int argc, char** argv)
		{	
   			printf("%s\n", argv[0]);
  	 		puts("\t--help (-h) -- print this message");
   			puts("\t--ipc_addr addr (-i) -- set address for ipc server");
		}

#ifndef ANDROID
		int profileRepositoryEntryPoint(int argc, char ** argv)
#else
		int profileRepositoryEntryPoint(std::string pathToDatabase)
#endif //ANDROID
		{
		   	iviLink::conf::Configurator config("");
#ifndef ANDROID
			Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("profileRepository.main"));
  			PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

   			int option_index = 0;
   			int c;

   			if (argv[0] != 0)
		   	{
		    	LOG4CPLUS_INFO(logger, "argv[0] = " + std::string(argv[0]));
		   	}
		   	char cwd[PATH_MAX] = "";
		   	getcwd(cwd, PATH_MAX);
		   	if (cwd != 0)
		   	{
		    	LOG4CPLUS_INFO(logger, "cwd = '" + std::string(cwd) + "'");
		   	}
		   

		   	chdir(PROFILE_REPOSITORY);

		   	while (true)
		   	{
		    	c = getopt_long(argc, argv, "i:h", long_options, &option_index);
			    if (-1 == c)
		        	break;

		      	switch (c)
		      	{
		      	case 'i':
		        	if (optarg != 0)
		         	{
		            	LOG4CPLUS_INFO(logger, "option ipc_addr = '" + std::string(optarg) + "'");
		         	}
		         	config.setParam("ipc_addr", std::string(optarg));
		         	break;
		      	case 'h':
		        	print_help(argc, argv);
		        	exit(0);
		        	break;
		      	default:
		        	LOG4CPLUS_INFO(logger, "unknown param " + convertIntegerToString(c));
		        	exit(1);
		        	break;
		      	}
		   	}
#else
		   	config.setParam("path_to_database", pathToDatabase);
#endif //ANDROID

		   	iviLink::ProfileRepository::CProfileRepoServerClb server(config);
		   	return server.loop();
		}
	}
}

#endif // __PROFILE_REPOSITORY_ENTRY_POINT__
