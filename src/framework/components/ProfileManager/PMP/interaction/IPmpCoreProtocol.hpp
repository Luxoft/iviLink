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











#ifndef IPMPCOREPROTOCOL_HPP_
#define IPMPCOREPROTOCOL_HPP_

#include <list>

#include "utils/misc/CError.hpp"
#include "utils/misc/UID.hpp"

namespace iviLink
{
   namespace PMP
   {
      /**
       * Interface for PMP Core Protocol functionality
       */
      class IPmpCoreProtocol
      {
      public:
         virtual CError getAvailableProfileComplements(std::list<Profile::Uid> & complements) = 0;
         virtual CError reloadProfilesFromRepository() = 0;
         virtual CError resetProfileState() = 0;
         virtual CError lock(iviLink::CUid id) = 0;
         virtual CError unlock(iviLink::CUid id) = 0;
         virtual CError lockAll() = 0;
         virtual CError unlockAll() = 0;
         virtual CError disableByClient(iviLink::CUid id) = 0;
         virtual CError enableByClient(iviLink::CUid id) = 0;
         virtual CError enableByClientAll() = 0;

         virtual ~IPmpCoreProtocol(){};
      };
   }
}

#endif /* IPMPCOREPROTOCOL_HPP_ */
