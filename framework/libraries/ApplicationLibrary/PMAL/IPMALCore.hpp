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


#ifndef PMAL_ICORE_HPP_
#define PMAL_ICORE_HPP_

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <list>
#include <map>
#include <string>

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/

#include "UID.hpp"
#include "CPMALError.hpp"

namespace iviLink {
namespace PMAL {

/// Struct contains all information from Profile Manifest
struct ProfileInfo
{
   //ProfileInfo(const std::string & xml);      /// Constructor, loads information from XML file with Profile Manifest
   iviLink::Profile::Uid uid;                    /// UID of Profile
   std::map<std::string, std::string> params; /// Profile Parameters
   int version;                               /// Version of Profile
   std::string name;                          /// Name of Profile
   iviLink::Profile::ApiUid apiUid;                        /// UID of Profile API that Profile implenents
   iviLink::Profile::Uid complement;                    /// UID of complement Profile
};

/**
 * Public interface for PMAL Core.
 * This interface is designated for the client of PMAL.
 * The component responsible for searching profile and overall control of them.
 */
class IPMALCore
{
public:
   /**
    * Finds  profiles with given requirements
    * @param id - UID of Profile API or Profile
    * @param profileParameters contains pairs of parameters and values
    * @param[out] profiles - list of returned profiles
    * @param enabledProfiles - if true - only enabled profiles, otherwise all
    * @retval CPMALError::IVILINK_NO_ERROR - success
    * @retval CPMALError::ERROR_OTHER in case of any error (all errors are connected with IPC)
    */
   virtual CPMALError findProfiles(iviLink::BaseUid id,
         const std::map<std::string, std::string> & profileParameters
         , std::list<iviLink::Profile::Uid> & profiles, bool enabledProfiles = true) = 0;

   /**
    * Disables profiles by UID of Profile or Profile API
    * @param id - UID of Profile or Profile API
    * @retval CPMALError::IVILINK_NO_ERROR if success
    * @retval CPMALError::ERROR_UNKNOWN_PROFILE_UID if UID isn't in Profile Repository
    * @retval CPMALError::ERROR_OTHER if any other error
    */
   virtual CPMALError disableByUid(iviLink::BaseUid id) = 0;

   /**
    * Enables profiles
    * @param uid - UID of Profile of Profile API
    * @retval CPMALError::isNoError() if success
    * @retval CPMALError::ERROR_UNKNOWN_PROFILE_UID if UID isn't in Profile Repository
    * @retval CPMALError::ERROR_OTHER if any other error
    */
   virtual CPMALError enableByUid(iviLink::BaseUid uid) = 0;

   /**
    * Enables all Profiles in repository except ones prohibited by Manager due to some reasons
    * @retval CPMALError::isNoError() if success
    * @retval CPMALError::ERROR_OTHER in case of IPC errors
    */
   virtual CPMALError enableAll() = 0;

   /**
    * Function is used to get information about Profile
    * @param uid - Profile UID or UID of Profile Instance
    * @param[out] profInfo is ProfileInfo structure with information about Profile
    * @retval CPMALError::isNoError() if success
    * @retval CPMALError::ERROR_MANIFEST_PARSING if it's problems with XML parsing
    * @retval CPMALError::ERROR_UNKNOWN_PROFILE_UID in case of unknown Profile
    * @retval CPMALError::ERROR_OTHER in case of IPC and other errors
    */
   virtual CPMALError getProfileInfo(iviLink::Profile::Uid uid, ProfileInfo & profInfo) = 0;

protected:

   virtual ~IPMALCore() { }
};


}  // namespace ProfileManager
}  // namespace AXIS


#endif /* PMAL_ICORE_HPP_ */
