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







#include "CAppManPmpController.hpp"
#include "CAppManPmpIpcClient.hpp"

#include <cassert>
#include <unistd.h>

namespace iviLink
{

   namespace AppMan
   {

      namespace Ipc
      {
         Logger CAppManPmpController::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.Pmp.Controller"));
         CAppManPmpController * CAppManPmpController::mspInstance = 0;

         CAppManPmpController::CAppManPmpController()
            : mpClient(new CAppManPmpIpcClient())
            , mpConnection(new CAmpPmpConnection(mpClient))
         {
            LOG4CPLUS_TRACE(msLogger, "CAppManPmpController()");
            assert(mpClient);
            assert(mpConnection);

            mpConnection->connect();
            usleep(125000);
         }

         CAppManPmpController::~CAppManPmpController()
         {
            LOG4CPLUS_TRACE(msLogger, "~CAppManPmpController()");
            mpClient->init(0);
            mpConnection->disconnect();

            delete mpConnection;
            delete mpClient;
         }

         CAppManPmpController * CAppManPmpController::instance()
         {
            LOG4CPLUS_TRACE(msLogger, "instance()");
            static CMutex mutex;
            if (!mspInstance)
            {
               mutex.lock();
               if (!mspInstance)
               {
                  mspInstance = new CAppManPmpController();
               }
               mutex.unlock();
            }
            return mspInstance;
         }

         IPmpRequest * CAppManPmpController::pmpRequest()
         {
            LOG4CPLUS_TRACE(msLogger, "pmpRequest()");
            return mpClient;
         }

         void CAppManPmpController::init(IPmpHandler * pHandler)
         {
            LOG4CPLUS_TRACE(msLogger, "init()");
            mpClient->init(pHandler);
         }

         void CAppManPmpController::uninit()
         {
            LOG4CPLUS_TRACE(msLogger, "uninit()");
            init(0);
         }

         bool CAppManPmpController::checkConnection () const
         {
            LOG4CPLUS_TRACE(msLogger, "checkConnection()");
            return mpClient->checkConnection();
         }

      }

   }

}
