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


#ifndef CDATABASE_HPP_
#define CDATABASE_HPP_

#include <map>

#include "Logger.hpp"
#include "IDatabase.hpp"
#include "CApplication.hpp"
#include "ILaunchInfoProcessor.hpp"

class CMutex;

namespace pugi
{
   class xml_document;
}

namespace iviLink
{

   namespace AMP
   {

      class CDatabase   : public IDatabase
      {
      public:
         /**
          * Contructor
          */
         CDatabase(ILaunchInfoProcessor * processor, const std::string& pathToDatabase);

         /**
          * Loads database from XML database file
          * @retval true if success
          * @retval false if fail
          */
         bool load();

         /**
          * Saves database to XML file
          * @retval true if success
          * @retval false if fail
          */
         bool save();


         /**
          * prints logs with database
          */
         void print() const;

         void addApplicationAndServices(const std::string& launchInfo, const std::list<Service::Uid> & services);

         /**
          * Invoked when a package is removed on Android (may also be used later on Ubuntu when a folder 
          * with binaries is deleted or something like that)
          */
         void deleteByPartialLaunchInfo(const std::string& patrialLaunchInfo);

         /**
          * Destructor
          */
         virtual ~CDatabase();


      private:
         bool saveWithNoLocks();
         //Not implemented
         CDatabase(const CDatabase&);
         CDatabase & operator = (const CDatabase&);

         /**
          * @param launchInfo - string with information needed for launch application
          * @return true if AppMan database has current application
          */
         virtual bool hasApp(const std::string & launchInfo) const;

         /**
          * @param launchInfo - string with information needed for launch application
          * @return true application's been added to database
          */
         virtual int addApp(const std::string & launchInfo);

         /**
          * @param launchInfo - string with information needed for launch application
          * @param services is list of services that app support
          * @return true if success
          */
         virtual int addApp(const std::string & launchInfo, const std::list<Service::Uid> & services);

         /**
          * @param launchInfo - string with information needed for launch application
          * @return true if success
          */
         virtual bool removeApp(const std::string & launchInfo);

         /**
          * @param id is key of app in map
          * @return true if success
          */
         virtual bool removeApp(int id);

         /**
          * @param launchInfo - string with information needed for launch application
          * @param service is UID of service to add
          * @return true if success
          */
         virtual bool addService(const std::string & launchInfo, const Service::Uid & service);
         virtual bool addService(int id, const Service::Uid & service);

         /**
          * @param launchInfo - string with information needed for launch application
          * @param service is UID of service to remove
          * @return true if success
          */
         virtual bool removeService(const std::string & launchInfo, const Service::Uid & service);
         virtual bool removeService(int id, const Service::Uid & service);

         /**
          * @param launchInfo - string with information needed for launch application
          * @param[out] services is list of found service UIDs
          * @return true if success
          */
         virtual bool getServices(const std::string & launchInfo, std::list<Service::Uid> & services) const;
         virtual bool getServices(int, std::list<Service::Uid> & services) const;

         /**
          * @param service is UID of needed service
          * @param[out] is list of app launch infos of applications, that support service
          * @return true if success
          */
         virtual void getApps(const Service::Uid & service, std::list<std::string> & applications) const;
         virtual void getApps(const Service::Uid & service, std::list<int> & appIds) const;

         /**
          * @param id is a key of app in map
          * @param service id UID of service
          * @return true if app has this app
          */
         virtual bool hasService(int id, const Service::Uid & service) const;

         /**
          * Returns launch information by id of application in database
          * @param id is id of app in database
          * @param[out] launchInfo is result of function
          */
         virtual void appLaunchInfo(int id, std::string & launchInfo) const;

         /**
          * Function loads database from pugi::xml_document
          * @param doc is DOM structure with loaded file
          * @return true if success
          */
         void loadParsedDatabase(const pugi::xml_document &doc);

         /**
          * @param launchInfo of application in database
          * @return id of app in database if success of -1 if error
          */
         virtual int getId(const std::string & launchInfo) const;

         /**
          * prints database to logging system without locking mutex
          */
         void printWithNoLocks() const;

         bool hasAppWithNoLocks(const std::string & launchInfo) const;

         int genId();

         std::map<int, CApplication> mApplications;
         std::string mDBPath;
         std::string mDirPath; //path to directory with Database on device
         CMutex * mpMutex;
         int mIdCounter;
         ILaunchInfoProcessor * mLaunchInfoProcessor;

         static Logger msLogger;
      };

   }

}

#endif /* CDATABASE_HPP_ */
