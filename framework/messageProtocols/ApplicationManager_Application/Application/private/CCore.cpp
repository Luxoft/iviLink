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


#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>

#include <tr1/functional>
typedef std::tr1::function< void ()> notify_t;


#include "CCore.hpp"
#include "CAppManProxy.hpp"

namespace iviLink
{

   namespace AppMan
   {

      namespace App
      {
         Logger CCore::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.App.Core"));

         CCore::CCore(Android::AppInfo appInfo)
            : mpHandler(0)
            , mpProto(0)
            , mAMProxy(NULL)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mPid = getpid();
#ifndef ANDROID
            getExeName();
#else
            mLaunchInfo = appInfo.launchInfo;
#endif //ANDROID            
         }

         CCore::~CCore()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         }

         void CCore::initHandler(IAppManHandler * pHandler)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpHandler = pHandler;
         }

         void CCore::initIpc(iviLink::AppMan::Ipc::IAppManProto * pProto)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpProto = pProto;
         }

         void CCore::unInit()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpHandler = 0;
            mpProto = 0;
         }

         EInitResult  CCore::initApplication(std::list<Service::Uid> listOfSupportedServices)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            if (!mpProto)
            {
               LOG4CPLUS_WARN(msLogger, "ERROR_NOT_INITED");
               return EInitResult(ERROR_NOT_INITED);
            }
            return mpProto->initApplication(mPid, listOfSupportedServices);
         }

         BaseError CCore::useService(Service::Uid service, bool use)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            if (!mpProto)
            {
               return BaseError(BaseError::ErrorCode(1),"AppMan",BaseError::IVILINK_ERROR,"AppManCore hasn't been inited");
            }
            return mpProto->useService(mPid, service, use);
         }

         BaseError CCore::registerService(Service::Uid service)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            if (!mpProto)
            {
               return BaseError(BaseError::ErrorCode(1),"AppMan",BaseError::IVILINK_ERROR,"AppManCore hasn't been inited");
            }
            return mpProto->registerService(mPid, service);
         }

         BaseError CCore::sessionRequest(pid_t pid, const iviLink::Service::SessionUid & session,
               const iviLink::Service::Uid & service)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            assert(mPid == pid);
            if (!mpHandler)
            {
               return BaseError(BaseError::ErrorCode(1),"AppMan",BaseError::IVILINK_ERROR,"AppManCore hasn't been inited");
            }
            return mpHandler->sessionRequest(session, service);
         }

         BaseError CCore::waitingServiceRequest(pid_t pid, const iviLink::Service::Uid & service)
         {
             if (mAMProxy)
             {
                 mAMProxy->onLoadWaitingService(service);
                 return BaseError::NoError("AppMan");
             }
             else
             {
                 return BaseError(1, "AppMan", BaseError::IVILINK_ERROR, "Has no pointer on AM Proxy");
             }
         }

         BaseError CCore::getAppLaunchInfo(pid_t pid, std::string & launchInfo)
         {
            LOG4CPLUS_TRACE(msLogger,"getAppLaunchInfo() :: pid: " + convertIntegerToString(pid));
            assert(mPid == pid);
            launchInfo = mLaunchInfo;
            return BaseError::NoError("AppMan","No Error in CCore::getAppLaunchInfo()");
         }

          bool CCore::isLinkAlive()
          {
              assert( mpProto );
              if( mpProto )
                  return mpProto -> isLinkAlive();
              return false;
          }

          void CCore::onLinkUpNotify()
          {
              if( link_up )
                  link_up();
          }

          void CCore::onLinkDownNotify()
          {
              if( link_down )
                  link_down();
          }

          void CCore::setLinkCallbacks( notify_t link_up_, notify_t link_down_ )
          {
              link_up = link_up_;
              link_down = link_down_;
          }


#ifndef ANDROID
         void CCore::getExeName()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
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
#endif //ANDROID

         void CCore::registerAMProxy(iviLink::App::CAppManProxy * amProxy)
         {
             mAMProxy = amProxy;
         }

      }

   }

}
