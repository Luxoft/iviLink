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







#ifndef CPROFILEREPOCLIENT_HPP_
#define CPROFILEREPOCLIENT_HPP_

#include <list>

#include "utils/misc/Logger.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/ipc/CIpc.hpp"
#include "utils/ipc/ICallbackHandler.hpp"

#include "RepoTypes.hpp"
#include "ProfileRepoInternalTypes.hpp"
#include "CProfileRepoError.hpp"
#include "IProfileRepoClient.hpp"

namespace iviLink
{
   namespace conf
   {
      class Configurator;
   } // namespace conf

   namespace ProfileRepository
   {

      /**
       * Class is used as client of Profile Repository. Is used to make requests to Profile Repository
       * and returns results of this requests
       */
      class CProfileRepoClient   : public Ipc::ICallbackHandler
                                 , public IProfileRepoClient
      {
      public:

         /**
          * Constructor
          */
         CProfileRepoClient(conf::Configurator * pConfig = 0);

         /**
          * Destructor
          */
         virtual ~CProfileRepoClient();

         /**
          * Function contains loop used to keep connection with Profile Repository Process
          */
         void loop();

         /**
          * Calls to disconnect from Profile Repository Process
          */
         void disconnect();

         /**
          * Addes Profile API to repository. Calls function with the same name in CProfileApiDatabase class
          * @param manifestPath contains path of manifest XML
          * @return CError code
          */
         virtual CError addProfileApi(const std::string &manifestPath);

         /**
          * Removes profile API from repository. Calls function with the same name in CProfileApiDatabase class
          * @param profileApiUid string is UID of Profile API
          * @return CError code
          */
         virtual CError removeProfileApi(UID profileApiUid);

         /**
          * Removes entire profile with all implementations from repository. Calls function with the same name in CProfileDatabase class
          * @param profileUid - UID of profile
          * @return CError code
          */
         virtual CError addProfile(const std::string &xmlManifestPath);

         /**
          * Removes entire profile with all implementations from repository. Calls function with the same name in CProfileDatabase class
          * @param profileUid - UID of profile
          * @return CError code
          */
         virtual CError removeProfile(UID profileUid);

         /**
          * Addes Profile implementation to repository. Calls function with the same name in CProfileDatabase class
          * profileID - UID of Profile
          * @param library - lib descriptor of Profile Implementation library
          * @return CError code
          */
         virtual CError addProfileImplementation(UID profileID, const LibDescriptor& library);

         /**
          * Removes single profile implementation from repository. Calls function with the same name in CProfileDatabase class
          * @param profileUid - UID of profile
          * @param platform - information about target platform
          * @return CError code
          */
         virtual CError removeProfileImplementation(UID profileUid, const std::string &platform);

         /**
          * finds profiles with given requirements. Calls function with the same name in CProfileDatabase class
          * @param id is UID of Profile API or Profile
          * @param profileParameters contains pairs of parameters and values
          * @param[out] implementations is map with pair Profile UID and LibDescriptor that contains information about platform and library path
          * @param platform - information about target platform, if platform == "" - result will be returned for all platforms
          * @return CError code
          */
         virtual CError findProfiles(UID id,
               const std::map<std::string, std::string> & profileArguments,
               std::list<LibInfo> &implementations, const std::string &platform);

         /**
          * Calls function with the same name in CProfileDatabase class
          * @param uid - UID of Profile or Profile API
          * @return manifest XML description or empty string if wrong UID
          */
         virtual std::string getManifest(UID uid);

         /**
          * Calls function with the same name in CProfileDatabase class
          * @return list of profile UIDs from database
          */
         virtual std::list<UID> getProfilesList();

         /**
          * @retval true if connected to Profile Repository
          * @retval false if it's no connection with Profile Repository
          */
         bool checkConnection() const;

      private:

         /**
          * Callback is used to inform about connection establishing
          */
         virtual void OnConnection(Ipc::DirectionID);

         /**
          * Callback is used to inform about connection losing
          */
         virtual void OnConnectionLost(Ipc::DirectionID);

         /**
          * Callback is used to handle requests
          */
         virtual void OnRequest(Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
               UInt8* pResponseBuffer, UInt32& bufferSize, Ipc::DirectionID);

         /**
          * Returns instance of CError class by error code
          */
         CError errorByCode(UInt32 errCode);

         /**
          * Is used to send requests to Profile Repository Process
          */
         bool makeRequest(const RepoRequest * req, UInt8* const pResponseBuffer, UInt32& bufferSize);

         /**
          * Returns generated message ID
          */
         Ipc::MsgID getId();

         Ipc::CIpc * mpIpc;            ///< Is used to make requests to Profile Repository Process
         Ipc::MsgID mGId;              ///< Contains last generated message ID
         CSignalSemaphore mConLostSem; ///< Semaphore is used to handle connection events
         bool mBe;                     ///< Flag, false if you want to break connection
         volatile bool mNoConnection;  ///< Indicates connection losing
         mutable CMutex mConnMutex;    ///< for mNoConnection

         static Logger msLogger;       ///< object of logger
      };

   }

}

#endif /* CPROFILEREPOCLIENT_HPP_ */
