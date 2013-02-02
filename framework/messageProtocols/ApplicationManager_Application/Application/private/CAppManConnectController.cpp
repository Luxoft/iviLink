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


#include "CAppManConnectController.hpp"
#include "CAppManProtoClient.hpp"
#include "CAppManConnect.hpp"
#include "CCore.hpp"

#include "CMutex.hpp"


#include <cassert>
#include <unistd.h>

namespace iviLink
{

   namespace AppMan
   {
      Logger CAppManConnectController::msLogger 
            = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.App.ConnectController"));
      CAppManConnectController * CAppManConnectController::mspInstance = 0;

      CAppManConnectController::CAppManConnectController(Android::AppInfo appInfo)
         : mpClient(new Ipc::CAppManProtoClient())
         , mpConnection(new Ipc::CAppManConnect(mpClient))
         , mpCore (new App::CCore(appInfo))
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         assert(mpClient);
         assert(mpConnection);
         assert(mpCore);
         mpCore->initIpc(mpClient);

         mpConnection->connect();
      }

      CAppManConnectController::~CAppManConnectController()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         uninit();
         mpConnection->disconnect();

         delete mpConnection;
         delete mpClient;
         delete mpCore;
      }

      static CMutex gInstanceMutex;

      CAppManConnectController * CAppManConnectController::instance(Android::AppInfo appInfo)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         if (!mspInstance)
         {
            gInstanceMutex.lock();
            if (!mspInstance)
            {
               mspInstance = new CAppManConnectController(appInfo);
            }
            gInstanceMutex.unlock();
         }
         return mspInstance;
      }

      void CAppManConnectController::deleteInstance()
      {
         gInstanceMutex.lock();
         delete mspInstance;
         mspInstance = 0;
         gInstanceMutex.unlock();
      }

      IAppMan * CAppManConnectController::applicationManager()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         return mpCore;
      }

      void CAppManConnectController::init(IAppManHandler * pHandler)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         mpCore->initHandler(pHandler);
         mpCore->initIpc(mpClient);
         mpClient->init(mpCore);
      }

      void CAppManConnectController::uninit()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         init(0);
      }

      bool CAppManConnectController::checkConnection () const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         return mpClient->checkConnection();
      }

      void CAppManConnectController::checkSessionRequest()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         if (mpClient)
         {
            mpClient->checkSessionRequest();
         }
      }

      void CAppManConnectController::registerAMProxy(iviLink::App::CAppManProxy * amProxy)
      {
          mpCore->registerAMProxy(amProxy);
      }

   }

}
