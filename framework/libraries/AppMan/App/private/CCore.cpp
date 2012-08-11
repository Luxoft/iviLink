/* 
 * 
 * iviLINK SDK, version 1.0.1
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









#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>

#include "CCore.hpp"

namespace iviLink
{

   namespace AppMan
   {

      namespace App
      {
         Logger CCore::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.App.Core"));

         CCore::CCore()
            : mpHandler(0)
            , mpProto(0)
         {
            LOG4CPLUS_TRACE(msLogger,"CCore()");
            mPid = getpid();
            getExeName();
         }

         CCore::~CCore()
         {
            LOG4CPLUS_TRACE(msLogger,"~CCore()");
         }

         void CCore::initHandler(IAppManHandler * pHandler)
         {
            LOG4CPLUS_TRACE(msLogger,"initHandler()");
            mpHandler = pHandler;
         }

         void CCore::initIpc(iviLink::AppMan::Ipc::IAppManProto * pProto)
         {
            LOG4CPLUS_TRACE(msLogger,"initIpc()");
            mpProto = pProto;
         }

         void CCore::unInit()
         {
            LOG4CPLUS_TRACE(msLogger,"unInit()");
            mpHandler = 0;
            mpProto = 0;
         }

         EInitResult  CCore::initApplication(std::list<Service::Uid> listOfSupportedServices)
         {
            LOG4CPLUS_TRACE(msLogger,"initApplication()");
            if (!mpProto)
            {
               return EInitResult(ERROR_NOT_INITED);
            }
            return mpProto->initApplication(mPid, listOfSupportedServices);
         }

         CError CCore::useService(Service::Uid service, bool use)
         {
            LOG4CPLUS_TRACE(msLogger,"useService()");
            if (!mpProto)
            {
               return CError(CError::ErrorCode(1),"AppMan",CError::ERROR,"AppManCore hasn't been inited");
            }
            return mpProto->useService(mPid, service, use);
         }

         CError CCore::registerService(Service::Uid service)
         {
            LOG4CPLUS_TRACE(msLogger,"registerService()");
            if (!mpProto)
            {
               return CError(CError::ErrorCode(1),"AppMan",CError::ERROR,"AppManCore hasn't been inited");
            }
            return mpProto->registerService(mPid, service);
         }

         CError CCore::sessionRequest(pid_t pid, iviLink::Service::SessionUid session,
               iviLink::Service::Uid service)
         {
            LOG4CPLUS_TRACE(msLogger,"sessionRequest()");
            assert(mPid == pid);
            if (!mpHandler)
            {
               return CError(CError::ErrorCode(1),"AppMan",CError::ERROR,"AppManCore hasn't been inited");
            }
            return mpHandler->sessionRequest(session, service);
         }

         CError CCore::getAppLaunchInfo(pid_t pid, std::string & launchInfo)
         {
            LOG4CPLUS_TRACE(msLogger,"getAppLaunchInfo() :: pid: " + convertIntegerToString(pid));
            assert(mPid == pid);
            launchInfo = mLaunchInfo;
            return CError::NoError("AppMan","No Error in CCore::getAppLaunchInfo()");
         }

         void CCore::getExeName()
         {
            LOG4CPLUS_TRACE(msLogger,"getExeName()");
            char buf[1001];
            size_t size = 1000;
            char linkname[256]; /* /proc/<pid>/exe */

            if (snprintf(linkname, sizeof(linkname), "/proc/%i/exe", mPid) < 0)
            {
               return;
            }

            // read the symbolic link
            int ret = readlink(linkname, buf, size);

            //error
            if (ret == -1)
            {
               return;
            }

            /* Report insufficient buffer size */
            if (ret >= static_cast<int>(size))
            {
            return;
            }
            buf[ret] = 0;
            mLaunchInfo = buf;
            LOG4CPLUS_INFO(msLogger, "Launch info: " + mLaunchInfo);
         }

      }

   }

}
