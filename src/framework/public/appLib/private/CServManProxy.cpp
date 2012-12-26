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
 

#include "CServManProxy.hpp"
#include "CApp.hpp"

namespace iviLink
{
   namespace App
   {

      using Service::CServiceManager;

      CServManProxy::CServManProxy(CApp * pApp)
         : mpApp(pApp)
      {
      }

      CServManProxy::~CServManProxy()
      {
      }

      void CServManProxy::serviceRequestCallback(iviLink::Service::Uid const& serviceUID)
      {
         if (mpApp)
         {
            mpApp->onIncomingServiceBeforeLoading(serviceUID);
         }
      }

      void CServManProxy::serviceDropped(iviLink::Service::Uid const& serviceUID)
      {
         if (mpApp)
         {
            mpApp->onServiceDropped(serviceUID);
         }
      }

      void CServManProxy::serviceLoaded(const Service::Uid &serviceUID)
      {
         if (mpApp)
         {
            mpApp->onIncomingServiceAfterLoading(serviceUID);
         }
      }

      void CServManProxy::serviceLoadError(const Service::Uid & service)
      {
         if (mpApp)
         {
            mpApp->onServiceLoadError(service);
         }
      }
   }
}
