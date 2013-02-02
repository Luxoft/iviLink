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


#include <cassert>
#include <string>

#include "CDatabase.hpp"
#include "pugixml.hpp"
#include "CMutex.hpp"

namespace iviLink
{

   namespace AMP
   {
      Logger CDatabase::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppMan.Database.CDatabase"));

      CDatabase::CDatabase(ILaunchInfoProcessor * processor, const std::string& pathToDatabase)
         : mDBPath("AppManDatabase.xml")
         , mDirPath(pathToDatabase)
         , mpMutex(new CMutex)
         , mIdCounter(0)
         , mLaunchInfoProcessor(processor)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         load();
      }

      CDatabase::~CDatabase()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         delete mpMutex;
         delete mLaunchInfoProcessor;
      }

      int CDatabase::genId()
      {
         return ++mIdCounter;
      }

      int CDatabase::getId(const std::string & launchInfo) const
      {
         int res = -1;
         std::string trimmedLaunchInfo = mLaunchInfoProcessor->trimLaunchInfo(launchInfo);
         mpMutex->lock();
         for (std::map<int, CApplication>::const_iterator it = mApplications.begin();
               mApplications.end() != it; ++it)
         {
            if (it->second.launchInfo() == trimmedLaunchInfo)
            {
               res = it->first;
               break;
            }
         }
         mpMutex->unlock();
         return res;
      }

      bool CDatabase::load()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         mpMutex->lock();
         pugi::xml_document doc;
         // Get DOM structure from XML file
         LOG4CPLUS_INFO(msLogger, "DB path: " + mDBPath);
         LOG4CPLUS_INFO(msLogger, "DB directory path: " + mDirPath);
         std::string fullPath = mDirPath + mDBPath;
         pugi::xml_parse_result res = doc.load_file(fullPath.c_str());
         switch (res.status)
         {
         case pugi::status_ok:              // No error
            LOG4CPLUS_INFO(msLogger, "AppMan database parsed by xml-parser");
            break;
         case pugi::status_file_not_found:      // File was not found during load_file()
         case pugi::status_io_error:            // Error reading from file/stream
         case pugi::status_out_of_memory:       // Could not allocate memory
         case pugi::status_internal_error:      // Internal error occurred
            LOG4CPLUS_ERROR(msLogger, "AppMan database open error");
            mpMutex->unlock();
            return false;
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
            LOG4CPLUS_ERROR(msLogger, "AppMan database parsing error");
            mpMutex->unlock();
            return false;
            break;
         default:
            assert(0);
            break;
         }
         loadParsedDatabase(doc);
         saveWithNoLocks();
         mpMutex->unlock();
         return true;
      }

      void CDatabase::loadParsedDatabase(const pugi::xml_document &doc)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         mApplications.clear();
         pugi::xml_node appMan = doc.child("app-man");
         pugi::xml_node applications = appMan.child("applications");
         for (pugi::xml_node_iterator it = applications.begin(); it != applications.end(); ++it)
         {
            LOG4CPLUS_INFO(msLogger, "it->name : " + std::string(it->name()));
            if (strcmp(it->name(), "app"))
            {
               continue;
            }
            LOG4CPLUS_INFO(msLogger, "it->child_value(launch-info) : " + std::string(it->child_value("launch-info")));
            std::string trimmedLaunchInfo = mLaunchInfoProcessor->trimLaunchInfo(it->child_value("launch-info"));
            CApplication app(trimmedLaunchInfo);
            LOG4CPLUS_INFO(msLogger, "launch-info : " + trimmedLaunchInfo);
            if (hasAppWithNoLocks(app.launchInfo()))
            {
               LOG4CPLUS_WARN(msLogger, "!!! Application launch info repetition !!!");
               continue;
            }

            if (!mLaunchInfoProcessor->isLaunchInfoValid(app.launchInfo()))
            {
               LOG4CPLUS_ERROR(msLogger, "!!! LAUNCHINFO IS NOT VALID !!!");
               continue;
            }
            else
            {
               LOG4CPLUS_INFO(msLogger, "launchinfo is valid: " + app.launchInfo());
            }
            int id = genId();
            mApplications[id] = app;

            pugi::xml_node services = it->child("services");

            for (pugi::xml_node_iterator srv = services.begin();
                  services.end() != srv; ++srv)
            {
               if (strcmp(srv->name(),"service") || !strcmp(srv->name(), ""))
               {
                  continue;
               }
               std::string uid(srv->attribute("uid").value());
               LOG4CPLUS_INFO(msLogger, "service UID: " + uid);
               mApplications[id].addService(Service::Uid(uid));
            }
         }
      }

      bool CDatabase::saveWithNoLocks()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         pugi::xml_document doc;
         pugi::xml_node appMan = doc.append_child("app-man");
         pugi::xml_node applications = appMan.append_child("applications");
         for (std::map<int, CApplication>::const_iterator it = mApplications.begin();
               mApplications.end() != it; ++it)
         {
            pugi::xml_node app = applications.append_child("app");
            pugi::xml_node launchInfo = app.append_child("launch-info");
            launchInfo.append_child(pugi::node_pcdata).set_value(it->second.launchInfo().c_str());
            pugi::xml_node services = app.append_child("services");

            std::list<Service::Uid> list = it->second.services();
            for (std::list<Service::Uid>::const_iterator sit = list.begin();
                  list.end() != sit; ++sit)
            {
               pugi::xml_node srv = services.append_child("service");
               srv.append_attribute("uid").set_value(sit->value().c_str());
            }
         }

         std::string fullPath = mDirPath + mDBPath;
         bool result = doc.save_file(fullPath.c_str());
         return result;
      }

      bool CDatabase::save()
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpMutex->lock();
         bool result = saveWithNoLocks();
         mpMutex->unlock();
         return result;
      }

      bool CDatabase::hasAppWithNoLocks(const std::string & launchInfo) const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::string trimmedLaunchInfo = mLaunchInfoProcessor->trimLaunchInfo(launchInfo);
         for (std::map<int, CApplication>::const_iterator it = mApplications.begin();
               mApplications.end() != it; ++it)
         {
            if (it->second.launchInfo() == trimmedLaunchInfo)
            {
               return true;
            }
         }

         return false;
      }

      bool CDatabase::hasApp(const std::string & launchInfo) const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpMutex->lock();
         bool res = hasAppWithNoLocks(launchInfo);
         mpMutex->unlock();
         return res;
      }

      int CDatabase::addApp(const std::string & launchInfo)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "launch info: " + launchInfo);
         std::string trimmedLaunchInfo = mLaunchInfoProcessor->trimLaunchInfo(launchInfo);
         if (hasApp(trimmedLaunchInfo))
         {
            return -1;
         }

         mpMutex->lock();
         int id = genId();
         mApplications[id] = CApplication(trimmedLaunchInfo);
         saveWithNoLocks();
         mpMutex->unlock();
         return id;
      }

      int CDatabase::addApp(const std::string & launchInfo, const std::list<Service::Uid> & services)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         std::string trimmedLaunchInfo = mLaunchInfoProcessor->trimLaunchInfo(launchInfo);
         if (hasApp(trimmedLaunchInfo))
         {
            return -1;
         }

         mpMutex->lock();
         int id = genId();
         mApplications[id] = CApplication(trimmedLaunchInfo, services);
         saveWithNoLocks();
         mpMutex->unlock();
         return id;
      }

      bool CDatabase::removeApp(const std::string & launchInfo)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::string trimmedLaunchInfo = mLaunchInfoProcessor->trimLaunchInfo(launchInfo);

         mpMutex->lock();
         std::map<int, CApplication>::iterator it = mApplications.begin();
         for ( ; mApplications.end() != it; ++it)
         {
            if (it->second.launchInfo() == trimmedLaunchInfo)
            {
               break;
            }
         }

         if (it == mApplications.end())
         {
            mpMutex->unlock();
            return false;
         }
         mApplications.erase(it);
         saveWithNoLocks();
         mpMutex->unlock();
         return true;
      }

      bool CDatabase::removeApp(int id)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpMutex->lock();
         std::map<int, CApplication>::iterator it = mApplications.find(id);
         if (mApplications.end() == it)
         {
            mpMutex->unlock();
            return false;
         }
         mApplications.erase(it);
         saveWithNoLocks();
         mpMutex->unlock();
         return true;
      }

      bool CDatabase::addService(const std::string & launchInfo, const Service::Uid & service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::string trimmedLaunchInfo = mLaunchInfoProcessor->trimLaunchInfo(launchInfo);

         mpMutex->lock();
         std::map<int, CApplication>::iterator it = mApplications.begin();
         for ( ; mApplications.end() != it; ++it)
         {
            if (it->second.launchInfo() == trimmedLaunchInfo)
            {
               break;
            }
         }

         if (it == mApplications.end())
         {
            mpMutex->unlock();
            return false;
         }
         bool res = it->second.addService(service);
         if (res)
         {
            saveWithNoLocks();
         }
         mpMutex->unlock();
         return res;
      }

      bool CDatabase::addService(int id, const Service::Uid & service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         mpMutex->lock();
         std::map<int, CApplication>::iterator it = mApplications.find(id);

         if (it == mApplications.end())
         {
            mpMutex->unlock();
            return false;
         }
         bool res = it->second.addService(service);
         if (res)
         {
            saveWithNoLocks();
         }

         mpMutex->unlock();
         return res;
      }

      bool CDatabase::removeService(const std::string & launchInfo, const Service::Uid & service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::string trimmedLaunchInfo = mLaunchInfoProcessor->trimLaunchInfo(launchInfo);

         mpMutex->lock();
         std::map<int, CApplication>::iterator it = mApplications.begin();
         for ( ; mApplications.end() != it; ++it)
         {
            if (it->second.launchInfo() == trimmedLaunchInfo)
            {
               break;
            }
         }

         if (it == mApplications.end())
         {
            mpMutex->unlock();
            return false;
         }
         bool res = it->second.removeService(service);
         if (res)
         {
            saveWithNoLocks();
         }
         mpMutex->unlock();
         return res;
      }

      bool CDatabase::removeService(int id, const Service::Uid & service)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         mpMutex->lock();
         std::map<int, CApplication>::iterator it = mApplications.find(id);

         if (it == mApplications.end())
         {
            mpMutex->unlock();
            return false;
         }
         bool res = it->second.removeService(service);
         if (res)
         {
            saveWithNoLocks();
         }
         mpMutex->unlock();
         return res;
      }

      bool CDatabase::getServices(const std::string & launchInfo, std::list<Service::Uid> & services) const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::string trimmedLaunchInfo = mLaunchInfoProcessor->trimLaunchInfo(launchInfo);

         mpMutex->lock();
         services.clear();
         std::map<int, CApplication>::const_iterator it = mApplications.begin();
         for ( ; mApplications.end() != it; ++it)
         {
            if (it->second.launchInfo() == trimmedLaunchInfo)
            {
               break;
            }
         }

         if (it == mApplications.end())
         {
            mpMutex->unlock();
            return false;
         }
         services = it->second.services();
         mpMutex->unlock();
         return true;
      }

      bool CDatabase::getServices(int id, std::list<Service::Uid> & services) const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         mpMutex->lock();
         services.clear();
         std::map<int, CApplication>::const_iterator it = mApplications.find(id);

         if (it == mApplications.end())
         {
            mpMutex->unlock();
            return false;
         }
         services = it->second.services();
         mpMutex->unlock();
         return true;
      }

      void CDatabase::getApps(const Service::Uid & service, std::list<std::string> & applications) const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         mpMutex->lock();
         applications.clear();
         for (std::map<int, CApplication>::const_iterator it = mApplications.begin();
               mApplications.end() != it; ++it)
         {
            if (it->second.hasService(service))
            {
               applications.push_back(it->second.launchInfo());
            }
         }
         mpMutex->unlock();
      }

      void CDatabase::getApps(const Service::Uid & service, std::list<int> & appIds) const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

         mpMutex->lock();
         appIds.clear();
         for (std::map<int, CApplication>::const_iterator it = mApplications.begin();
               mApplications.end() != it; ++it)
         {
            if (it->second.hasService(service))
            {
               appIds.push_back(it->first);
            }
         }
         mpMutex->unlock();
      }

      void CDatabase::printWithNoLocks() const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         std::string fullPath = mDirPath + mDBPath;
         LOG4CPLUS_INFO(msLogger, "Database path: " + fullPath);
         for (std::map<int, CApplication>::const_iterator it = mApplications.begin();
               mApplications.end() != it; ++it)
         {
            LOG4CPLUS_INFO(msLogger, "  Id: " + convertIntegerToString(it->first) + ", App : " +it->second.launchInfo());
            std::list<Service::Uid> list = it->second.services();
            for (std::list<Service::Uid>::const_iterator sit = list.begin();
                  list.end() != sit; ++sit)
            {
               LOG4CPLUS_INFO(msLogger, "Service UID : " + sit->value());
            }
         }
      }

      void CDatabase::print() const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpMutex->lock();
         printWithNoLocks();
         mpMutex->unlock();
      }

      bool CDatabase::hasService(int id, const Service::Uid & service) const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpMutex->lock();

         std::map<int, CApplication>::const_iterator it = mApplications.find(id);
         bool res = false;
         if (mApplications.end() != it)
         {
            res = it->second.hasService(service);
         }
         mpMutex->unlock();
         return res;
      }

      void CDatabase::appLaunchInfo(int id, std::string & launchInfo) const
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         mpMutex->lock();
         std::map<int, CApplication>::const_iterator it = mApplications.find(id);
         if (mApplications.end() == it)
         {
            launchInfo = "";
         }
         else
         {
            launchInfo = it->second.launchInfo();
         }
         mpMutex->unlock();
      }


      void CDatabase::addApplicationAndServices(const std::string& launchInfo, const std::list<Service::Uid> & services)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         bool needSave = false;
         std::string trimmedLaunchInfo = mLaunchInfoProcessor->trimLaunchInfo(launchInfo);
         if (!mLaunchInfoProcessor->isLaunchInfoValid(trimmedLaunchInfo))
         {
            LOG4CPLUS_WARN(msLogger, "Invalid launch info provided: " + trimmedLaunchInfo);
            return;
         }
         mpMutex->lock();
         if (!hasAppWithNoLocks(trimmedLaunchInfo))
         {
            LOG4CPLUS_INFO(msLogger, "addApplicationAndServices(): a new application! " + trimmedLaunchInfo);
            needSave = true;
            int id = genId();
            mApplications[id] = CApplication(trimmedLaunchInfo);
         }
         else
         {
            LOG4CPLUS_INFO(msLogger, "addApplicationAndServices(): application already registered! " + trimmedLaunchInfo);
         }
         std::map<int, CApplication>::iterator appsIterator;
         for ( appsIterator = mApplications.begin(); mApplications.end() != appsIterator; ++appsIterator)
         {
            if (appsIterator->second.launchInfo() == trimmedLaunchInfo)
            {
               break;
            }
         }
         std::list<Service::Uid>::const_iterator servicesIterator;
         for (servicesIterator = services.begin(); servicesIterator != services.end(); servicesIterator ++)
         {
            LOG4CPLUS_INFO(msLogger, "addApplicationAndServices(): service " + (*servicesIterator).value());
            needSave = appsIterator->second.addService(*servicesIterator) || needSave;
         }
         if (needSave)
         {
            LOG4CPLUS_INFO(msLogger, "addApplicationAndServices(): database changed! " + trimmedLaunchInfo);
            saveWithNoLocks();
         }
         mpMutex->unlock();
      }


      void CDatabase::deleteByPartialLaunchInfo(const std::string& launchInfo)
      {
         LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
         LOG4CPLUS_INFO(msLogger, "Part of launch info: " + launchInfo);
         bool needSave = false;
         mpMutex->lock();
         std::map<int, CApplication>::iterator appsIterator = mApplications.begin();
         std::map<int, CApplication>::iterator endIter = mApplications.end();
         for (; appsIterator != endIter;) 
         {
            if (appsIterator->second.launchInfo().find(launchInfo) != string::npos) 
            {
               LOG4CPLUS_INFO(msLogger, "Found possible match: " + appsIterator->second.launchInfo());
               if (!mLaunchInfoProcessor->isLaunchInfoValid(appsIterator->second.launchInfo())) 
               {
                  LOG4CPLUS_INFO(msLogger, "App was deleted: " + appsIterator->second.launchInfo());
                  mApplications.erase(appsIterator++);
                  needSave = true;
               }
               else
               {
                  ++appsIterator;
               }
            } else {
               ++appsIterator;
            }
         } 

         if (needSave)
         {
            saveWithNoLocks();
         }
         mpMutex->unlock();
      }

   } // namespace AMP
} // namespace iviLink
