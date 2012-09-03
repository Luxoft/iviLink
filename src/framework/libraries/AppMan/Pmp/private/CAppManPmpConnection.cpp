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











#include "CAppManPmpConnection.hpp"

#include <cassert>

namespace iviLink
{

   namespace AppMan
   {

      namespace Ipc
      {
         Logger CAmpPmpConnection::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.Pmp.Connection"));

         CAmpPmpConnection::CAmpPmpConnection(CAppManPmpIpcClient * pClient)
            : CThread("AppManPmpConnection")
            , mpClient(pClient)
         {
            LOG4CPLUS_TRACE(msLogger, "CAmpPmpConnection()");
            assert(mpClient);
         }

         CAmpPmpConnection::~CAmpPmpConnection()
         {
            LOG4CPLUS_TRACE(msLogger, "~CAmpPmpConnection()");
         }

         void CAmpPmpConnection::connect()
         {
            LOG4CPLUS_TRACE(msLogger, "connect()");
            start();
         }

         void CAmpPmpConnection::disconnect()
         {
            LOG4CPLUS_TRACE(msLogger, "disconnect()");
            mpClient->disconnect();
         }

         void CAmpPmpConnection::threadFunc()
         {
            LOG4CPLUS_TRACE(msLogger, "threadFunc()");
            mpClient->loop();
         }

      }

   }

}
