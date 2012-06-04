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





#include <unistd.h>

#include "framework/components/SystemController/componentLauncher/include/CComponentLauncher.hpp"

using namespace std;

namespace SystemController
{

const string CComponentLauncher::COMPONENT_PATH = string(PREFIX);

Logger CComponentLauncher::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.CComponentLauncher"));

CComponentLauncher* CComponentLauncher::sInstance = 0;

CComponentLauncher* CComponentLauncher::getInstance()
{
   LOG4CPLUS_TRACE(sLogger, "getInstance()");

   if(0 == sInstance)
   {
	   sInstance = new CComponentLauncher();
      CComponentLauncher::setSIGCHLDHandler();
   }

   return sInstance;
}

CComponentLauncher::CComponentLauncher()
{
   LOG4CPLUS_TRACE(sLogger, "CComponentLauncher()");

   mComponentLocation.insert(pair<Components, string>(PROFILE_MANAGER, COMPONENT_PATH + string(PROFILE_MANAGER_PROCESS_NAME)));
   mComponentLocation.insert(pair<Components, string>(CHANNEL_SUPERVISOR, COMPONENT_PATH + string(NEGOTIATOR_PROCESS_NAME)));
   mComponentLocation.insert(pair<Components, string>(CONNECTIVITY_AGENT, COMPONENT_PATH + string(CONNECTIVITY_AGENT_PROCESS_NAME)));
   mComponentLocation.insert(pair<Components, string>(APPLICATION_MANAGER, COMPONENT_PATH + string(APPLICATION_MANAGER_PROCESS_NAME)));
}

CComponentLauncher::~CComponentLauncher()
{
   LOG4CPLUS_TRACE(sLogger, "~CComponentLauncher()");
}

void CComponentLauncher::launchConnectivityAgent(int connectivityAgentGender)
{
   LOG4CPLUS_TRACE(sLogger, "launchConnectivityAgent("
                           + convertIntegerToString(connectivityAgentGender) + ")");

   if(isComponentRunning(CONNECTIVITY_AGENT))
   {
      shutdownComponent(CONNECTIVITY_AGENT);
   }

   launchComponent(CONNECTIVITY_AGENT, connectivityAgentGender);
}

void CComponentLauncher::launchProfileManager()
{
   LOG4CPLUS_TRACE(sLogger, "launchProfileManager()");

   if(isComponentRunning(PROFILE_MANAGER))
   {
      shutdownComponent(PROFILE_MANAGER);
   }

   launchComponent(PROFILE_MANAGER);
}

void CComponentLauncher::launchChannelSupervisor()
{
   LOG4CPLUS_TRACE(sLogger, "launchChannelSupervisor()");

   if(isComponentRunning(CHANNEL_SUPERVISOR))
   {
      shutdownComponent(CHANNEL_SUPERVISOR);
   }

   launchComponent(CHANNEL_SUPERVISOR);
}

void CComponentLauncher::launchApplicationManager()
{
   LOG4CPLUS_TRACE(sLogger, "launchApplicationManager()");

   if(isComponentRunning(APPLICATION_MANAGER))
   {
      shutdownComponent(APPLICATION_MANAGER);
   }

   launchComponent(APPLICATION_MANAGER);
}

bool CComponentLauncher::isComponentRunning(Components component)
{
   LOG4CPLUS_TRACE(sLogger, "isComponentRunning()");

   LOG4CPLUS_WARN(sLogger, "@TODO Implementation");

   return false;
}

void CComponentLauncher::shutdownComponent(Components component)
{
   // @TODO Either keep this method in Launcher or move to IPC Protocol.
}

void CComponentLauncher::launchComponent(Components component, int componentGender)
{
   LOG4CPLUS_TRACE(sLogger, "launchComponent()");
   LOG4CPLUS_TRACE(sLogger, "ComponentGender = " + convertIntegerToString(componentGender));


   char gender[] = "0";
   gender[0] = componentGender + '0';

   pid_t componentProcessPID;

   char* componentArguments[] =
   {
      const_cast<char*>(mComponentLocation.at(component).c_str()),
      gender,
      NULL
   };

   LOG4CPLUS_INFO(sLogger, "Call fork() system call ...");
   componentProcessPID = fork();

   if(-1 == componentProcessPID)
   {
	  LOG4CPLUS_ERROR(sLogger, "Error creating new process: " + string(strerror(errno)));
   }

   if(0 == componentProcessPID)
   {
	  LOG4CPLUS_INFO(sLogger, "Child PID = " + convertIntegerToString(componentProcessPID));

	  execvp(mComponentLocation.at(component).c_str(), componentArguments);

	  LOG4CPLUS_ERROR(sLogger, "Couldn't launch the component " + mComponentLocation.at(component)
							+ ": " + string(strerror(errno)));
   }
   else
   {
	  LOG4CPLUS_INFO(sLogger, "Parent PID = " + convertIntegerToString(componentProcessPID));
   }

}

void CComponentLauncher::launchComponent(Components component)
{
   LOG4CPLUS_TRACE(sLogger, "launchComponent()");

   launchComponent(component, 0);
}

void CComponentLauncher::onSIGCHLD(int signal_number)
{
   LOG4CPLUS_TRACE(sLogger, "onSIGCHLD(" + convertIntegerToString(signal_number) + ")");

   pid_t pid = -1;
   int status;

   while((pid = waitpid(-1, &status, WNOHANG)) > 0)
   {
      LOG4CPLUS_INFO(sLogger, "Child with PID " + convertIntegerToString(pid) + "exited with status "
    		                                   + convertIntegerToString((int)WEXITSTATUS(status)));
   }
}

void CComponentLauncher::setSIGCHLDHandler()
{
   LOG4CPLUS_TRACE(sLogger, "setSIGCHLDHandler()");

   struct sigaction sigchld_action;

   memset(&sigchld_action, 0, sizeof(sigchld_action));
   sigchld_action.sa_handler = CComponentLauncher::onSIGCHLD;

   if(sigaction(SIGCHLD, &sigchld_action, NULL) < 0)
   {
      LOG4CPLUS_INFO(sLogger, "ComponentLauncher::setSIGCHLDHandler() errno = " + convertIntegerToString(errno)
    		               + " " + string(strerror(errno)));

      return;
   }

   LOG4CPLUS_INFO(sLogger, "ComponentLauncher::setSIGCHLDHandler() done");
}

}
