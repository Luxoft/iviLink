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





#ifndef COMPONENTLAUNCHER_HPP
#define COMPONENTLAUNCHER_HPP

#ifndef PREFIX
#define PREFIX ./
#endif

#include <map>
#include <string>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>

#include "utils/misc/include/Logger.hpp"

namespace SystemController
{

enum Components
{
   PROFILE_MANAGER,
   CHANNEL_SUPERVISOR,
   CONNECTIVITY_AGENT,
   APPLICATION_MANAGER
};

class CComponentLauncher
{
private:
   static Logger sLogger;

   static const std::string COMPONENT_PATH;

protected:

   CComponentLauncher();

public:

   static CComponentLauncher* getInstance();

   virtual ~CComponentLauncher();

   void launchProfileManager();
   void launchChannelSupervisor();
   void launchConnectivityAgent(int connectivityAgentGender);
   void launchApplicationManager();

private:

   static CComponentLauncher* sInstance;

   std::map<Components, std::string> mComponentLocation;

   bool isComponentRunning(Components component);
   void shutdownComponent(Components component);
   void launchComponent(Components component);
   void launchComponent(Components component, int componentGender);

   static void onSIGCHLD(int signal_number);
   static void setSIGCHLDHandler();
};

}
#endif /* COMPONENTLAUNCHER_HPP */
