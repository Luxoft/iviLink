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
#include <signal.h>
#include <string>
#include <unistd.h>

#include "utils/misc/Logger.hpp"
#include "utils/misc/CError.hpp"
#include "reset.hpp"

#include "framework/components/SystemController/componentLauncher/CComponentLauncher.hpp"

using SystemController::CComponentLauncher;

static char const* gsArgv0 = 0;

static Logger gsLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.main"));

int destroy(pid_t pid);

void onSIGTERM(int sig)
{
   hardStop(true);
}

void setSIGTERMHandler()
{
   LOG4CPLUS_TRACE(gsLogger, "setSIGCHLDHandler()");

   struct sigaction action;

   action.sa_handler = onSIGTERM;
   sigemptyset(&action.sa_mask);
   action.sa_flags = 0;

   if (sigaction(SIGTERM, &action, NULL) < 0)
   {
      LOG4CPLUS_INFO(gsLogger, "setSIGTERMHandler() sigaction " + CError::FormErrnoDescr(errno));
   }

}

void setupReset(char const* argv0)
{
   gsArgv0 = argv0;
   setSIGTERMHandler();
}

void stopComponentns()
{
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

   if (internal_reset)
   {
      stopComponentns();
   }

   pid_t pid;
   pid_t sid;

   pid = fork();
   if (pid < 0)
   {
      LOG4CPLUS_FATAL(gsLogger, "fork error: " + CError::FormErrnoDescr(errno));
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

   sid = setsid();
   if (sid < 0)
   {
      fputs(("setsid error: " + CError::FormErrnoDescr(errno)).c_str(),
         stderr);
      exit(1);
   }

   execl(gsArgv0, gsArgv0, NULL);
   
   fputs(("execl error: " + CError::FormErrnoDescr(errno)).c_str(),
      stderr);
   exit(1);
}

int destroy(pid_t pid)
{
   usleep(500000);
   
   // -pid for killing all children with process
   if (-1 == kill(-pid, SIGTERM))
   {
      int se = errno;
      LOG4CPLUS_ERROR(gsLogger, "kill of pid " + convertIntegerToString(pid) 
         + " failed, " + CError::FormErrnoDescr(se));
      return 10;
   }

   return 0;
}

int search_and_destroy(std::string const name)
{
   FILE *fp;
   int status;

   if (name.empty())
   {
      LOG4CPLUS_ERROR(gsLogger, "Empty file name");
      return 1;
   }

   struct sigaction saved;
   struct sigaction def;
   def.sa_handler = SIG_DFL;
   sigemptyset(&def.sa_mask);
   def.sa_flags = 0;

   if (-1 == sigaction(SIGCHLD, &def, &saved))
   {
      int se = errno;
      LOG4CPLUS_ERROR(gsLogger, "sigaction failed " + CError::FormErrnoDescr(se));
      return 2;
   }


   fp = popen(("pidof " + name).c_str(), "r");
   if (fp == NULL)
   {
      int se = errno;
      LOG4CPLUS_ERROR(gsLogger, "popen failed " + CError::FormErrnoDescr(se));
      return 2;
   }

   const pid_t self = getpid();

   int pid;
   while (fscanf(fp, "%u", &pid) != EOF)
   {
      LOG4CPLUS_TRACE(gsLogger, "got pid " + convertIntegerToString(pid));
      if (self != pid)
      {
         destroy(pid);
      }
      else
      {
         LOG4CPLUS_TRACE(gsLogger, "pid is self");
      }
   }

   status = pclose(fp);
   if (status == -1) 
   {
      int se = errno;
      LOG4CPLUS_ERROR(gsLogger, "pclose failed " + CError::FormErrnoDescr(se));
      return 3;
   }

   if (-1 == sigaction(SIGCHLD, &saved, NULL))
   {
      int se = errno;
      LOG4CPLUS_ERROR(gsLogger, "sigaction failed " + CError::FormErrnoDescr(se));
      return 2;
   }

   return 0;
}

std::string name_from_path(std::string const& path)
{
   size_t pos = path.find_last_of("\\/");
   if (std::string::npos == pos)
      return path;
   else
      return std::string(path.begin() + pos + 1, path.end());
}

int hardStop(bool internal_stop)
{
   LOG4CPLUS_TRACE_METHOD(gsLogger, __PRETTY_FUNCTION__);
   if (internal_stop)
   {
      stopComponentns();

      destroy(getpid());
      exit(0);
   }

   return search_and_destroy(name_from_path(gsArgv0));
}
