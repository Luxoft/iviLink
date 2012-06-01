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

/***************************************************************************
 * Project           AXIS
 * (c) copyright     2012
 * Company           LUXOFT
 * @file             CPmpComponentManager.cpp
 * @brief            Source file for CPmpComponentManager class (PMP Component manager library)
 * @author           VPlachkov
 ***************************************************************************/

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "utils/configurator/include/configurator.h"

#include "framework/components/ProfileManager/PMP/componentManager/include/CPmpComponentManager.hpp"
#include "framework/components/ProfileManager/PMP/core/include/CPmpCore.hpp"
#include "framework/components/ProfileManager/PMP/PIM/include/CPmpPim.hpp"
#include "framework/components/ProfileManager/PMP/interaction/include/CPmpProtoController.hpp"
#include "framework/components/ProfileManager/PMP/interaction/include/CPmpCoreProtocol.hpp"
#include "framework/components/ProfileManager/PMP/interaction/include/CPmpPimProtocol.hpp"
#include "framework/components/ProfileManager/PMP/repository/include/CRepoController.hpp"
#include "framework/components/ProfileManager/PMP/ipc_protocol/include/CIpcProtocol.hpp"
#include "framework/libraries/AppMan/Pmp/include/CAppManPmpController.hpp"

namespace AXIS
{

   namespace PMP
   {
      Logger CPmpComponentManager::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.CPmpComponentManager"));

      CPmpComponentManager::CPmpComponentManager(conf::Configurator * pConfig/* = NULL*/)
         : mpPmpProtoController(0)
         , mRepoPid(-1)
         , mpCoreProtocol(0)
         , mpCore(0)
         , mpPimProtocol(0)
         , mpPim(0)
         , mpIpcProtocol(0)
         , mpConfig(pConfig)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (!initProfileRepository())
         {
            LOG4CPLUS_FATAL(msLogger, "Error while initializing Profile Repository");
            exit(1);
         }
         initPmp();
         assert(mpPmpProtoController);
         assert(mpCoreProtocol);
         assert(mpCore);
         assert(mpPimProtocol);
         assert(mpPim);
         assert(mpIpcProtocol);
      }

      CPmpComponentManager::~CPmpComponentManager()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         uninitPmp();
         killProfileRepository();
      }

      bool CPmpComponentManager::initProfileRepository()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         char rpath[PATH_MAX] = "";
         char ipc_addr[PATH_MAX] = "";
         char * params[] = { rpath, 0, 0, 0 };

         std::string repoPath;
#if defined(NO_SYSTEM_CONTROLLER_LAUNCH)
         repoPath += "../../../ProfileRepository/process/ProfileRepository";
#else
         repoPath += std::string(PREFIX) + "/" + PROFILE_REPOSITORY_PROCESS_NAME;
