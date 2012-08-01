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








#ifndef IPROFILEREPOCLIENT_HPP_
#define IPROFILEREPOCLIENT_HPP_

#include <list>
#include <map>
#include <string>

#include "RepoTypes.hpp"
#include "utils/misc/CError.hpp"

namespace iviLink
{

   namespace ProfileRepository
   {

      /**
       * Class is used as client of Profile Repository. Is used to make requests to Profile Repository
       * and returns results of this requests
       */
      class IProfileRepoClient
      {
      public:

         /**
          * Addes Profile API to repository. Calls function with the same name in CProfileApiDatabase class
          * @param manifestPath contains path of manifest XML
          * @return CError code
          */
         virtual CError addProfileApi(const std::string &manifestPath) = 0;

         /**
          * Removes profile API from repository. Calls function with the same name in CProfileApiDatabase class
          * @param profileApiUid string is UID of Profile API
          * @return CError code
          */
         virtual CError removeProfileApi(UID profileApiUid) = 0;

         /**
          * Removes entire profile with all implementations from repository. Calls function with the same name in CProfileDatabase class
          * @param profileUid - UID of profile
          * @return CError code
          */
         virtual CError addProfile(const std::string &xmlManifestPath) = 0;

         /**
          * Removes entire profile with all implementations from repository. Calls function with the same name in CProfileDatabase class
          * @param profileUid - UID of profile
          * @return CError code
          */
         virtual CError removeProfile(UID profileUid) = 0;

         /**
          * Addes Profile implementation to repository. Calls function with the same name in CProfileDatabase class
          * profileID - UID of Profile
          * @param library - lib descriptor of Profile Implementation library
          * @return CError code
          */
         virtual CError addProfileImplementation(UID profileID, const LibDescriptor& library) = 0;

         /**
          * Removes single profile implementation from repository. Calls function with the same name in CProfileDatabase class
          * @param profileUid - UID of profile
          * @param platform - information about target platform
          * @return CError code
          */
         virtual CError removeProfileImplementation(UID profileUid, const std::string &platform) = 0;

         /**
          * finds profiles with given requirements. Calls function with the same name in CProfileDatabase class
          * @param id is UID of Profile API or Profile
          * @param profileParameters contains pairs of parameters and values
          * @param[out] implementations is map with pair Profile UID and LibDescriptor that contains information about platform and library path
          * @param platform - information about target platform, if platform == "" - result will be returned for all platforms
          * @return CError code
          */
         virtual CError findProfiles(UID id, const std::map<std::string, std::string> & profileArguments, std::list<LibInfo> &implementations, const std::string &platform) = 0;

         /**
          * Calls function with the same name in CProfileDatabase class
          * @param uid - UID of Profile or Profile API
          * @return manifest XML description or empty string if wrong UID
          */
         virtual std::string getManifest(UID uid) = 0;

         /**
          * Calls function with the same name in CProfileDatabase class
          * @return list of profile UIDs from database
          */
         virtual std::list<UID> getProfilesList() = 0;
      };

   }

}

#endif /* IPROFILEREPOCLIENT_HPP_ */
