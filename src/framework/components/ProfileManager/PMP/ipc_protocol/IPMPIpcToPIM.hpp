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







#ifndef IPMPIPCTOPIM_HPP_
#define IPMPIPCTOPIM_HPP_

#include "utils/misc/CError.hpp"
#include "utils/misc/UID.hpp"
#include "utils/ipc/ipc_common.hpp"

namespace iviLink {
namespace PMP {

class IPMPIpcToPIM
{
public:

   /**
    * @todo description
    *
    * @param profileUid
    * @param piuid
    * @param serviceUID
    * @param dirId id of direction of connection
    *
    * @retval NO_ERROR
    * @retval ERROR_OTHER ipc errors 
    * 
    * @todo proper error codes
    */
   virtual CError incomingProfile(iviLink::CUid const& profileUid,
      iviLink::Profile::ApiUid const& papiUid,
      iviLink::Profile::IUid const& piuid,
      iviLink::Service::Uid const& serviceUID,
      iviLink::Ipc::DirectionID const& dirId) = 0;

   /**
    * @todo description
    *
    * @param piuid
    * @param dirId id of direction of connection
    *
    * @retval NO_ERROR
    * @retval ERROR_OTHER ipc errors 
    * 
    * @todo proper error codes
    */
   virtual CError profileDied(iviLink::Profile::IUid const& piuid,
      iviLink::Ipc::DirectionID const& dirId) = 0;

protected:

   virtual ~IPMPIpcToPIM()
   {
   }
};

}  // namespace ProfileManager
}  // namespace AXIS

#endif /* IPMPIPCTOPIM_HPP_ */
