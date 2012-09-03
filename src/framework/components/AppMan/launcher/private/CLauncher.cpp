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
#include <limits.h>
#include <unistd.h>

#include "CLauncher.hpp"

std::string get_dir_from_path(std::string const& path)
{
   size_t found = path.find_last_of("/\\");
   return path.substr(0, found);
}


namespace iviLink
{

   namespace AMP
   {
      Logger CLauncher::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Launcher.CLauncher"));

      CLauncher::CLauncher()
         : mpHandler(0)
      {
         LOG4CPLUS_TRACE(sLogger, "CLauncher()");
      }

      CLauncher::~CLauncher()
      {
         LOG4CPLUS_TRACE(sLogger, "~CLauncher()");
      }

      #ifndef ANDROID
      void CLauncher::init(ILauncherHandler * pHandler)
      #else
      void CLauncher::init(ILauncherHandler * pHandler, JavaVM * jm, jclass launchClass, jmethodID launchMethod)
      #endif
      {
         LOG4CPLUS_TRACE(sLogger, "init()");
         mpHandler = pHandler;
         #ifndef ANDROID
         #else
         pJm = jm;
         jLauncher = launchClass;
         jLaunchMethod = launchMethod;
         #endif //ANDROID
      }

      void CLauncher::launche(const std::string & launchInfo)
      {
         LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(sLogger, "launch info: " + launchInfo);
         #ifndef ANDROID
         char rpath[PATH_MAX] = "";
         char * params[] = { rpath, 0, 0, 0 };

         std::string prefixed_path(PREFIX);
         prefixed_path += launchInfo;

         LOG4CPLUS_INFO(sLogger, "prefixed_path: " + prefixed_path);

         if (realpath(prefixed_path.c_str(), rpath) != rpath)
         {
            LOG4CPLUS_ERROR(sLogger, "realpath failed " + convertIntegerToString(errno) + std::string(strerror(errno)));
            return;
         }

         if (rpath != NULL)
         {
            LOG4CPLUS_INFO(sLogger, "rpath: "+ std::string(rpath));
         }

         std::string dir = get_dir_from_path(rpath);

         pid_t pid = fork();
         switch (pid)
         {
         case -1:
            LOG4CPLUS_ERROR(sLogger, "fork returned -1");
            if (mpHandler)
            {
               mpHandler->launchError(launchInfo);
            }
            return;
            break;
         case 0:
            // changing current dir, so application will not be confused
            if (-1 == chdir(dir.c_str()))
            {
               int saved_errno = errno;
               fputs(("chdir failed: " + convertIntegerToString(saved_errno) + strerror(saved_errno)).c_str(),
                  stderr);
               exit(1);
            }

            // starting application
            execv(rpath, params);
            {
               int saved_errno = errno;
               fputs(("execv failed: " + convertIntegerToString(saved_errno) + strerror(saved_errno)).c_str(),
                  stderr);
               exit(1);
            }

            break;
         default:
            LOG4CPLUS_INFO(sLogger, "app launched");
            if (mpHandler)
            {
               LOG4CPLUS_INFO(sLogger, "say handler : app launched");
               mpHandler->launchedApp(launchInfo,pid);
            }
            break;
         }
         #else 
         JNIEnv *env;
         iviLink::Android::JniThreadHelper jth(pJm);
         env = jth.getEnv();
	      jstring param = env->NewStringUTF(launchInfo.c_str());
	      pid_t pid=(pid_t)(env->CallStaticIntMethod(jLauncher, jLaunchMethod, param)); // int Launcher.startApplication(String) is invoked
         if(pid == -1)
         {
            LOG4CPLUS_FATAL(sLogger, "app not launched: "+launchInfo);
            return; // should I do exit(1) too?
         }	      
         LOG4CPLUS_INFO(sLogger, "app launched: ");
	      LOG4CPLUS_INFO(sLogger, "        launchInfo: "+launchInfo);
	      LOG4CPLUS_INFO(sLogger, "                 pid: "+convertIntegerToString(pid));
	      if (mpHandler)
	      {
	         LOG4CPLUS_INFO(sLogger, "make callback: mpHandler->launchedApp(launchinfo, pid)");
            mpHandler->launchedApp(launchInfo,pid);
         }
         #endif // ANDROID
      }
      
      #ifndef ANDROID
      #else
      void CLauncher::uninit()
      {
         mpHandler = 0;
         pJm = 0;
         jLauncher = 0;
         jLaunchMethod = 0;
      }
      #endif //ANDROID

   } // AMP

} // iviLink