#endif // NO_SYSTEM_CONTROLLER_LAUNCH

         LOG4CPLUS_INFO(msLogger, "repoPath = " + repoPath);

         if (realpath(repoPath.c_str(), rpath) != rpath)
         {
            LOG4CPLUS_INFO(msLogger, "realpath failed " + convertIntegerToString(errno) + " " + std::string(strerror(errno)));
            return false;
         }
            


         mRepoPid = fork();
         switch (mRepoPid)
         {
         case -1:
            LOG4CPLUS_WARN(msLogger, "fork returned -1");
            return false;
            break;
         case 0:
            LOG4CPLUS_ERROR(msLogger, "launching '" + std::string(rpath) + "'");


            if (mpConfig)
            {
               std::string addr = mpConfig->getParam("pmp_repo_ipc_address");
               if (!addr.empty())
               {
                  params[1] = const_cast<char*>("-i");
                  strcat(ipc_addr, addr.c_str());
                  params[2] = ipc_addr;
               }
            }

            if (execv(rpath, params) == -1)
            {
               LOG4CPLUS_FATAL(msLogger, "Exec of ProfileRepository failed");
               exit(1);
            }
            break;
         default:
            LOG4CPLUS_INFO(msLogger, "mRepoPid = " + convertIntegerToString(static_cast<int>(mRepoPid)));
            break;
         }
         return true;
      }

      /**
       * function itoa isn't in C++ standard, so next two functions (itoa and reverse) were copied from http://en.wikipedia.org/wiki/Itoa
       */
       /* reverse:  reverse string s in place */
       void reverse(char s[])
       {
           int i, j;
           char c;

           for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
               c = s[i];
               s[i] = s[j];
               s[j] = c;
           }
       }

       /* itoa:  convert n to characters in s */
       void itoa(int n, char s[])
       {
           int i, sign;

           if ((sign = n) < 0)  /* record sign */
               n = -n;          /* make n positive */
           i = 0;
           do {       /* generate digits in reverse order */
               s[i++] = n % 10 + '0';   /* get next digit */
           } while ((n /= 10) > 0);     /* delete it */
           if (sign < 0)
               s[i++] = '-';
           s[i] = '\0';
           reverse(s);
       }

      void CPmpComponentManager::killProfileRepository()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (kill(mRepoPid, SIGKILL) != 0)
         {
            char * str = strerror(errno);
            if (str)
            {
               LOG4CPLUS_WARN(msLogger, "kill error " + convertIntegerToString(errno) + std::string(str));
            }
            else
            {
               LOG4CPLUS_WARN(msLogger, "kill error");
            }
         }

         pid_t res = waitpid(mRepoPid, NULL, 0);
         LOG4CPLUS_INFO(msLogger, "waitpid returned " + convertIntegerToString(static_cast<int>(res)));
      }

      void CPmpComponentManager::initPmp()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         mpPmpProtoController = new CPmpProtoController;
         mpPmpProtoController->start();

         mpCoreProtocol = new CPmpCoreProtocol(mpPmpProtoController->protocol());
         mpPimProtocol = new CPmpPimProtocol(mpPmpProtoController->protocol());
         for (int i = 0; i<20; ++i)
         {
            // it must be the first time the CRepoController::instance called
            if (CRepoController::instance(mpConfig)->checkConnection())
            {
               break;
            }
            usleep(250000);
         }
         if (!CRepoController::instance()->checkConnection())
         {
            killProfileRepository();
            LOG4CPLUS_FATAL(msLogger, "Error while initializing Profile Repository Client");
            exit(1);
         }
         std::string platform = "Ubuntu"; ///todo: change way of setting name of current platform
         LOG4CPLUS_INFO(msLogger, "todo: change way of setting name of current platform");
         mpCore = new CPmpCore(CRepoController::instance()->repository(), platform);
         mpPim = new CPmpPim(mpPimProtocol);

         std::string ipcAddr;
         if (mpConfig)
            ipcAddr = mpConfig->getParam("pmp_ipc_address");

         mpIpcProtocol = new PMP::Ipc::CIpcProtocol(mpPim, mpCore, ipcAddr.empty() ? NULL : ipcAddr.c_str() );

         mpPim->setIpc(mpIpcProtocol);

         mpCore->init(mpPim,static_cast<IPmpCoreProtocol*>(mpCoreProtocol));

         mpPmpProtoController->protocol()->registerCoreClb(mpCore);
         mpPmpProtoController->protocol()->registerPimClb(mpPim);

         mpPmpProtoController->protocol()->ready();
         while (!mpPmpProtoController->protocol()->isReady())
         {
            LOG4CPLUS_INFO(msLogger, "Ready iteration");
            usleep(125000);
         }
         LOG4CPLUS_INFO(msLogger, "Ready");
         mpCore->resetProfilesState();
         mpIpcProtocol->connect();

         mpPim->setCore(mpCore);
         initApplicationManagerHandler(mpPim);
      }

      void CPmpComponentManager::uninitPmp()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         AXIS::AppMan::Ipc::CAppManPmpController::instance()->uninit();

         mpPmpProtoController->stop();
         mpCore->init(0,0);

         delete mpIpcProtocol;
         delete mpCore;
         delete mpCoreProtocol;
         delete mpPimProtocol;
         delete mpPim;
         delete mpPmpProtoController;
      }

      CPmpComponentManager * CPmpComponentManager::instance(conf::Configurator * pConfig/* = NULL*/)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         static CPmpComponentManager sInstance(pConfig);
         return &sInstance;
      }

      AXIS::AppMan::IPmpRequest * CPmpComponentManager::applicationManager()
      {
         return AXIS::AppMan::Ipc::CAppManPmpController::instance()->pmpRequest();
      }

      void CPmpComponentManager::initApplicationManagerHandler(AXIS::AppMan::IPmpHandler * pHandler)
      {
         AXIS::AppMan::Ipc::CAppManPmpController::instance()->init(pHandler);
      }

   }

}
