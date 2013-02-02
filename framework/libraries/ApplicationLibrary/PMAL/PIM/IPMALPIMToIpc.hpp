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


#ifndef IPIMTOIPC_HPP_
#define IPIMTOIPC_HPP_

/// @todo temporary include
#include "UID.hpp"

namespace iviLink {
namespace PMAL {

class IPMALPIMToIpc
{
public:

   /**
    * Receives the message about incoming profile which must be launched on this
    * side.
    * @param profileUid is the profile id known on this side in repository
    * @param papiUid is the profile api uid of profile with given @c profileUid
    * @param piuid is the profile instance id used to spawn the profile on other
    * side
    * @param serviceUID is the service id to which profile belongs
    */
   virtual void incomingProfile(iviLink::BaseUid const& profileUid,
         iviLink::Profile::ApiUid const& papiUid,
         iviLink::Profile::IUid const& piuid,
         iviLink::Service::Uid const& serviceUID) = 0;

   /**
    * Receives the message about died profile
    * @param piuid
    */
   virtual void profileDied(iviLink::Profile::IUid const& piuid) = 0;

protected:

   virtual ~IPMALPIMToIpc() {}
};


}  // namespace ProfileManager
}  // namespace AXIS


#endif /* IPIMTOIPC_HPP_ */
