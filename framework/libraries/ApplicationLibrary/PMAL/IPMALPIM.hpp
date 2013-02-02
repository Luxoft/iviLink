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


#ifndef PMAL_IPIM_HPP_
#define PMAL_IPIM_HPP_

#include "ProfileHolder.hpp"
#include "UID.hpp"

#include "CPMALError.hpp"

#include "AppInfo.hpp"

namespace iviLink {
namespace PMAL {

/**
 * Public interface for PMAL PIM - the profile implementation manager.
 * This interface is designated for the client of PMAL.
 * PIM is responsible for:
 * * profile implementations loading/unloading;
 * * creation and destruction of profile instances objects.
 */
class IPMALPIM
{
public:
   /**
    * Creates the instance of profile and requests the supplement profile on
    * other side.
    *
    * @param profileUid is the id of profile implementation library
    * @param sid is the id of service to which profile belongs
    * @param pProxy it the pointer profile callbacks handler
    * @param [out] pProfile is the resulting pointer to profile instance object.
    * @param info application info, necessary for Android
    * In case of any error (not IVILINK_NO_ERROR return value), pProfile will be NULL.
    *
    * @retval IVILINK_NO_ERROR profile loaded successfully
    * @retval ERROR_UNKNOWN_PROFILE_UID cannot get profile library by porfileUid
    *    The reason may vary: profile is really unknown, profile is disabled,
    *    profile is known, but its path is unknow (and such other problems in
    *    profile repo), etc.
    * @retval ERROR_PIM_INTERNAL some internal PIM error preventing profile
    *    instance creation
    * @retval ERROR_PROFILE_INIT error on profile instance init prevents it use
    */
   virtual CPMALError loadProfile(iviLink::Profile::Uid const& profileUid,
         iviLink::Service::Uid const& sid,
         iviLink::Profile::IProfileCallbackProxy* const pProxy,
         Profile::ProfileHolder*& pProfile,
         Android::AppInfo info = Android::AppInfo()) = 0;


   /**
    * Destroys loaded profile instance.
    * @param profile is the pointer to profile instance object
    * @retval IVILINK_NO_ERROR profile instance object successfully destroyed
    * @retval ERROR_WRONG_PARAM pProfile is NULL or points to unknown profile
    */
   virtual CPMALError unloadProfile(Profile::ProfileHolder*& pProfile) = 0;

protected:

   virtual ~IPMALPIM() { }

};

}  // namespace ProfileManager
}  // namespace AXIS

#endif /* PMAL_IPIM_HPP_ */
