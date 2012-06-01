/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */

/***************************************************************************
 * Project           AXIS
 * (c) copyright     2012
 * Company           LUXOFT
 * @file             CProfileApiDatabase.hpp
 * @brief            Header file for CProfileApiDatabase class (part of Profile Repository Process)
 * @author           VPlachkov
 ***************************************************************************/

#ifndef CPROFILEAPIDATABASE_HPP_
#define CPROFILEAPIDATABASE_HPP_

#include <string>
#include <map>

#include "utils/misc/include/Logger.hpp"
#include "framework/components/ProfileRepository/include/RepoTypes.hpp"
#include "framework/components/ProfileRepository/process/CProfileApiInfo.hpp"

class CMutex;

namespace pugi
{
   class xml_document;
}

namespace AXIS
{
   namespace ProfileRepository
   {

      /**
       * This class is used as Profile API Database
       * Is loads/saves database, reacts on clients requests
       */
      class CProfileApiDatabase
      {
      public:

         /**
          * This enum is used as data type for CProfileApiDatabase state
          */
         enum EDatabaseState
         {
            EDB_NORMAL_OK,
            EDB_OPEN_ERROR,
            EDB_SAVE_ERROR,
            EDB_PARSING_ERROR,
            EDB_EMPTY
         };

         /**
          * Constructor
          * @param dbPath = path of XML with Profile API Database
          */
         CProfileApiDatabase(std::string dbPath = "ProfileApiDatabase.xml");

         /**
          * Destructor
          */
         ~CProfileApiDatabase();

         /**
          * Addes Profile API to repository
          * @param manifestPath contains path of manifest XML
          * @return CError code
          */
         CError addProfileApi(std::string manifestPath);

         /**
          * Removes profile API from repository
          * @param profileApiUid string is UID of Profile API
          * @return CError code
          */
         CError removeProfileApi(UID profileApiUid);

         /**
          * Function is used to get Profile API manifest
          * @param uid id of Profile API
          * @return XML-string with Profile API manifest
          */
         std::string getManifest(UID uid);

         /**
          * This function is used for printing Database to standard output
          */
         void printDB();

      private:
         /**
          * Loads Profile API Database from XML file
          */
         void loadDatabase();

         /**
          * Loads parsed information to into internal map
          * @param doc - opened and parsed XML file
          * @return true if OK, otherwise false
          */
         bool loadParsedDatabase(const pugi::xml_document &doc);

         /**
          * Saves Profile API database to XML file
          */
         bool saveChanges();

         std::string mDBPath; /// path of Profile API Database XML file
         std::map<UID,CProfileApiInfo> mApis; /// map with CProfileApiInfo structure, thar contains info about Profile APIs in Database
         EDatabaseState mDBState; /// state of CProfileApiDatabase object
         CMutex * mpDbMutex; /// mutex for file operations
         CMutex * mpRequestMutex; /// mutex for functions with user requests

         static Logger msLogger; ///< object of logger
      };
   }
}

#endif /* CPROFILEAPIDATABASE_HPP_ */
