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


#include <cerrno>
#include <cstdio>
#include <unistd.h>
#include <sys/prctl.h>

#include "CComponentLauncher.hpp"

#include "BaseError.hpp"

using namespace std;

namespace iviLink
{
namespace SystemController
{

const string CComponentLauncher::COMPONENT_PATH = string(IVILINK_BIN_DIR);
const string CComponentLauncher::APPLICATION_PATH = string(IVILINK_APP_DIR);

Logger CComponentLauncher::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.CComponentLauncher"));

CComponentLauncher* CComponentLauncher::sInstance = 0;

CComponentLauncher* CComponentLauncher::getInstance()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   if(0 == sInstance)
   {
      sInstance = new CComponentLauncher();
      CComponentLauncher::setSIGCHLDHandler();
   }

   return sInstance;
}

CComponentLauncher::CComponentLauncher()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   mComponentLocation.insert(pair<Components, string>(PROFILE_MANAGER, COMPONENT_PATH + string(PROFILE_MANAGER_PROCESS_NAME)));
   mComponentLocation.insert(pair<Components, string>(CHANNEL_SUPERVISOR, COMPONENT_PATH + string(NEGOTIATOR_PROCESS_NAME)));
   mComponentLocation.insert(pair<Components, string>(CONNECTIVITY_AGENT, COMPONENT_PATH + string(CONNECTIVITY_AGENT_PROCESS_NAME)));
   mComponentLocation.insert(pair<Components, string>(APPLICATION_MANAGER, COMPONENT_PATH + string(APPLICATION_MANAGER_PROCESS_NAME)));
   mComponentLocation.insert(pair<Components, string>(SYSTEM_CONTROLLER_WATCHDOG, COMPONENT_PATH + string(SYSTEM_CONTROLLER_WATCHDOG_PROCESS_NAME)));
   LOG4CPLUS_WARN(sLogger, "Information about AuthApp location, name and availability should be taken from configuration");
   mComponentLocation.insert(pair<Components, string>(AUTHENTICATION_APP, COMPONENT_PATH + string(AUTHENTICATION_APPLICATION_PROCESS_NAME)));
   mComponentLocation.insert(pair<Components, string>(SPLASH_SCREEN, COMPONENT_PATH + string(PROGRESSBAR_PROCESS_NAME)));
}

CComponentLauncher::~CComponentLauncher()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
}

void CComponentLauncher::launchConnectivityAgent()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   if(isComponentRunning(CONNECTIVITY_AGENT))
   {
      shutdownComponent(CONNECTIVITY_AGENT);
   }

   launchComponent(CONNECTIVITY_AGENT);
}

void CComponentLauncher::launchProfileManager()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   if(isComponentRunning(PROFILE_MANAGER))
   {
      shutdownComponent(PROFILE_MANAGER);
   }

   launchComponent(PROFILE_MANAGER);
}

void CComponentLauncher::launchChannelSupervisor()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   if(isComponentRunning(CHANNEL_SUPERVISOR))
   {
      shutdownComponent(CHANNEL_SUPERVISOR);
   }

   launchComponent(CHANNEL_SUPERVISOR);
}

void CComponentLauncher::launchApplicationManager()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   if(isComponentRunning(APPLICATION_MANAGER))
   {
      shutdownComponent(APPLICATION_MANAGER);
   }

   launchComponent(APPLICATION_MANAGER);
}

void CComponentLauncher::launchAuthenticationApp()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   if(isComponentRunning(AUTHENTICATION_APP))
   {
      shutdownComponent(AUTHENTICATION_APP);
   }

   launchComponent(AUTHENTICATION_APP);
}

void CComponentLauncher::launchSplashScreen()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   if(isComponentRunning(SPLASH_SCREEN))
   {
      shutdownComponent(SPLASH_SCREEN);
   }

   launchComponent(SPLASH_SCREEN);
}

void CComponentLauncher::launchSystemControllerWatchdog()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   if(isComponentRunning(SYSTEM_CONTROLLER_WATCHDOG))\
   {
      shutdownComponent(SYSTEM_CONTROLLER_WATCHDOG);
   }
   launchComponent(SYSTEM_CONTROLLER_WATCHDOG);
}


void CComponentLauncher::shutdownAllComponents()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   shutdownComponent(SPLASH_SCREEN);
   shutdownComponent(SYSTEM_CONTROLLER_WATCHDOG);
   shutdownComponent(AUTHENTICATION_APP);
   shutdownComponent(APPLICATION_MANAGER);
   shutdownComponent(CONNECTIVITY_AGENT);
   shutdownComponent(CHANNEL_SUPERVISOR);
   shutdownComponent(PROFILE_MANAGER);   
}

