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


#ifndef C_SERVICE_HPP
#define C_SERVICE_HPP

#include "Logger.hpp"
#include "ProfileHolder.hpp"
#include "ServiceManagerCommon.hpp"

#include "AppInfo.hpp"
#include "PublicError.hpp"

class CMutex;

namespace iviLink
{
   namespace Service
   {

      class CService
      {
         public:

         //Types section
            typedef std::map<Profile::ApiUid, Profile::ProfileHolder*> tProfilesMap;
            typedef std::set<Profile::ApiUid> tProfilesSet;

         //Methods  section
         /**
          * Constructor
          * @param xmlPath is location of services XMLs
          */
         CService(const std::string &xmlPath, iviLink::Android::AppInfo appInfo, const Service::Uid & service);

         /**
          * Destructor
          */
         ~CService();

         /**
          * return the unique class name
          * @return  the unique class Name
          */
         const Service::Uid & getUid() const;


         /**
          * return the unique class name
          * @return  the unique class Name
          */
         const Service::Uid & getPairUid() const;

         /**
          * Loads service
          * @param cbMap is map with Profile callbacks. Must have callbacks for profiles that are part of
          * given service
          * @return true if success
          */
         iviLink::Error load(tCallbacksMap & cbMap);

         /**
          * Unloads service
          */
         void unload();

         /**
          * Function is used to load service from incoming request from anoter side
          * @param cbMap is map with profile callbacks
          * @return true if success
          */
         bool incomingLoad(tCallbacksMap & cbMap);

         /**
          * Called when profile on the other side is set up and paired profile needed
          * @param uid  UID of profile that should be loaded
          * @param papiUid api uid of profile that should be loaded
          * @return  ERR_OK or error code
          */
         ERROR_CODE incomingProfileRequest(iviLink::Profile::Uid const& uid,
               iviLink::Profile::ApiUid const& papiUid);

         /**
          * Function is used to handle situation when profile died on another side
          */
         void profileDied(const Profile::IUid &instance);

         /**
          * Returns true if all profiles in service are active
          */
         bool allLoaded() const;

         /**
          * Returns true if all profiles of service are inactive
          */
         bool allDied() const;

         /*
          * Returns true if there were no profile dies
          */
         bool correctState() const;

         /**
          * Checks whether service contains profile API with given Profile API UID, gives it to client,
          * locks it waits for releaseProfile()
          * @param profileApi is UID of Profile API
          * @return  pointer to profile API or NULL
          */
         BaseProfileApi * getProfile(const Profile::ApiUid &profileApi);

         /**
          * unlocks profile
          */
         void releaseProfile(const Profile::ApiUid &profileApi);

         /**
          * Returns true if has locked profiles that need to be unloaded
          */
         bool hasUnloadWaits() const;

      private:

         // Methods section

         /**
          * Assigning operator
          *
          * Unimplemented to avoid missusing
          *
          * @param classRef The reference to the object the class is assiging from
          */
         CService& operator=(const CService& );

         /**
          * Copying constructor
          * @param classRef The reference to the object the class is constructing on
          */
         CService(const CService& rhs);

         // Members section
         std::string                                  mManifestPath;    ///< path to XML manifest of Service
         std::string                                  mXmlPath;         ///< path of XML location
         iviLink::Android::AppInfo                    mAppInfo;
         iviLink::Service::Uid                        mUid;             ///< Service UID
         iviLink::Service::Uid                        mPairUid;         ///< Service pair UID
         tProfilesMap                                 mProfilesMap;     ///< Map with loaded profiles
         CMutex *                                     mpProfilesMapMutex;///< Mutex for profiles map
         tCallbacksMap                                mCbMap;           ///< map with profiles callbacks
         CMutex *                                     mpCbMapMutex;     ///< mutex for callback map
         static Logger                                msLogger;         ///< object with logger
         bool                                         mCorrect;         ///< true if all profiles are active
         tProfilesSet                                 mUnloadRequests;  ///< set of locked profile uids that wait unload
      };
   }
}
#endif
