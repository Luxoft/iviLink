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







#ifndef CPROFILEDATABASE_HPP_
#define CPROFILEDATABASE_HPP_

#include <string>
#include <map>
#include <list>

#include "utils/misc/Logger.hpp"
#include "framework/components/ProfileRepository/CProfileRepoError.hpp"
#include "framework/components/ProfileRepository/RepoTypes.hpp"
#include "CProfileInfo.hpp"

class CMutex;

namespace pugi
{
   class xml_document;
}

namespace iviLink
{
   namespace ProfileRepository
   {

      /**
       * Class is used as Profile Database
       * It makes all operations to add/remove Profiles and Profile Implementations,
       * findProfile functionality and returns all needed information about profile
       */
      class CProfileDatabase
      {
      public:
         /**
          * This enumeration is used as data type for CProfileDatabase state
          */
         enum EDatabaseState
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
         CProfileDatabase(std::string dbPath = "ProfileDatabase.xml");

         /**
          * Destructor
          */
         ~CProfileDatabase();

         /**
         * Addes Profile to repository
         * @param xmlManifestPath contains path to Profile Manifest
         * @return true if success
         */
         CError addProfile(const std::string xmlManifestPath);

         /**
          * Removes entire profile with all implementations from repository
          * @param profileUid - UID of profile
          * @return CError code
          */
         CError removeProfile(UID profileUid);

         /**
          * Addes Profile implementation to repository
          * profileID - UID of Profile
          * @param library - lib descriptor of Profile Implementation library
          * @return CError code
          */
         CError addProfileImplementation(UID profileID, const LibDescriptor& library);

         /**
          * Removes single profile implementation from repository
          * @param profileUid - UID of profile
          * @param platform - information about target platform
          * @return CError code
          */
         CError removeProfileImplementation(UID profileUid, std::string platform);

         /**
          * Removes single profile implementation from repository
          * @param profileUid - ld - descriptor of library to remove
          * @return CError code
          */
         CError removeProfileImplementation(UID profileUid, LibDescriptor ld);

         /**
          * finds profiles with given requirements
          * @param id is UID of Profile API or Profile
          * @param profileParameters contains pairs of parameters and values
          * @param platform - information about target platform, if platform == "" - result will be returned for all platforms
          * @return map with pair Profile UID and LibDescriptor that contains information about platform and library path
          */
         std::list<LibInfo> findProfiles(UID id, const std::map<std::string, std::string> & profileArguments, std::string platform = "");

         /**
          * @param uid - UID of Profile or Profile API
          * @return manifest XML description or empty string if wrong UID
          */
         std::string getManifest(UID uid);

         /**
          * @return list of profile UIDs in database
          */
         std::list<UID> getProfilesList();
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

         std::string mDBPath;                   /// path to Database
         std::map<UID,CProfileInfo> mProfiles;  /// map with CProfileInfo structures that have all needed information about profiles
         EDatabaseState mDBState;               /// state of database
         CMutex * mpDbMutex;                    /// mutex for file operations
         CMutex * mpRequestMutex;               /// mutex for user requests

         static Logger msLogger;                ///< object of logger
      };
   }
}

#endif /* CPROFILEDATABASE_HPP_ */
