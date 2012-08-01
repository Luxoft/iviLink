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



#ifndef CSERVMANPROXY_HPP_
#define CSERVMANPROXY_HPP_

#include "framework/appLib/ServiceManager/CServiceManager.hpp"
#include "framework/appLib/appLib/common.hpp"
#include "framework/appLib/ServiceManager/common.hpp"

namespace iviLink
{
   class CApp;
   namespace App
   {

      class CServManProxy  : public Service::CServiceManager::IClientCallbacks
      {
      public:
         CServManProxy(CApp * pApp);
         virtual ~CServManProxy();

      private:

         //from IClientCallbacks
         virtual void serviceRequestCallback(iviLink::Service::Uid const& serviceUID);
         virtual void serviceDropped(iviLink::Service::Uid const& serviceUID);
         virtual void serviceLoadError(const Service::Uid & service);
         virtual void serviceLoaded(const Service::Uid & service);

         CApp * mpApp;
      };

   }
}
#endif
