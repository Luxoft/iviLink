/* 
 * 
 * iviLINK SDK, version 1.1.2
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
 * 
 */


/**
 * @file                ProfileDatabase.hpp
 * @ingroup             Profile Manager
 * @author              Plachkov Vyacheslav <vplachkov@luxoft.com>
 * @date                10.01.2013
 *
 * ProfileDatabase class is a repository for Profiles
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
    typedef std::map<Profile::Uid,ProfileInfo> ProfileInfoMap;

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
    * Function is used for logging purposes
    */
    void printDB();

    /**
     * @param[out] libs contains all profile libs
     */
    void getAllLibs(std::list<LibInfo> & libs);

    /**
     * Counts relevance between request and profile
     * @return value in range 0...100
     */
     int getRelevance(const Profile::Uid & profile, const UInt32 version, const std::map<std::string, std::string> & profileArguments) const;

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
