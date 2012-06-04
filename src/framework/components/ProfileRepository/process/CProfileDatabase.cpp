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





#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>

#include "CProfileDatabase.hpp"
#include "utils/xml/include/pugixml.hpp"
#include "utils/threads/include/CMutex.hpp"

namespace AXIS
{
   namespace ProfileRepository
   {

      Logger CProfileDatabase::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileRepository.Process.CProfileDatabase"));

      CProfileDatabase::CProfileDatabase(std::string dbPath)
         : mDBPath(dbPath)
         , mDBState(EDB_NORMAL_OK)
         , mpDbMutex(new CMutex)
         , mpRequestMutex(new CMutex)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         loadDatabase();
         printDB();
      }

      CProfileDatabase::~CProfileDatabase()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         delete mpDbMutex;
         delete mpRequestMutex;
      }

      void CProfileDatabase::loadDatabase()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mProfiles.clear();
         pugi::xml_document doc;
         mpDbMutex->lock();
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

      bool CProfileDatabase::loadParsedDatabase(const pugi::xml_document &doc)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         if (EDB_NORMAL_OK != mDBState)
         {
            return false;
         }
         pugi::xml_node profs = doc.child("profiles");
         for (pugi::xml_node_iterator it = profs.begin(); it != profs.end(); ++it)
         {
            CProfileInfo info(std::string(it->child_value("manifest-path")));
            if (info.failed())
            {
               LOG4CPLUS_WARN(msLogger, "Failed loading of XML Manifest");
               continue;
            }
            std::map<UID,CProfileInfo>::iterator mit = mProfiles.find(info.uid());
            if (mProfiles.end() == mit)
            {
               mProfiles[info.uid()] = info;
               pugi::xml_node libs = it->child("libs");
               for (pugi::xml_node_iterator sit = libs.begin(); sit != libs.end(); ++sit)
               {
                  mProfiles[info.uid()].addLib(sit->attribute("platform").value(), sit->attribute("path").value());
               }
            }
            else if ( 0 != it->child_value("uid"))
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

      bool CProfileDatabase::saveChanges()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         pugi::xml_document doc;
         pugi::xml_node profiles = doc.append_child("profiles");
         mpDbMutex->lock();
         for (std::map<UID,CProfileInfo>::const_iterator it = mProfiles.begin(); it != mProfiles.end(); ++it)
         {
            pugi::xml_node profile = profiles.append_child("profile");
            pugi::xml_node maniPath = profile.append_child("manifest-path");
            maniPath.append_child(pugi::node_pcdata).set_value(it->second.xmlPath().c_str());
            pugi::xml_node libsNode = profile.append_child("libs");
            for (std::map<std::string,std::string>::const_iterator sit = it->second.libs().begin(); sit != it->second.libs().end(); ++sit)
            {
               pugi::xml_node libNode = libsNode.append_child("lib");
               libNode.append_attribute("platform").set_value(sit->first.c_str());
               libNode.append_attribute("path").set_value(sit->second.c_str());
            }
         }
         bool result = doc.save_file(mDBPath.c_str());
         mpDbMutex->unlock();
         if (result)
         {
            mDBState = EDB_SAVE_ERROR;
         }
         printDB();
         return result;
      }

      CError CProfileDatabase::addProfile(const std::string xmlManifestPath)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         CProfileInfo info(xmlManifestPath);
         mpRequestMutex->lock();
         std::map<UID,CProfileInfo>::iterator mit = mProfiles.find(info.uid());
         if (mProfiles.end() != mit)
         {
            mpRequestMutex->unlock();
            LOG4CPLUS_WARN(msLogger, "Error: UID repetition");
            return CProfileRepoError(CProfileRepoError::ERROR_UID_ALREADY_EXISTS);
         }
         else
         {
            mProfiles[info.uid()] = info;
         }
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

      CError CProfileDatabase::removeProfile(UID profileUid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpRequestMutex->lock();
         std::map<UID,CProfileInfo>::iterator mit = mProfiles.find(profileUid);
         if (mProfiles.end() == mit)
         {
            mpRequestMutex->unlock();
            return CProfileRepoError(CProfileRepoError::ERROR_UNKNOWN_UID);
         }
         else
         {
            for (std::map<std::string,std::string>::const_iterator it = mit->second.libs().begin(); it != mit->second.libs().end(); ++it)
            {
               mit->second.removeLib(it->first);
            }
            mProfiles.erase(mit);
         }
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

      CError CProfileDatabase::addProfileImplementation(UID profileID, const LibDescriptor& library)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpRequestMutex->lock();
         std::map<UID,CProfileInfo>::iterator mit = mProfiles.find(profileID);
         if (mProfiles.end() == mit)
         {
            mpRequestMutex->unlock();
            return CProfileRepoError(CProfileRepoError::ERROR_UNKNOWN_UID);
         }
         else
         {
            mit->second.addLib(library.platform,library.libPath);
         }
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

      CError CProfileDatabase::removeProfileImplementation(UID profileUid, std::string platform)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpRequestMutex->lock();
         std::map<UID,CProfileInfo>::iterator mit = mProfiles.find(profileUid);
         if (mProfiles.end() == mit)
         {
            mpRequestMutex->unlock();
            return CProfileRepoError(CProfileRepoError::ERROR_UNKNOWN_UID);
         }
         bool res = mit->second.removeLib(platform);
         if (!res)
         {
            mpRequestMutex->unlock();
            return CProfileRepoError(CProfileRepoError::ERROR_NO_DLL);
         }
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

      CError CProfileDatabase::removeProfileImplementation(UID profileUid, LibDescriptor ld)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         return removeProfileImplementation(profileUid,ld.platform);
      }

      std::list<LibInfo> CProfileDatabase::findProfiles(UID id, const std::map<std::string, std::string> & profileArguments, std::string platform)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpRequestMutex->lock();
         LOG4CPLUS_INFO(msLogger, "- CProfileDatabase::findProfiles() Find UID : " + id.value);
         std::list<LibInfo> result;
         for (std::map<UID,CProfileInfo>::iterator pit = mProfiles.begin(); mProfiles.end() != pit; ++pit)
         {
            CProfileInfo &info = pit->second;
            LOG4CPLUS_INFO(msLogger, "-- UID: "+ info.uid().value + ", API UID : " + info.apiUid().value);
            if ((info.uid() == id || info.apiUid() == id)/* && (platform == "" || "" != info.lib(platform))*/)
            {
               int rel = getRelevance(info,profileArguments);
               if (!rel)
               {
                  continue;
               }
               if ("" == platform)
               {
                  for (std::map<std::string,std::string>::const_iterator libIt = info.libs().begin(); info.libs().end() != libIt; ++libIt)
                  {
                     LibInfo libInfo(info.uid(),libIt->first, libIt->second,rel);
                     result.push_back(libInfo);
                  }
               }
               else
               {
                  std::string rPath = info.lib(platform);
                  if ("" != rPath)
                  {
                     LibInfo libInfo(info.uid(),platform,rPath);
                     result.push_back(libInfo);
                  }
               }
            }
         }
         mpRequestMutex->unlock();
         /// todo: sort results by relevance (add operator < to to LibInfo structure) (after first AXIS implementation, this task is on future)
         LOG4CPLUS_WARN(msLogger, "Add sorting results by relevance");
         return result;
      }


      int CProfileDatabase::getRelevance(const CProfileInfo & proInfo, const std::map<std::string, std::string> & profileArguments)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         for (std::map<std::string, std::string>::const_iterator it = profileArguments.begin(); profileArguments.end() != it; ++it)
         {
            std::map<std::string,std::string>::const_iterator fit = proInfo.attributes().find(it->first);
            if (fit == proInfo.attributes().end() || fit->second != it->second)
            {
               LOG4CPLUS_INFO(msLogger, "returned 0");
               return 0;
            }
         }
         LOG4CPLUS_INFO(msLogger, "returned 100");
         return 100;
      }

      std::string CProfileDatabase::getManifest(UID uid)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpRequestMutex->lock();
         std::map<UID,CProfileInfo>::iterator mit = mProfiles.find(uid);
         if (mProfiles.end() == mit)
         {
            mpRequestMutex->unlock();
            return std::string("");
         }
         std::stringstream txt;
         txt << std::ifstream(mit->second.xmlPath().c_str()).rdbuf();
         mpRequestMutex->unlock();
         return txt.str();
      }

      std::list<UID> CProfileDatabase::getProfilesList()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpRequestMutex->lock();
         std::list<UID> list;
         for (std::map<UID,CProfileInfo>::const_iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
         {
            list.push_back(it->first);
         }
         mpRequestMutex->unlock();
         return list;
      }

      void CProfileDatabase::printDB()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "Profile Database path: " + mDBPath);
         LOG4CPLUS_INFO(msLogger, "State: " + convertIntegerToString(static_cast<int>(mDBState)));
         for (std::map<UID,CProfileInfo>::const_iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
         {
            it->second.print();
         }
      }
   }
}
