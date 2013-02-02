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


#ifndef IPROFILE_MANAGER_CALLBACKS
#define IPROFILE_MANAGER_CALLBACKS

#include "UID.hpp"

namespace iviLink
{
namespace PMAL
{

/// IProfileManagerCallbacks interface
/**
 * Profile manager uses this interface to send the profile control callbacks to Service  manager.
 */
class IProfileManagerCallbacks
{
public:

   /**
    * Callback for service created on the other side
    * @param serviceUID is the id of requested service
    * @retval IVILINK_NO_ERROR service is available and ready to receive incoming profile
    * @retval @todo other error codes
    */
   virtual BaseError serviceRequestCallback(iviLink::Service::Uid const& serviceUID) = 0;

   /**
    * Callback that should be invoked when paired profile requested from 
    * other side. This gives the client the opportunity to reject profile 
    * creation. Otherwise, client must call IPIM::loadProfile() with passed
    * @c profileUid (from this thread).
    * After the profile loading, client must pass profile processing to other
    * thread and return from this callback.
    * 
    * @param serviceUID service that should own this profile
    * @param profileUid profile that should be created
    * @param papiUid ProfileAPI uid of profil that should be created
    * 
    * @retval IVILINK_NO_ERROR all ok on PMAL client's side
    * @retval @todo other error codes
    */
   virtual BaseError incomingProfile(iviLink::Service::Uid const& serviceUID, 
      iviLink::Profile::Uid const& profileUid, 
      iviLink::Profile::ApiUid const& papiUid) = 0;

   /**
    * Callback for service dropped on the other side
    * @param serviceUID
    */
   virtual void serviceDropped(iviLink::Service::Uid const& serviceUID) = 0;

   /**
    * Callback that should be invoked when paired profile from other side died
    * @param serviceUID service that owns this profile
    * @param piuid is the profile instance id of dead profile
    */
   virtual void profileDied(iviLink::Service::Uid const& serviceUID, iviLink::Profile::IUid const& piuid) =0;

protected:

   virtual ~IProfileManagerCallbacks() {}
};

}  // namespace ProfileManager
}  // namespace AXIS

#endif
