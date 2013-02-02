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


#ifndef CSERVICE_MANAGER_HPP
#define CSERVICE_MANAGER_HPP

#include "CMutex.hpp"
#include "IProfileManagersCallbacks.hpp"
#include "ServiceManagerCommon.hpp"
#include "AppLibCommon.hpp"
#include "Logger.hpp"

#include "AppInfo.hpp"
#include "PublicError.hpp"

namespace iviLink
{
   namespace Service
   {
      class CService;

      /**
       * This class provides service manager functionality to service manager clients. It's used to
       * load/unload services and provide access to its functionality
       */
      class CServiceManager: public PMAL::IProfileManagerCallbacks
      {
      public:

         /// IServiceClientCallbacks interface
         /**
          * This interface should be implemented by service consumer in client application.
          * Service manager uses this callbacks to notify client about
          */

         class IClientCallbacks
         {
         public:

            /**
             * Callback for service created on the other side
             * @param service is Service UID
             */
            virtual void serviceRequestCallback(const Service::Uid & service) = 0;

            /**
             * Callback is called after service loading
             * @param service is Service UID
             */
            virtual void serviceLoaded(const Service::Uid & service) = 0;

            /**
             * Callback is called in case of error in service loading
             * @param service is UID of service
             */
            virtual void serviceLoadError(const Service::Uid & service) = 0;

            /**
             * Callback for service dropped on the other side
             * @param servicde is UID of service
             */
            virtual void serviceDropped(const Service::Uid & service) = 0;

         };

         //  Members section

         // Methods section
         /**
          * Interface function to provide sigleton behavior
          * @return pointer to existing instance of singleton and creates it if not exists
          */

         static CServiceManager* getInstance(
                       iviLink::Android::AppInfo appInfo = iviLink::Android::AppInfo());

         /**
          * Interface function to destroy the singleton instance
          */
         static void deleteInstance();

         /**
          * Registers service consumer as a callbacks receiver
          * @param client pointer to consumer
          */
         void registerClient(CServiceManager::IClientCallbacks* pClient);

         /**
          * Registers callback that used by profiles
          * @param profileApi is Profile API UID
          * @param pCallbacks is pointer of instance of callbacks receiver
          */
         void registerProfileCallbacks(const Profile::ApiUid &profileApi,
               Profile::IProfileCallbackProxy* pCallbacks);

         /**
          * Loads service
          * @param service is UID of service
          * @return true if success
          */
         iviLink::Error load(const Service::Uid & service);

         /**
          * Unloads service
          * @param pUID UID service to load
          */
         bool unload(const Service::Uid & service);

         /**
          * Function returns list of active services
          */
         void getActive(Service::ListOfUids &services);

         /**
          * Returns true if service with given UID is active
          */
         bool isActive(const Service::Uid &service);

         /**
          * Returns pointer on instance of loaded Profile and locks profile
          * @param service is UID of service
          * @param profileApi is UID of Profile API of needed profile
          */
         BaseProfileApi * getProfile(const Service::Uid &service, const Profile::ApiUid &profileApi);

         /**
          * Is used to unlock profile in service manager
          */
         void releaseProfile(const Service::Uid &service, const Profile::ApiUid &profileApi);

      private:
         //> from IProfileManagerCallbacks
         /**
          * Callback for service created on the other side
          * @param UID service uid
          */
          virtual BaseError serviceRequestCallback(iviLink::Service::Uid const& serviceUID);

         /**
          * Callback that should be invoked when paired profile requested from other side
          * @param serviceUID service that should own this profile
          * @param profileUID profile that should be created
          */
         virtual BaseError incomingProfile(iviLink::Service::Uid const& serviceUID,
            iviLink::Profile::Uid const& profileUid,
            iviLink::Profile::ApiUid const& papiUid);

         /**
          * Callback for service dropped on the other side
          * @param serviceUID
          */
         virtual void serviceDropped(iviLink::Service::Uid const& serviceUID);

         /**
          * Callback that should be invoked when paired profile from other side died
          * @param serviceUID service that owns this profile
          * @param piuid is the profile instance id of dead profile
          */
         virtual void profileDied(iviLink::Service::Uid const& serviceUID,
               iviLink::Profile::IUid const& piuid);


         // Methods section
         /**
          * Constructor is private to provide singleton behavior
          */
         CServiceManager(iviLink::Android::AppInfo appInfo = iviLink::Android::AppInfo());

         /**
          * Destructor
          */
         virtual ~CServiceManager();

         /**
          * Loads services database
          */
         void loadDb();

         /**
          * Inits PMAL (Profile Manager Application Library)
          * @return CError::NoError() if success
          */
         BaseError initPmal();

         /**
          * Function is used in case of incoming service request
          * @param service is UID of service
          * @return true if success
          */
         bool incomingLoad(const Service::Uid & service);

         /**
          * Copying constructor
          *
          * Unimplemented to avoid missusing
          *
          * @param classRef The reference to the object the class is constructing on
          */
         CServiceManager(const CServiceManager& );

         /**
          * Assigning operator
          *
          * Unimplemented to avoid missusing
          *
          * @param classRef The reference to the object the class is assiging from
          */
         CServiceManager& operator=(const CServiceManager& );

         bool isServiceAlreadyLoaded(const Service::Uid & service);

      private:

         // Members section
         static Logger              msLogger;               ///< static object of logger
         static CServiceManager *   mspInstance;            ///< pointer on instance of service manager
         static CMutex              msSingletonMutex;       ///< mutex used in getInstance()
         IClientCallbacks *         mpClient;               ///< pointer to client of service manager
         tServiceSet                mSystemServices;        ///< set of services that supports current system
         CMutex                     mSystemServicesMutex;    ///< mutex for system services set
         tServiceMap                mActiveServices;        ///< map of active services
         CMutex                     mActiveServicesMutex;   ///< mutex for map of active services
         tCallbacksMap              mCallbacks;             ///< map with profile callbacks
         CMutex                     mCallbacksMutex;        ///< mutex for map with profile callbacks
         std::string                mXmlPath;               ///< path to folder with service manager XMLs
         iviLink::Android::AppInfo  mAppInfo;
         
      };
   }
}

#endif
