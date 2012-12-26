/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#include "CProfileRepoConnection.hpp"
#include "CProfileRepoClient.hpp"

namespace iviLink
{

   namespace PMP
   {

      Logger CProfileRepoConnection::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.CProfileRepoConnection"));

      CProfileRepoConnection::CProfileRepoConnection(ProfileRepository::CProfileRepoClient * pRepoClient)
         : CThread("ProfileManagerRepoConnection")
         , mpRepoClient(pRepoClient)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

      }

      CProfileRepoConnection::~CProfileRepoConnection()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

      }

      void CProfileRepoConnection::threadFunc()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpRepoClient->loop();
      }

      void CProfileRepoConnection::connect()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         start();
      }

      void CProfileRepoConnection::disconnect()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpRepoClient->disconnect();
      }

   }

}
