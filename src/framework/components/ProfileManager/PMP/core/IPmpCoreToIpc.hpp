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
 

#ifndef IPMPCORETOIPC_HPP_
#define IPMPCORETOIPC_HPP_

#include <map>
#include <list>

#include "UID.hpp"
#include "CError.hpp"

namespace iviLink
{
   namespace PMP
   {
      /**
       * PMP Core interface for IPC
       */
      class IPmpCoreToIpc
      {
      public:

         /**
          * Disables Profile and complement (on another side) by client's request
          * @param id is UID of Profile to disable
          * @return true if success and false if UID is unknown
          */
         virtual bool disableByClient(iviLink::CUid id) = 0;

         /**
          * Enables Profile and complement (on another side) by client's request
          * @param id is UID of Profile
          * @return true if success and false if UID is unknown
          */
         virtual bool enableByClient(iviLink::CUid id) = 0;

         /**
          * Enables all Profiles by client's request
          */
         virtual void enableByClientAll() = 0;

         /**
          * Makes request to Profile Repository to get Profile of Profile API manifest
          * @param uid is UID of Profile of Profile API
          * @manifest[out] is result string with manifest
          * @return true if UID is unknown
          */
         virtual bool getManifest(iviLink::CUid uid, std::string & manifest) = 0;

         /**
          * Function is used to get path to Profile Implementation library
          * @param uid is UID of Profile
          * @param[out] path is path to Profile Implementation library
          * @return true if success and false if UID is unknown
          */
         virtual bool getProfileLibPath(iviLink::Profile::Uid uid, std::string& path) = 0;

         /**
          * Calls another version of findProfiles() function with list of LibInfo objects.
          * This version of function is used to make requests by clients of Profile Manager
          * @param id is UID of Profile API of Profile
          * @param profileParameters is map of Profile Parameters
          * @param[out] profiles is list of results
          * @param enabledProfiles is true if client want to get only enabled and unlocked Profiles
          */
         virtual void findProfiles(iviLink::CUid id,
               const std::map<std::string, std::string> & profileParameters
               , std::list<iviLink::CUid> & profiles, bool enabledProfiles = true) = 0;
      };
   }
}

#endif /* IPMPCORETOIPC_HPP_ */
