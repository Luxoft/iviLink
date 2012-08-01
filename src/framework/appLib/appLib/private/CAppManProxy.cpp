/* 
 * 
 * iviLINK SDK, version 1.0
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




#include "CAppManProxy.hpp"
#include "utils/threads/CMutex.hpp"
#include "framework/appLib/ApplicationManager/CAppManConnectController.hpp"
#include "framework/appLib/ApplicationManager/IAppMan.hpp"
#include "CLogger.hpp"

namespace iviLink
{
   namespace App
   {

      CAppManProxy::CAppManProxy(CApp * pApp)
         : CThread("iviLinkAppCoreThread")
         , mpApp(pApp)
         , mInitStatus(NOT_INITED)
         , mpInitMutex(new CMutex())
         , mBe(true)
         , mLauncher(ERROR_APP_NOT_INITED_IN_IVILINK)
         , mpRequestSemaphore(new CSignalSemaphore())
         , mpUseListMutex(new CMutex())
      {
         LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
      }

      CAppManProxy::~CAppManProxy()
      {
         LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
         AppMan::CAppManConnectController::deleteInstance();
         delete mpInitMutex;
         delete mpRequestSemaphore;
         delete mpUseListMutex;
      }

      void CAppManProxy::threadFunc()
      {
         mpRequestSemaphore->wait();
         while (mBe)
         {
            handleInit();
            handleUseService();
            if (!hasRequests() && mBe)
            {
               mpRequestSemaphore->wait();
            }
         }
      }

      void CAppManProxy::init(const Service::ListOfUids &services)
      {
         LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
         mpInitMutex->lock();
         if (NOT_INITED == mInitStatus)
         {
            mInitStatus = HAS_INIT_REQUEST;
            mInitedServices = services;
         }
         mpInitMutex->unlock();
         mpRequestSemaphore->signal();
      }

      void CAppManProxy::handleInit()
      {
         LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
         mpInitMutex->lock();
         bool compareRes = HAS_INIT_REQUEST == mInitStatus;
         mpInitMutex->unlock();
         if (compareRes)
         {
            AppMan::EInitResult initRes;
            do
            {
               initRes = AppMan::CAppManConnectController::instance()->applicationManager()
                     ->initApplication(mInitedServices);
               if (AppMan::STARTED_BY_APPMAN == initRes)
               {
                  mLauncher = LAUNCHED_BY_IVILINK;
                  mpApp->initDone(mLauncher);
                  break;
               }
               else if (AppMan::STARTED_BY_USER == initRes)
               {
                  mLauncher = LAUNCHED_BY_USER;
                  mpApp->initDone(mLauncher);
                  break;
               }
               usleep(250000);
            }
            while(mBe);
         }
      }


      void CAppManProxy::handleUseService()
      {
         LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
         mpUseListMutex->lock();
         if (mUseList.empty())
         {
            mpUseListMutex->unlock();
            return;
         }
         bool use = mUseList.front().second;
         Service::Uid service = mUseList.front().first;
         mpUseListMutex->unlock();

         CError err = AppMan::CAppManConnectController::instance()->applicationManager()
               ->useService(service, use);

         if (err.isNoError())
         {
            mpUseListMutex->lock();
            mUseList.erase(mUseList.begin());
            mpUseListMutex->unlock();
         }
      }

      bool CAppManProxy::hasRequests()
      {
         mpUseListMutex->lock();
         bool res = !mUseList.empty();
         mpUseListMutex->unlock();
         return res;
      }

      void CAppManProxy::finish()
      {
         LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
         mBe = false;
         mpRequestSemaphore->signal();
         stop();
      }


      void CAppManProxy::useService(const Service::Uid& service, bool use)
      {
         LOG4CPLUS_TRACE_METHOD(*App::CLogger::logger(),__PRETTY_FUNCTION__);
         mpUseListMutex->lock();
         mUseList.push_back(make_pair(service,use));
         mpUseListMutex->unlock();
         mpRequestSemaphore->signal();
      }

      ELaunchInfo CAppManProxy::launcher() const
      {
         return mLauncher;
      }

   }
}
