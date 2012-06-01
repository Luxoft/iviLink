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
 * @file             CProfileApiDatabase.cpp
 * @brief            Source file for CProfileApiDatabase class (part of Profile Repository Process)
 * @author           VPlachkov
 ***************************************************************************/

#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>

#include "CProfileApiDatabase.hpp"
#include "utils/xml/include/pugixml.hpp"
#include "utils/threads/include/CMutex.hpp"


namespace AXIS
{
   namespace ProfileRepository
   {

      Logger CProfileApiDatabase::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileRepository.Process.CProfileApiDatabase"));

      CProfileApiDatabase::CProfileApiDatabase(std::string dbPath)
      : mDBPath(dbPath)
      , mDBState(EDB_NORMAL_OK)
      , mpDbMutex(new CMutex)
      , mpRequestMutex(new CMutex)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         loadDatabase();
         printDB();
      }

      CProfileApiDatabase::~CProfileApiDatabase()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         delete mpDbMutex;
         delete mpRequestMutex;
      }

      void CProfileApiDatabase::loadDatabase()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpDbMutex->lock();
         mApis.clear();
         pugi::xml_document doc;
         // Get DOM structure from XML file
         pugi::xml_parse_result res = doc.load_file(mDBPath.c_str());
         switch (res.status)
         {
         case pugi::status_ok:              // No error
            mDBState = EDB_NORMAL_OK;
            break;
         case pugi::status_file_not_found:      // File was not found during load_file()
         case pugi::status_io_error:            // Error reading from file/stream
         case pugi::status_out_of_memory:       // Could not allocate memory
         case pugi::status_internal_error:      // Internal error occurred
            LOG4CPLUS_ERROR(msLogger, "Profile API Database open error");
            mDBState = EDB_OPEN_ERROR;
            break;
         case pugi::status_unrecognized_tag:    // Parser could not determine tag type
         case pugi::status_bad_pi:              // Parsing error occurred while parsing document declaration/processing instruction
         case pugi::status_bad_comment:         // Parsing error occurred while parsing comment
         case pugi::status_bad_cdata:           // Parsing error occurred while parsing CDATA section
         case pugi::status_bad_doctype:         // Parsing error occurred while parsing document type declaration
         case pugi::status_bad_pcdata:          // Parsing error occurred while parsing PCDATA section
         case pugi::status_bad_start_element:   // Parsing error occurred while parsing start element tag
         case pugi::status_bad_attribute:       // Parsing error occurred while parsing element attribute
         case pugi::status_bad_end_element:     // Parsing error occurred while parsing end element tag
         case pugi::status_end_element_mismatch: // There was a mismatch of start-end tags (closing tag had incorrect name, some tag was not closed or there was an excessive closing tag)
            LOG4CPLUS_ERROR(msLogger, "Profile API Database parsing error");
            mDBState = EDB_PARSING_ERROR;
            break;
         default:
            assert(0);
            break;
         }
         loadParsedDatabase(doc);
         mpDbMutex->unlock();
      }

      bool CProfileApiDatabase::loadParsedDatabase(const pugi::xml_document &doc)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (EDB_NORMAL_OK != mDBState)
         {
            return false;
         }
         pugi::xml_node apis = doc.child("profile-apis");
         for (pugi::xml_node_iterator it = apis.begin(); it != apis.end(); ++it)
         {
            std::map<UID,CProfileApiInfo>::iterator mit = mApis.find(UID(it->child_value("uid")));
            if (mApis.end() == mit)
            {
               CProfileApiInfo inf = CProfileApiInfo(std::string(it->child_value("src")));
               if (!inf.failed())
               {
                  mApis[UID(it->child_value("uid"))] = inf;
               }
            }
            else if (0 != it->child_value("uid"))
            {
               LOG4CPLUS_WARN(msLogger, "Error: UID repetition: " + std::string(it->child_value("uid")));
            }
            else
            {
               LOG4CPLUS_WARN(msLogger, "Error: UID repetition");
            }
         }
         return true;
      }

      bool CProfileApiDatabase::saveChanges()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         pugi::xml_document doc;
         pugi::xml_node apis = doc.append_child("profile-apis");
         mpDbMutex->lock();
         for (std::map<UID,CProfileApiInfo>::const_iterator it = mApis.begin(); it != mApis.end(); ++it)
         {
            LOG4CPLUS_INFO(msLogger, it->first.value);
            pugi::xml_node apiNode = apis.append_child("profile-api");
            pugi::xml_node uidNode = apiNode.append_child("uid");
            uidNode.append_child(pugi::node_pcdata).set_value(it->first.value.c_str());
            pugi::xml_node srcNode = apiNode.append_child("src");
            srcNode.append_child(pugi::node_pcdata).set_value(it->second.xmlPath().c_str());
         }
         bool result =doc.save_file(mDBPath.c_str());
         mpDbMutex->unlock();
         if (!result)
         {
            LOG4CPLUS_ERROR(msLogger, "ERROR : eDBSaveError");
            mDBState = EDB_SAVE_ERROR;
         }
         printDB();
         return result;
      }

      CError CProfileApiDatabase::addProfileApi(std::string manifestPath)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "manifestPath = " + manifestPath);
         CProfileApiInfo info(manifestPath);
         info.print();
         mpRequestMutex->lock();
         std::map<UID,CProfileApiInfo>::iterator mit = mApis.find(info.uid());
         if (mApis.end() != mit)
         {
            LOG4CPLUS_WARN(msLogger, "Error: UID repetition");
            mpRequestMutex->unlock();
            return CProfileRepoError(CProfileRepoError::ERROR_UID_ALREADY_EXISTS);
         }
         else
         {
            LOG4CPLUS_TRACE(msLogger, "##else");
            mApis[info.uid()] = info;
         }
         LOG4CPLUS_TRACE(msLogger, "toSave()");
         if (saveChanges())
         {
            mpRequestMutex->unlock();
            LOG4CPLUS_INFO(msLogger, "saveChanges() : true");
            return CProfileRepoError::NoProfileRepoError();
         }
         else
         {
            mpRequestMutex->unlock();
            LOG4CPLUS_INFO(msLogger, "saveChanges() : false");
            return CProfileRepoError(CProfileRepoError::ERROR_DATABASE_WRITE);
         }
      }

      CError CProfileApiDatabase::removeProfileApi(UID profileApiUid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "profileApiUid = '" + profileApiUid.value + "'");
         mpRequestMutex->lock();
         std::map<UID,CProfileApiInfo>::iterator mit = mApis.find(profileApiUid);
         if (mApis.end() == mit)
         {
            mpRequestMutex->unlock();
            LOG4CPLUS_WARN(msLogger, "Error: UID repetition");
            return CProfileRepoError(CProfileRepoError::ERROR_UNKNOWN_UID);
         }
         mApis.erase(mit);
         if (saveChanges())
         {
            mpRequestMutex->unlock();
            return CProfileRepoError::NoProfileRepoError();
         }
         else
         {
            mpRequestMutex->unlock();
            return CProfileRepoError(CProfileRepoError::ERROR_DATABASE_WRITE);
         }
      }

      std::string CProfileApiDatabase::getManifest(UID uid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "uid: " + uid.value);
         mpRequestMutex->lock();
         std::map<UID,CProfileApiInfo>::iterator mit = mApis.find(uid);
         if (mApis.end() == mit)
         {
            mpRequestMutex->unlock();
            return std::string("");
         }
         std::stringstream txt;
         txt << std::ifstream(mit->second.xmlPath().c_str()).rdbuf();
         mpRequestMutex->unlock();
         return txt.str();
      }

      void CProfileApiDatabase::printDB()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "Profile API Database path: " + mDBPath);
         LOG4CPLUS_INFO(msLogger, "State: " + convertIntegerToString(static_cast<int>(mDBState)));
         for (std::map<UID,CProfileApiInfo>::const_iterator it = mApis.begin(); mApis.end() != it; ++it)
         {
            it->second.print();
         }
      }
   }
}
