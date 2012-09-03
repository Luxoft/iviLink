/* 
 * 
 * iviLINK SDK, version 1.1.2
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











#ifndef CPROFILEREPOSERVERCLB_HPP_
#define CPROFILEREPOSERVERCLB_HPP_

#include "utils/threads/CSignalSemaphore.hpp"
#include "utils/ipc/CIpc.hpp"
#include "utils/ipc/ICallbackHandler.hpp"
#include "framework/components/ProfileRepository/ProfileRepoInternalTypes.hpp"

#include "utils/misc/Logger.hpp"
#include "CProfileApiDatabase.hpp"
#include "CProfileDatabase.hpp"

namespace iviLink
{
   namespace conf
   {
      class Configurator;
   } // namespace conf

   namespace ProfileRepository
   {

      /**
       * Class is used to handle requests from clients of Profile Repository, and call them in
       * Profile API Database and Profile Database
       */
      class CProfileRepoServerClb   : public Ipc::ICallbackHandler
      {
      public:

         /**
          * Constructor
          */
         CProfileRepoServerClb(conf::Configurator & config);

         /**
          * Destructor
          */
         virtual ~CProfileRepoServerClb();

         /**
          * Callback is called on connection losing event
          */
         virtual void OnConnectionLost(Ipc::DirectionID);

         virtual void OnConnection(Ipc::DirectionID);

         /**
          * Callback is called on incoming requests
          */
         virtual void OnRequest(Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, Ipc::DirectionID);

         /**
          * Function contains loop used to keep connection with Profile Repository Process
          */
         int loop();

      private:

         /**
          * This function calls addProfileApi in database
          */
         void addProfileAPI(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize);

         /**
          * This function calls removeProfileAPI in database
          */
         void removeProfileAPI(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize);

         /**
          * This function calls addProfile in database
          */
         void addProfile(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize);

         /**
          * This function calls removeProfile in database
          */
         void removeProfile(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize);

         /**
          * This function calls addProfileImplementation in database
          */
         void addProfileImplementation(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize);

         /**
          * This function calls removeProfileImplementationPl in database
          */
         void removeProfileImplementationPl(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize);

         /**
          * This function calls findProfiles in database
          */
         void findProfiles(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize);

         /**
          * This function calls getManifest in database
          */
         void getManifest(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize);

         /**
          * This function calls getProfilesList in database
          */
         void getProfilesList(const RepoRequest * req, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize);

         Ipc::CIpc* mpIpc;               ///< Is used to handle requests from clients
         CSignalSemaphore mConLostSem; ///< Semaphore is used to handle connection losing events

         conf::Configurator & mConfig;

         CProfileApiDatabase mApiDB;   ///< Profile API Database
         CProfileDatabase mProfileDB;  ///< Profile Database

         static Logger msLogger;       ///< object of logger
      };

   }

}

#endif /* CPROFILEREPOSERVERCLB_HPP_ */
