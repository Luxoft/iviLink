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

/***************************************************************************
 * Project           AXIS
 * (c) copyright     2012
 * Company           LUXOFT
 * @file             CServiceManager.cpp
 * @brief            The CServiceManager class implementation file
 * @author           KSkidanov
 ***************************************************************************/
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <assert.h>
#include <cstring>
#include <algorithm>
#include "stdio.h"
/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "framework/libraries/ServicesLib/src/include/CGenericService.hpp"

/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "framework/libraries/ServicesLib/src/include/CServiceManager.hpp"
/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "utils/misc/include/logging.hpp"
#include "utils/xml/include/pugixml.hpp"
#include "framework/libraries/ServicesLib/src/include/CClimateService.hpp"
#include "framework/libraries/ServicesLib/src/include/CVehicleSystemService.hpp"
#include "framework/libraries/AppMan/App/include/CAppManConnectController.hpp"
#include "framework/components/ProfileManager/PMAL/include/CComponentMgr.hpp"
#include "framework/components/ProfileManager/PMAL/include/CPMALError.hpp"

using AXIS::PMAL::CPMALError;
using AXIS::PMAL::CPMALComponentMgr;
using AXIS::AppMan::CAppManConnectController;

static const char gModuleName[] = "CServiceManager";

CMutex CServiceManager::mSingletonMutex;
CServiceManager* CServiceManager::mpSelf = 0;

const char* CServiceManager::mXMLPath = SERVICE_REPOSITORY  "/";


Logger CServiceManager::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ServicesLib.ServiceManager"));

CServiceManager* CServiceManager::getInstance()
{
   mSingletonMutex.lock();
   if (mpSelf == NULL)
   {
      mpSelf = new CServiceManager();
   }
   mSingletonMutex.unlock();
   return mpSelf;
}

void CServiceManager::deleteInstance()
{
   mSingletonMutex.lock();
   delete mpSelf;
   mpSelf = NULL;
   mSingletonMutex.unlock();
}

CServiceManager::CServiceManager()
{
   LOG4CPLUS_TRACE(msLogger, "CServiceManager::CServiceManager()");
   std::string path = (std::string)CServiceManager::mXMLPath + "HeadUnitServices.xml";
   pugi::xml_document doc;

   LOG4CPLUS_TRACE(msLogger, "path = " + path);

   pugi::xml_parse_result result = doc.load_file(path.c_str());
   CGenericService::tProfileCallbacksMap cbMap;
   if (result)
   {
      pugi::xml_node device_node = doc.child("device");
      for (pugi::xml_node service_node = device_node.child("service"); service_node; service_node = service_node.next_sibling("service"))
      {
         if(strcmp(service_node.attribute("name").value(), "ClimateServiceHUI") == 0)
         {
            mServicesProtoMap.insert(std::pair<std::string ,CGenericService* >("ClimateServiceHUI", new CClimateService("ClimateServiceHUI", cbMap) ));
         }
         else if(strcmp(service_node.attribute("name").value(), "ClimateServiceClient") == 0)
         {
            mServicesProtoMap.insert(std::pair<std::string ,CGenericService* >("ClimateServiceClient", new CClimateService("ClimateServiceClient",cbMap)));
         }
         else if(strcmp(service_node.attribute("name").value(), "VehicleSystemHUI") == 0)
         {
            mServicesProtoMap.insert(std::pair<std::string ,CGenericService* >("VehicleSystemHUI", new CVehicleSystemService("VehicleSystemHUI",cbMap)));
         }
         else if(strcmp(service_node.attribute("name").value(), "VehicleSystemClient") == 0)
         {
            mServicesProtoMap.insert(std::pair<std::string ,CGenericService* >("VehicleSystemClient", new CVehicleSystemService("VehicleSystemClient",cbMap)));
         }         
      }

      LOG4CPLUS_INFO(msLogger, "mServicesProtoMap == begin ==");
      for (std::map<std::string, CGenericService* >::const_iterator it = mServicesProtoMap.begin(); 
         it != mServicesProtoMap.end(); ++it)
      {
         LOG4CPLUS_INFO(msLogger, it->first + "  " + convertIntegerToString((intptr_t)it->second));
      }
      LOG4CPLUS_INFO(msLogger, "mServicesProtoMap == end ==");
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger, "xml error: " + std::string(result.description()));
   }

   CError err = initPmal();
   if(err.isNoError())
   {
      LOG4CPLUS_TRACE(msLogger, "CServiceManager::CServiceManager()=> Service Manager created!");   
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger, "CServiceManager::CServiceManager()=> PMAL component manager creation error "
         + static_cast<std::string>(err));   
   }
}

