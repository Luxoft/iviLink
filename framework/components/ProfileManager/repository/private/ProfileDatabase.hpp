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


#ifndef CPROFILEDATABASE_HPP_
#define CPROFILEDATABASE_HPP_

#include <string>
#include <map>
#include <list>

#include "Logger.hpp"
#include "RepositoryError.hpp"
#include "RepositoryTypes.hpp"
#include "ProfileInfo.hpp"

namespace pugi
{
class xml_document;
}

namespace iviLink
{
namespace PMP
{

/**
* Class is used as Profile Database
* It makes all operations to add/remove Profiles and Profile Implementations,
* findProfile functionality and returns all needed information about profile
*/
class ProfileDatabase
{
public:
    typedef std::map<Profile::Uid,CProfileInfo> ProfileInfoMap;

    /**
    * This enumeration is used as data type for ProfileDatabase state
    */
    enum DatabaseState
    {
        EDB_NORMAL_OK,
        EDB_OPEN_ERROR,
        EDB_SAVE_ERROR,
        EDB_PARSING_ERROR,
        EDP_EMPTY
    };

    /**
    * Constructor
    * @param dbPath = path of XML with Profile Database
    */
    ProfileDatabase(const std::string& rootFolderPath, const std::string & relativeFolderPath, const std::string & dbName);

    /**
    * Destructor
    */
    ~ProfileDatabase();

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
    std::list<LibInfo> findProfiles(Profile::Uid id, const std::map<std::string, std::string> & profileArguments, std::string platform = "");

    /**
    * @param uid - UID of Profile or Profile API
    * @return manifest XML description or empty string if wrong UID
    */
    std::string getManifest(Profile::Uid uid);

    /**
    * @return list of profile UIDs in database
    */
    std::list<Profile::Uid> getProfilesList();
    /**
    * Function is used for logging purposes
    */
    void printDB();

private:
    /**
    * Loads Database from file
    */
    void loadDatabase();

    /**
    * Loads data from parsed DOM structure
    * @param doc DOM object that contains parsed data
    * @return true if success
    */
    bool loadParsedDatabase(const pugi::xml_document &doc);

    /**
    * Saves Database into an XML file
    * @return true if success
    */
    bool saveChanges();

    /**
    * Counts relevance between request and profile
    * @return value in range 0...100
    */
    int getRelevance(const CProfileInfo & proInfo, const std::map<std::string, std::string> & profileArguments);

    std::string mFolderPath;
    std::string mDbName;
    std::string mRootFolderPath;
    ProfileInfoMap mProfiles;  /// map with CProfileInfo structures that have all needed information about profiles
    DatabaseState mDbState;               /// state of database

    static Logger msLogger;                ///< object of logger
};

}
}

#endif /* CPROFILEDATABASE_HPP_ */