bool CComponentLauncher::isComponentRunning(Components component)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   //mComponentPidCondVar.lock();
   bool found = mComponentPid.end() != mComponentPid.find(component);
   //mComponentPidCondVar.unlock();
   return found;
}

void CComponentLauncher::shutdownComponent(Components component)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, "shutdownComponent() component "  + convertIntegerToString((int)component));

   // @TODO Either keep this method in Launcher or move to IPC Protocol.

  // mComponentPidCondVar.lock();
   
   bool killed = true;
   {
      tComponentPid::const_iterator it = mComponentPid.find(component);
      if (it != mComponentPid.end())
      {
         pid_t pid = it->second;
         LOG4CPLUS_INFO(sLogger, "Killing component " + convertIntegerToString((int)component)
            + " with pid " + convertIntegerToString(pid));

         // killing component and all its subprocesses
         if (-1 == kill(-pid, SIGTERM))
         {
            LOG4CPLUS_WARN(sLogger, "Unable to kill component group: " + BaseError::FormErrnoDescr(errno));
            killed = false;
         }
      }
   }

   if (!killed)
   {
      //mComponentPidCondVar.unlock();
      return;
   }

   //while (mComponentPid.find(component) != mComponentPid.end())
   //   mComponentPidCondVar.wait();

   //mComponentPidCondVar.unlock();

   LOG4CPLUS_INFO(sLogger, "Component killed");
}

pid_t CComponentLauncher::launchComponent(Components component)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   pid_t componentProcessPID;

   char* componentArguments[] =
   {
      const_cast<char*>(mComponentLocation.at(component).c_str()),
      NULL
   };

   LOG4CPLUS_INFO(sLogger, "Call fork() system call ...");
   componentProcessPID = fork();

   if(-1 == componentProcessPID)
   {
      LOG4CPLUS_ERROR(sLogger, "Error creating new process: " + string(strerror(errno)));
   }
   else if(0 == componentProcessPID)
   {
      setpgid(0, 0);
      prctl(PR_SET_PDEATHSIG,SIGTERM);

      execvp(mComponentLocation.at(component).c_str(), componentArguments);

      fputs(("Couldn't launch the component " + mComponentLocation.at(component)
         + ": " + string(strerror(errno))).c_str(),
         stderr);
      exit(1);
   }
   else
   {
      setpgid(componentProcessPID, componentProcessPID);

      LOG4CPLUS_INFO(sLogger, "Parent PID = " + convertIntegerToString(componentProcessPID));

      if(component != AUTHENTICATION_APP)
      {
    	  LOG4CPLUS_TRACE(Logger::getInstance(LOG4CPLUS_TEXT("profiler.ComponentIsStarted")),
    			  mComponentLocation.at(component).substr(mComponentLocation.at(component).rfind("/") + 1));
      }

      //mComponentPidCondVar.lock();
      mComponentPid[component] = componentProcessPID;
      //mComponentPidCondVar.unlock();
   }

   return componentProcessPID;
}

void CComponentLauncher::onSIGCHLD(int signal_number)
{
   LOG4CPLUS_TRACE(sLogger, "onSIGCHLD(" + convertIntegerToString(signal_number) + ")");

   pid_t pid = -1;
   int status;

   while((pid = waitpid(-1, &status, WNOHANG)) > 0)
   {
      LOG4CPLUS_INFO(sLogger, "Child with PID " + convertIntegerToString(pid) + " exited with status "
    		                                   + convertIntegerToString((int)WEXITSTATUS(status)));

      if (sInstance)
      {
         //sInstance->mComponentPidCondVar.lock();
         for (tComponentPid::iterator it = sInstance->mComponentPid.begin(); it != sInstance->mComponentPid.end(); ++it)
         {
            if (it->second == pid)
            {
               sInstance->mComponentPid.erase(it);
               //sInstance->mComponentPidCondVar.signal();
               break;
            }
         }
         //sInstance->mComponentPidCondVar.unlock();
      }
   }
}

void CComponentLauncher::setSIGCHLDHandler()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   struct sigaction sigchld_action;

   memset(&sigchld_action, 0, sizeof(sigchld_action));
   sigchld_action.sa_handler = CComponentLauncher::onSIGCHLD;

   if(sigaction(SIGCHLD, &sigchld_action, NULL) < 0)
   {
      LOG4CPLUS_INFO(sLogger, "ComponentLauncher::setSIGCHLDHandler() errno = " + convertIntegerToString(errno)
    		               + " " + string(strerror(errno)));

      return;
   }
}

}
}
