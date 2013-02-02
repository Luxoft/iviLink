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


#include "CPmpConnection.hpp"

#include <cassert>

namespace iviLink
{

   namespace AppMan
   {

      namespace Ipc
      {
         Logger CPmpConnection::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Ipc.AmpForPmp.Connection"));

         CPmpConnection::CPmpConnection(CAppManPmpIpcServer * pServer)
            :CThread("AppManPmpCon")
            , mpServer(pServer)
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            assert(mpServer);
         }

         CPmpConnection::~CPmpConnection()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
         }

         void CPmpConnection::disconnect()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpServer->disconnect();
         }

         void CPmpConnection::threadFunc()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            mpServer->loop();
         }

         void CPmpConnection::connect()
         {
            LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
            start();
         }

      }

   }

}
