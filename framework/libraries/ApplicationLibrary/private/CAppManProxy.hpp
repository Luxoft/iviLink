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


#ifndef IVI_LINK_APP_CCORE_HPP_
#define IVI_LINK_APP_CCORE_HPP_


#include "Application.hpp"
#include "IAppManHandler.hpp"
#include "CThread.hpp"
#include "CSignalSemaphore.hpp"

#include "IAppMan.hpp"

class CMutex;

namespace iviLink
{
   namespace App
   {

      enum EInitStatus
      {
         NOT_INITED,
         INITED,
         HAS_INIT_REQUEST
      };

      typedef std::list<std::pair<Service::Uid, bool> > tUseServiceList;

      class CAppManProxy : public CThread
      {
      public:
         CAppManProxy(Application * pApp, Android::AppInfo appInfo);

         virtual ~CAppManProxy();

         void finish();
         void init(const Service::ListOfUids &services);
         void useService(const Service::Uid& service, bool use);
         ELaunchInfo launcher() const;

         bool isLinkAlive();
         void startProxyThread();

         void onLoadWaitingService(const Service::Uid & service);

      private:
         // from CThread
         virtual void threadFunc();

         void handleInit();
         void handleUseService();
         bool hasRequests();

         Application * mpApp;
         EInitStatus mInitStatus;
         Service::ListOfUids mInitedServices;
         CMutex * mpInitMutex;
         bool mBe;
         ELaunchInfo mLauncher;
         CSignalSemaphore * mpRequestSemaphore;
         tUseServiceList mUseList;
         CMutex * mpUseListMutex;
         CSignalSemaphore * mpStartSemaphore;

         Android::AppInfo mAppInfo;
      };
   }
}
#endif
