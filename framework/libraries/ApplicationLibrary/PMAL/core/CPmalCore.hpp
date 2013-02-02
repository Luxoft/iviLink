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


#ifndef CPMALCORE_HPP_
#define CPMALCORE_HPP_

#include "IPMALCoreToPIM.hpp"
#include "IPMALCore.hpp"
#include "IPMALIpcToCore.hpp"

#include "Logger.hpp"

namespace iviLink
{

   namespace PMAL
   {

      namespace Test
      {
         class CPmalCoreMockClient;
      }
      class CPmalCore : public IPMALCoreToPIM, public IPMALCore
      {
         friend class Test::CPmalCoreMockClient;

         static Logger logger;
         
      public:
         /**
          * Constructor
          * @param pIpc is pointer to IPC, it can't be NULL
          */
         CPmalCore(IPMALIpcToCore * pIpc);

         virtual ~CPmalCore();

      public:
         // from IPMALCoreToPIM

         virtual CPMALError getProfileLibPath(Profile::Uid id, std::string& path);

         virtual CPMALError isEnabledProfile(Profile::Uid id, bool& enabled);

      public:
         // from IPMALCore

         /**
          * Finds  profiles with given requirements
          * @param id - UID of Profile API or Profile
          * @param profileParameters contains pairs of parameters and values
          * @param[out] profiles - list of returned profiles
          * @param enabledProfiles - if true - only enabled profiles, otherwise all
          * @retval CPMALError::IVILINK_NO_ERROR - success
          * @retval CPMALError::ERROR_OTHER in case of any error (all errors are connected with IPC)
          */
         virtual CPMALError findProfiles(iviLink::BaseUid id,
               const std::map<std::string, std::string> & profileParameters
               , std::list<iviLink::Profile::Uid> & profiles, bool enabledProfiles = true);

         /**
          * Disables profiles by UID of Profile or Profile API
          * @param id - UID of Profile or Profile API
          * @retval CPMALError::IVILINK_NO_ERROR if success
          * @retval CPMALError::ERROR_UNKNOWN_PROFILE_UID if UID isn't in Profile Repository
          * @retval CPMALError::ERROR_OTHER if any other error
          */
         virtual CPMALError disableByUid(iviLink::BaseUid id);

         /**
          * Enables profiles
          * @param uid - UID of Profile of Profile API
          * @retval CPMALError::isNoError() if success
          * @retval CPMALError::ERROR_UNKNOWN_PROFILE_UID if UID isn't in Profile Repository
          * @retval CPMALError::ERROR_OTHER if any other error
          */
         virtual CPMALError enableByUid(iviLink::BaseUid id);


         /**
          * Enables all Profiles in repository except ones prohibited by Manager due to some reasons
          * @retval CPMALError::isNoError() if success
          * @retval CPMALError::ERROR_OTHER in case of IPC errors
          */
         virtual CPMALError enableAll();

         /**
          * Function is used to get information about Profile
          * @param uid - Profile UID or UID of Profile Instance
          * @param[out] profInfo is ProfileInfo structure with information about Profile
          * @retval CPMALError::isNoError() if success
          * @retval CPMALError::ERROR_MANIFEST_PARSING if it's problems with XML parsing
          * @retval CPMALError::ERROR_UNKNOWN_PROFILE_UID in case of unknown Profile
          * @retval CPMALError::ERROR_OTHER in case of IPC and other errors
          */
         virtual CPMALError getProfileInfo(iviLink::Profile::Uid id, ProfileInfo & profInfo);

      private:
         /**
          * Destructor
          */
         
         IPMALIpcToCore * mpIpc; ///< pointer to IPC
      };

   }

}

#endif /* CPMALCORE_HPP_ */
