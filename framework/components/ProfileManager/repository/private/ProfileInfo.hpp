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


#ifndef CPROFILEINFO_HPP_
#define CPROFILEINFO_HPP_

#include <string>
#include <map>

#include "Logger.hpp"
#include "RepositoryTypes.hpp"

namespace iviLink
{
   namespace PMP
   {
      /**
       * Class is used to present information about Profile in Profile Repository
       */
      class CProfileInfo
      {
      public:
         /**
          * Constructor
          * @param xmlPath path to manifest, constructor parses this manifest
          */
         CProfileInfo(std::string xmlPath);

         /**
          * Constructor
          */
         CProfileInfo();

         /**
          * Destructor
          */
         ~CProfileInfo();

         /**
          * @return path to manifest
          */
         std::string xmlPath() const;

         /**
          * @return UID of Profile
          */
         Profile::Uid uid() const;

         /**
          * @return name of Profile
          */
         std::string name() const;

         /**
          * @return version of Profile
          */
         int version() const;

         /**
          * @return UIA of implemented Profile API
          */
         Profile::ApiUid apiUid() const;

         /**
          * @return map of Profile attributes
          */
         const std::map<std::string,std::string> &attributes() const;

         /**
          * @return map of Profile Implementations
          */
         const std::map<std::string,std::string> &libs() const;

         /**
          * @param platform is name of profile implementation platform
          * @return Profile Implementation by name of platform
          */
         std::string lib(std::string platform);

         /**
          * Adds Profile Implementation to repository
          * @param platform is name of Profile Implementation platform
          * @param libPath is path to library that is profile implementation
          * @return true if success
          */
         bool addLib(std::string platform, std::string libPath);

         /**
          * Removes profile implementation from Profile Repository
          * @return true if success
          */
         bool removeLib(std::string platform);

         /**
          * Prints Profile information to log
          */
         void print() const;

         /**
          * @return true if Profile has incorrect state
          */
         bool failed() const;

      private:
         /**
          * Loads information about profile from manifest
          */
         void loadProfileInfo();

         std::string mXmlPath;              ///< Path to manifest
         Profile::Uid mUid;                 ///< UID of Profile
         std::string mName;                 ///< Name of Profile
         int mVersion;                      ///< Version of Profile
         Profile::ApiUid mApiUid;           ///< UID of implemented Profile API
         std::map<std::string, std::string> mAttributes; ///< Map of Profile Attributes
         std::map<std::string, std::string> mLibs;       ///< Map of Profile Implementations
         bool mFailed;                                   ///< If true - Profile state is incorrect

         static Logger msLogger;                         ///< object of logger
      };
   }
}

#endif /* CPROFILEINFO_HPP_ */
