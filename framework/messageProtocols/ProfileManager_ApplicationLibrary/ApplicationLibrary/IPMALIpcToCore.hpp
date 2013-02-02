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


#ifndef IIPCTOCORE_HPP_
#define IIPCTOCORE_HPP_

#include <list>
#include <map>
#include <string>

#include "CPMALError.hpp"
#include "UID.hpp"

namespace iviLink
{
   namespace PMAL
   {

      class IPMALIpcToCore
      {
      public:
         /**
          * Disables Profile of Profile API in Profile Repository by given UID
          *
          * @param id
          *
          * @retval IVILINK_NO_ERROR
          * @retval ERROR_UNKNOWN_PROFILE_UID profile id unknown and cannot be
          *    disabled
          * @retval ERROR_OTHER ipc errors @todo proper error codes
          */
         virtual bool disableByUid(iviLink::BaseUid const& id) = 0;

         /**
          * Enables Profile of Profile API in Profile Repository by given UID
          *
          * @param id
          *
          * @retval IVILINK_NO_ERROR
          * @retval ERROR_UNKNOWN_PROFILE_UID profile id unknown and cannot be
          *    enabled
          * @retval ERROR_OTHER ipc errors @todo proper error codes
          */
         virtual bool enableByUid(iviLink::BaseUid const& id) = 0;

         /**
          * Enables all Profiles in Profile Repository
          *
          * @retval IVILINK_NO_ERROR
          * @retval ERROR_OTHER ipc errors @todo proper error codes
          */
         virtual bool enableAll() = 0;

         /**
          * Returns Profile of Profile API manifest by given UID
          *
          * @param uid
          * @param manifest
          *
          * @retval IVILINK_NO_ERROR
          * @retval ERROR_UNKNOWN_PROFILE_UID profile id unknown and
          *    manifest cannot be obtained
          * @retval ERROR_OTHER ipc errors @todo proper error
          */
         virtual bool getManifest(iviLink::BaseUid const& uid, std::string& manifest) = 0;

         /**
          * Returns path to Profile implementation library
          *
          * @param uid
          * @param path
          *
          * @retval IVILINK_NO_ERROR
          * @retval ERROR_UNKNOWN_PROFILE_UID profile id unknown and
          *    manifest cannot be obtained
          * @retval ERROR_OTHER ipc errors @todo proper error
          */
         virtual bool getProfileLibPath(iviLink::Profile::Uid const& uid, std::string& path) = 0;

         /**
          * Returns list of Profile UIDs found by Profile Repository
          *
          * @param id
          * @param profileParameters
          * @param profiles
          * @param enabledProfiles
          *
          * @retval IVILINK_NO_ERROR
          * @retval ERROR_OTHER ipc errors @todo proper error
          */
         virtual bool findProfiles(iviLink::BaseUid const& id,
               std::map<std::string, std::string> const& profileParameters,
               std::list<iviLink::Profile::Uid> & profiles,
               bool enabledProfiles) = 0;

      protected:
         virtual ~IPMALIpcToCore() {}
      };
   }
}

#endif /* IIPCTOCORE_HPP_ */
