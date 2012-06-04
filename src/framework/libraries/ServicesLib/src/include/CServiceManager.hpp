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




#ifndef CSERVICE_MANAGER_HPP
#define CSERVICE_MANAGER_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <map>
#include <vector>
#include <string>
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/misc/include/Types.hpp"
#include "utils/threads/include/CMutex.hpp"
#include "framework/components/ProfileManager/PMAL/include/IProfileManagersCallbacks.hpp"
#include "utils/misc/include/UID.hpp"
#include "utils/misc/include/CError.hpp"
#include "utils/misc/include/Logger.hpp"
#include "framework/libraries/ServicesLib/src/include/CGenericService.hpp"

using AXIS::PMAL::IProfileManagerCallbacks;

class CGenericService;

class CServiceManager: public IProfileManagerCallbacks
{
public:

   //Types section

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
       * @param UID service uid
       */
      virtual void serviceRequestCallback(AXIS::Service::Uid const& serviceUID) = 0;

      /**
       * Callback that should be invoked when paired profile requested from other side
       * @param serviceUID service that should own this profile
       * @param profileUID profile that should be created
       */
      virtual void incomingProfileRequest(AXIS::Service::Uid const& serviceUID, AXIS::Profile::Uid const& profileUid, AXIS::Profile::ApiUid const& papiUid) =0;

      /**
       * Callback for service dropped on the other side
       * @param UID service uid
       */
      virtual void serviceDropped(AXIS::Service::Uid const& serviceUID) = 0;

     /**
       * Callback that should be invoked when paired profile from other side died
       * @param serviceUID service that should own this profile
       * @param profileUID profile that should be created
       */

      virtual void profileDied(AXIS::Service::Uid const& serviceUID, AXIS::Profile::IUid const& piuid) =0;
   };

   //  Members section
   static const char*                                          mXMLPath;

   // Methods section
   /**
    * Interface function to provide sigleton behavior
    * @return pointer to existing instance of singleton and creates it if not exists
    */
   static CServiceManager* getInstance();

   /**
    * Interface function to destroy the singleton instance
    */
   void deleteInstance();

   /**
    * Registers service consumer as a callbacks receiver
    * @param client pointer to consumer
    */
   void registerClient(CServiceManager::IClientCallbacks* client);

   /**
    * Unregisters service consumer as a callbacks receiver
    * @param client pointer to consumer
    */
   void unregisterClient(CServiceManager::IClientCallbacks* client);

   /**
    * Loads service and return it's instance to caller
    * @param pUID UID service to load
    * @return pointer to service instance
    */
   CGenericService* loadService(const char* pUID, CGenericService::tProfileCallbacksMap & cbMap);

   /**
    * Unloads service 
    * @param pUID UID service to load
    */
   void dropService(const char* serviceUID);

public:
   //> from IProfileManagerCallbacks
   /**
    * Callback for service created on the other side
    * @param UID service uid
    */
    virtual CError serviceRequestCallback(AXIS::Service::Uid const& serviceUID);
   
   /**
    * Callback that should be invoked when paired profile requested from other side
    * @param serviceUID service that should own this profile
    * @param profileUID profile that should be created
    */
   virtual CError incomingProfile(AXIS::Service::Uid const& serviceUID, 
      AXIS::Profile::Uid const& profileUid, 
      AXIS::Profile::ApiUid const& papiUid);

   /**
    * Callback for service dropped on the other side
    * @param serviceUID
    */
   virtual void serviceDropped(AXIS::Service::Uid const& serviceUID);

   /**
    * Callback that should be invoked when paired profile from other side died
    * @param serviceUID service that owns this profile
    * @param piuid is the profile instance id of dead profile
    */
   virtual void profileDied(AXIS::Service::Uid const& serviceUID, AXIS::Profile::IUid const& piuid);

private:

   // Methods section
   /**
    * Constructor is private to provide sigleton behavior
    */
   CServiceManager();

   /**
    * Destructor
    */
   virtual ~CServiceManager();

   CError initPmal();

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

private:

   // Members section
   static CServiceManager*                                              mpSelf;
   static CMutex                                                        mSingletonMutex;
   std::map<std::string, CGenericService* >                             mServicesProtoMap;
   CMutex                                                               mClientsMutex;
   std::vector<CServiceManager::IClientCallbacks* >                     mClients;
   static Logger                                                        msLogger;
};

#endif
