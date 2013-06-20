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


#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <vector>

#include "Logger.hpp"
#include "BaseError.hpp"
#include "reset.hpp"

#include "CComponentLauncher.hpp"

using iviLink::SystemController::CComponentLauncher;

static char const* gsArgv0 = 0;

static Logger gsLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.main"));

int destroy(pid_t pid);

void onSIGTERM(int sig)
{
   hardStop(true);
}

void setSIGTERMHandler()
{
   LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);

   struct sigaction action;

   action.sa_handler = onSIGTERM;
   sigemptyset(&action.sa_mask);
   action.sa_flags = 0;

   if (sigaction(SIGTERM, &action, NULL) < 0)
   {
      LOG4CPLUS_INFO(gsLogger, "setSIGTERMHandler() sigaction " + BaseError::FormErrnoDescr(errno));
   }

}

void setupReset(char const* argv0)
{
   gsArgv0 = argv0;
   setSIGTERMHandler();
}

void stopComponents()
{
   LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
   CComponentLauncher* pcl = CComponentLauncher::getInstance();
   pcl->shutdownAllComponents();
}

void hardReset(bool internal_reset)
{
   LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);

   if (0 != hardStop(false))
   {
      LOG4CPLUS_FATAL(gsLogger, "Stop failed, exiting.");
      exit(1); 
   }

   pid_t pid;
   pid_t sid;

   pid = fork();
   if (pid < 0)
   {
      LOG4CPLUS_FATAL(gsLogger, "fork error: " + BaseError::FormErrnoDescr(errno));
      exit(1);
   }
   if (pid > 0)
   {
      // parent

      LOG4CPLUS_INFO(gsLogger, "fork successful, child = " +
         convertIntegerToString(pid));

      exit(0);
   }

   // child
   // function shall create a new session, if the calling process is not a process group leader
   sid = setsid();   
   if (sid < 0)
   {
      fputs(("setsid error: " + BaseError::FormErrnoDescr(errno)).c_str(),
         stderr);
      exit(1);
   }

   execl(gsArgv0, gsArgv0, NULL);
   
   fputs(("execl error: " + BaseError::FormErrnoDescr(errno)).c_str(),
      stderr);

   exit(1);
}

int destroy(pid_t pid)
{
   //usleep(500000);
   
   // -pid for killing all children with process
   if (-1 == kill(-pid, SIGTERM))
   {
      int se = errno;
      LOG4CPLUS_ERROR(gsLogger, "kill of pid " + convertIntegerToString(pid) 
         + " failed, " + BaseError::FormErrnoDescr(se));
      return 10;
   }

   return 0;
}

std::string name_from_path(std::string const& path)
{
    size_t pos = path.find_last_of("\\/");
    if (pos == std::string::npos)
    {
        return path;
    }
    else
    {
        return std::string(path.begin() + pos + 1, path.end());
    }
}

int filterProc(const struct dirent* entry)
{
    if (entry->d_type & DT_DIR)
    {
        if (std::string(entry->d_name).find_first_not_of("0123456789") == std::string::npos)
        {
            return 1;
        }
    }
    return 0;
}

tPidVector pidof(std::string const& name)
{
    tPidVector pids;
    struct dirent ** entries=NULL;
    int count = 0;
    pids.clear();

    count = scandir("/proc/", &entries, filterProc, alphasort);
    if (count < 0)
    {
        return pids;
    }

    while (count--)
    {
        char processExecutablePath[PATH_MAX];
        std::string processExecutableLink = "/proc/" + std::string(entries[count]->d_name) + "/exe";
        memset(processExecutablePath, 0, sizeof(processExecutablePath));
        if (readlink(processExecutableLink.c_str(), processExecutablePath, sizeof(processExecutablePath)) >= 0)
        {
            std::string path = name_from_path(processExecutablePath);
            if (path == name) 
            {
                pids.push_back(atoi(entries[count]->d_name));
            }
        }
        free(entries[count]);
    }
    free(entries);
    return pids;
}


int search_and_destroy(std::string const& name)
{
   if (name.empty())
   {
      LOG4CPLUS_ERROR(gsLogger, "Empty file name");
      return 1;
   }

   tPidVector pids = pidof(name);

   const pid_t self = getpid();

   for (tPidIterator pid = pids.begin(); pid != pids.end(); ++pid)
   {
      LOG4CPLUS_TRACE(gsLogger, "got pid " + convertIntegerToString(*pid));
      if (self != *pid)
      {
         destroy(*pid);
      }
      else
      {
         LOG4CPLUS_TRACE(gsLogger, "pid is self");
      }
   }

   return 0;
}

int hardStop(bool internal_stop)
{
   LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
   if (internal_stop)
   {
      stopComponents();

      destroy(getpid());      
      exit(0);
   }
   else
   {
      shutdownIviProcesses();
   }

   return search_and_destroy(name_from_path(gsArgv0));
}

int getQuantityOfRunningIVILink()
{
   LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
   int count = 0;

   tPidVector pids = pidof(SYSTEM_CONTROLLER_PROCESS_NAME);

   count = pids.size();

   LOG4CPLUS_INFO(gsLogger, "number of running iviLink = " +
         convertIntegerToString(count));
   return count;
}

bool shutdownIviProcess(const char *proc_name)
{   
   LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
   bool killed = true;
   
   tPidVector pids = pidof(proc_name);

   for (tPidIterator pid = pids.begin(); pid != pids.end(); ++pid)
   {
         if (-1 == kill(-(*pid), SIGTERM))
         {
            LOG4CPLUS_WARN(gsLogger, "Failed to kill " + std::string(proc_name) + BaseError::FormErrnoDescr(errno));
            killed = false;
         }
         else
         {
            LOG4CPLUS_INFO(gsLogger, "Killed process " + std::string(proc_name) + " with pid = " + convertIntegerToString(*pid));
            killed = true;
         }
   }
   return killed;
}

void shutdownIviProcesses()
{
   LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
   std::vector<char*> viviProc;
   viviProc.push_back("IVILinkProgressBar");
   viviProc.push_back("IVILinkSystemControllerWatchdog");
   viviProc.push_back("IVILinkConnectivityAgent");
   viviProc.push_back("IVILinkNegotiator");
   viviProc.push_back("IVILinkProfileManager");
   viviProc.push_back("IVILinkProfileRepository");
   viviProc.push_back("IVILinkApplicationManager");
   viviProc.push_back("AuthenticationApplication");
   viviProc.push_back("ScreenSharing");
   viviProc.push_back("Climate_App");
   viviProc.push_back("Seat_App");
   viviProc.push_back("Navi");

   for (int i = 0; i != viviProc.size(); i++)
   {
      shutdownIviProcess(viviProc[i]);
   }

}
