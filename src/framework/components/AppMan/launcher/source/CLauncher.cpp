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





#include <cerrno>
#include <cstdlib>
#include <limits.h>
#include <unistd.h>

#include "framework/components/AppMan/launcher/include/CLauncher.hpp"

std::string get_dir_from_path(std::string const& path)
{
   size_t found = path.find_last_of("/\\");
   return path.substr(0, found);
}


namespace AXIS
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

      void CLauncher::init(ILauncherHandler * pHandler)
      {
         LOG4CPLUS_TRACE(sLogger, "init()");
         mpHandler = pHandler;
      }

      void CLauncher::launche(const std::string & launchInfo)
      {
         LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(sLogger, "launch info: " + launchInfo);
         char rpath[PATH_MAX] = "";
         char ipc_addr[PATH_MAX] = "";
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
               LOG4CPLUS_WARN(sLogger, "chdir failed: " + convertIntegerToString(saved_errno) + strerror(saved_errno));
            }

            // starting application
            if (execv(rpath, params) == -1)
            {
               LOG4CPLUS_ERROR(sLogger, "Exec of App failed");
               exit(1);
            }
            else
            {
               // never will be here
               LOG4CPLUS_INFO(sLogger, "Exec success");
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
      }

   }

}
