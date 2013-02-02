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


#ifndef PROFILE_REPOSITORY_HPP_
#define PROFILE_REPOSITORY_HPP_

#include <string>
#include <list>

#include "RepositoryTypes.hpp"
#include "RepositoryError.hpp"
#include "Logger.hpp"
#include "configurator.h"

namespace iviLink
{

namespace PMP
{

class ProfileApiDatabase;
class ProfileDatabase;

class ProfileRepository
{
public:

    ProfileRepository(conf::Configurator * configurator);

    ~ProfileRepository();

    /**
     * Addes Profile API to repository
     * @param manifestPath contains path of manifest XML
     * @return CError code
     */
    BaseError addProfileApi(std::string manifestPath);

    /**
     * Removes profile API from repository
     * @param profileApiUid string is UID of Profile API
     * @return CError code
     */
    BaseError removeProfileApi(Profile::ApiUid profileApiUid);

    /**
     * Addes Profile to repository
     * @param xmlManifestPath contains path to Profile Manifest
     * @return true if success
     */
    BaseError addProfile(const std::string xmlManifestPath);

     /**
      * Removes entire profile with all implementations from repository
      * @param profileUid - UID of profile
      * @return CError code
      */
    BaseError removeProfile(Profile::Uid profileUid);

     /**
      * Addes Profile implementation to repository
      * profileID - UID of Profile
      * @param library - lib descriptor of Profile Implementation library
      * @return CError code
      */
    BaseError addProfileImplementation(Profile::Uid profileID, const LibDescriptor& library);

     /**
      * Removes single profile implementation from repository
      * @param profileUid - UID of profile
      * @param platform - information about target platform
      * @return CError code
      */
    BaseError removeProfileImplementation(Profile::Uid profileUid, std::string platform);

     /**
      * Removes single profile implementation from repository
      * @param profileUid - ld - descriptor of library to remove
      * @return CError code
      */
    BaseError removeProfileImplementation(Profile::Uid profileUid, LibDescriptor ld);

     /**
      * finds profiles with given requirements
      * @param id is UID of Profile API or Profile
      * @param profileParameters contains pairs of parameters and values
      * @param platform - information about target platform, if platform == "" - result will be returned for all platforms
      * @return map with pair Profile UID and LibDescriptor that contains information about platform and library path
      */
    std::list<LibInfo> findProfiles(Profile::ApiUid id, const std::map<std::string, std::string> & profileArguments, std::string platform);

     /**
      * @param uid - UID of Profile or Profile API
      * @return manifest XML description or empty string if wrong UID
      */
    std::string getManifest(Profile::Uid uid);

     /**
      * @return list of profile UIDs in database
      */
    std::list<Profile::Uid> getProfilesList();

private:
    ProfileApiDatabase * mProfileApiDatabase;
    ProfileDatabase * mProfileDatabase;
    conf::Configurator * mConfigurator;

    static Logger msLogger; ///< object of logger
};

} /* namespace PMP */
} /* namespace iviLink */
#endif /* PROFILE_REPOSITORY_HPP_ */
