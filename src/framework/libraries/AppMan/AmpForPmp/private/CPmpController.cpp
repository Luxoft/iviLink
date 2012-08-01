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







#include "CPmpController.hpp"
#include "CAppManPmpIpcServer.hpp"

#include <cassert>
#include <unistd.h>

namespace iviLink
{

   namespace AppMan
   {

      namespace Ipc
      {
         Logger CPmpController::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.AmpForPmp.Controller"));

         CPmpController::CPmpController()
            : mpServer(new CAppManPmpIpcServer())
            , mpConnection(new CPmpConnection(mpServer))
         {
            LOG4CPLUS_TRACE(msLogger,"CPmpController()");
            assert(mpServer);
            assert(mpConnection);

            mpConnection->connect();
            usleep(125000);
         }

         CPmpController::~CPmpController()
         {
            LOG4CPLUS_TRACE(msLogger,"C~PmpController()");
            mpServer->init(0,0);
            mpConnection->disconnect();

            delete mpConnection;
            delete mpServer;
         }

         CPmpController * CPmpController::instance()
         {
            LOG4CPLUS_TRACE(msLogger,"instance()");
            static CPmpController sInstance;
            return &sInstance;
         }

         IPmpHandler * CPmpController::pmpHandler()
         {
            LOG4CPLUS_TRACE(msLogger,"pmpHandler()");
            return mpServer;
         }

         void CPmpController::init(IPmpRequest * pRequest, IPmpConnectionStatus * pConnectionStatus)
         {
            LOG4CPLUS_TRACE(msLogger,"init()");
            mpServer->init(pRequest,pConnectionStatus);
         }

         void CPmpController::uninit()
         {
            LOG4CPLUS_TRACE(msLogger,"uninit()");
            init(0,0);
         }

         bool CPmpController::checkConnection() const
         {
            LOG4CPLUS_TRACE(msLogger,"checkConnection()");
            return mpServer->checkConnection();
         }

      }

   }

}