CServiceManager::~CServiceManager()
{
   for(std::map<std::string, CGenericService* >::iterator iter =mServicesProtoMap.begin(); iter!=mServicesProtoMap.end(); ++iter)
   {
      delete iter->second;
      iter->second = NULL;
   }
}

CError CServiceManager::initPmal()
{
   CPMALError err = CPMALComponentMgr::create(this, NULL);
   if (!err.isNoError())
   {
      return err;
   }
   CPMALComponentMgr* pPmalMgr = CPMALComponentMgr::getInstance();

   CAppManConnectController * pAppmanCC = CAppManConnectController::instance();
   if (!pAppmanCC)
   {
      return CError(1, gModuleName, CError::FATAL, "unable to get app man connect controller");
   }

   AXIS::AppMan::IAppManHandler* pHandler = pPmalMgr->getAppManHandler();
   if (!pHandler)
   {
      return CError(1, gModuleName, CError::FATAL, "unable to get app man handler");
   }

   pAppmanCC->init(pHandler);

   return CError::NoError();
}

CGenericService* CServiceManager::loadService(const char* pUID, CGenericService::tProfileCallbacksMap & cbMap)
{
   CGenericService* pService = NULL;
   LOG4CPLUS_TRACE(msLogger, "CServiceManager::loadService()=> UID " + std::string(pUID) );
   std::map<std::string, CGenericService* >::iterator iter = mServicesProtoMap.find((std::string)pUID);
   if( iter!=mServicesProtoMap.end())
   {
      pService = iter->second->clone(cbMap);
   }
   else 
   {
      LOG4CPLUS_TRACE(msLogger, "CServiceManager::loadService()=> service not found" );
   }
   return pService;
}

 void CServiceManager::dropService(const char* serviceUID)
 {
   
 }
 
void CServiceManager::registerClient(CServiceManager::IClientCallbacks* client)
{
   mClientsMutex.lock();
   mClients.push_back(client);
   mClientsMutex.unlock();
}

void CServiceManager::unregisterClient(CServiceManager::IClientCallbacks* client)
{
   mClientsMutex.lock();
   std::vector<CServiceManager::IClientCallbacks* >::iterator it = std::find(mClients.begin(), mClients.end(),client);

   if(it!=mClients.end())
   {
      mClients.erase(it);
   }
   mClientsMutex.unlock();
}


CError CServiceManager::serviceRequestCallback(AXIS::Service::Uid const& serviceUID)
{
   mClientsMutex.lock();
   LOG4CPLUS_TRACE(msLogger, "CServiceManager::serviceRequestCallback()=> requested service " + serviceUID.value() );
   for(std::vector<CServiceManager::IClientCallbacks* >::iterator it =mClients.begin(); it!=mClients.end();++it )
   {
      (*it)->serviceRequestCallback(serviceUID);
   }
   mClientsMutex.unlock();

   return CError::NoError();
}

CError CServiceManager::incomingProfile(AXIS::Service::Uid const& serviceUID, AXIS::Profile::Uid const& profileUid, AXIS::Profile::ApiUid const& papiUid)
{
   mClientsMutex.lock();
   LOG4CPLUS_TRACE(msLogger, "CServiceManager::incomingProfileRequest()=> requested  profile "
      + profileUid.value() +"for service"+serviceUID.value() );
   for(std::vector<CServiceManager::IClientCallbacks* >::iterator it =mClients.begin(); it!=mClients.end();++it )
   {
      (*it)->incomingProfileRequest(serviceUID, profileUid,papiUid);
   }
   mClientsMutex.unlock();

   return CError::NoError();
}

void CServiceManager::serviceDropped(AXIS::Service::Uid const& serviceUID)
{
   mClientsMutex.lock();
   LOG4CPLUS_TRACE(msLogger, "CServiceManager::serviceDropped()=> service " + serviceUID.value() );
   for(std::vector<CServiceManager::IClientCallbacks* >::iterator it =mClients.begin(); it!=mClients.end();++it )
   {
      (*it)->serviceDropped(serviceUID);
   }
   mClientsMutex.unlock();
}

void CServiceManager::profileDied(AXIS::Service::Uid const& serviceUID, AXIS::Profile::IUid const& piuid )
{
   mClientsMutex.lock();
   LOG4CPLUS_TRACE(msLogger, "CServiceManager::profileDied()=> closed  profile"
                     + piuid.value() +"for service " + serviceUID.value() );
   for(std::vector<CServiceManager::IClientCallbacks* >::iterator it =mClients.begin(); it!=mClients.end();++it )
   {
      (*it)->profileDied(serviceUID, piuid);
   }
   mClientsMutex.unlock();
}
