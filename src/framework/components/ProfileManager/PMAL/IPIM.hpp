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
 

#ifndef PMAL_IPIM_HPP_
#define PMAL_IPIM_HPP_

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "CProfile.hpp"
#include "UID.hpp"

#include "CPMALError.hpp"

namespace AXIS {
namespace PMAL {


/**
 * Public interface for PMAL PIM - the profile implementation manager.
 * This interface is designated for the client of PMAL.
 * PIM is responsible for:
 * * profile implementations loading/unloading;
 * * creation and destruction of profile instances objects.
 */
class IPIM
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
    * In case of any error (not NO_ERROR return value), pProfile will be NULL.
    *
    * @retval NO_ERROR profile loaded successfully
    * @retval ERROR_UNKNOWN_PROFILE_UID cannot get profile library by porfileUid
    * @retval ERROR_PIM_INTERNAL some internal PIM error preventing profile
    *    instance creation
    * @retval ERROR_PROFILE_INIT error on profile instance init prevents it use
    */
   virtual CPMALError loadProfile(AXIS::Profile::Uid const& profileUid,
         AXIS::Service::Uid const& sid,
         AXIS::Profile::IProfileCallbackProxy* const pProxy,
         Profile::CProfile*& pProfile) = 0;

   /**
    * Destroys loaded profile instance.
    * @param profile is the pointer to profile instance object
    * @retval NO_ERROR profile instance object successfully destroyed
    * @retval ERROR_WRONG_PARAM pProfile is NULL or points to unknown profile
    */
   virtual CPMALError unloadProfile(Profile::CProfile*& pProfile) = 0;

protected:

   virtual ~IPIM() { }

};


}  // namespace ProfileManager
}  // namespace AXIS


#endif /* PMAL_IPIM_HPP_ */
