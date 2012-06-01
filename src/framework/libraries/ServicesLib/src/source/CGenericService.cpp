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
 * @file             CGenericService.cpp
 * @brief            The CGenericService class implementation file
 * @author           KSkidanov
 ***************************************************************************/
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <assert.h>
#include <cstring>
#include "stdio.h"
/********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/


/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "framework/libraries/ServicesLib/src/include/CGenericService.hpp"
/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "utils/misc/include/logging.hpp"
#include "utils/xml/include/pugixml.hpp"
#include "framework/libraries/ServicesLib/src/include/CServiceManager.hpp"
#include "framework/components/ProfileManager/PMAL/include/CComponentMgr.hpp"
#include "framework/components/ProfileManager/PMAL/include/IProfileManagersCallbacks.hpp"
#include "framework/components/ProfileManager/PMAL/include/IPMALCore.hpp"
#include "framework/components/ProfileManager/PMAL/include/IPMALPIM.hpp"
#include "framework/components/ProfileManager/PMP/componentManager/include/CPmpComponentManager.hpp"

using AXIS::PMAL::IPMALCore;
using AXIS::PMAL::CPMALError;
using AXIS::PMAL::IPMALPIM;
using AXIS::PMP::CPmpComponentManager;
using AXIS::PMAL::CPMALComponentMgr;
using AXIS::Profile::CProfile;

Logger CGenericService::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("ServicesLib.GenericService"));

CGenericService::CGenericService(const char* pUID, tProfileCallbacksMap & cbMap):
   mUID(pUID),mEnabled(false),mProfilesCbMap(cbMap)
{
   std::string str = mUID.value();
   LOG4CPLUS_TRACE(msLogger, "CGenericService::CGenericService()=> UID " + std::string(pUID) );
}

CGenericService::CGenericService(const char* pUID):
   mUID(pUID),mEnabled(false)
{
   std::string str = mUID.value();
   LOG4CPLUS_TRACE(msLogger, "CGenericService::CGenericService()=> UID " + std::string(pUID) );
}

CGenericService::CGenericService(const CGenericService& rhs)
{
   mUID = rhs.mUID;
   mEnabled = rhs.mEnabled;
   mProfilesCbMap = rhs.mProfilesCbMap;
}
CGenericService::~CGenericService()
{
   for(std::map<std::string, tProfileInfo >::iterator iter =mProfilesMap.begin(); iter!=mProfilesMap.end(); ++iter)
   {
      
      IPMALCore* pCore = CPMALComponentMgr::getInstance()->getCore();
      IPMALPIM* pPim = CPMALComponentMgr::getInstance()->getPIM();
      CPMALError err = pPim->unloadProfile(iter->second.pAPI);
      iter->second.pAPI = NULL;
   }
}

ERROR_CODE CGenericService::instantiate()
{
 
   std::string path = CServiceManager::mXMLPath + mUID.value() + ".xml";
   LOG4CPLUS_TRACE(msLogger, "CGenericService::instantiate() xml file =" +path);
   return parseServiceXML(path.c_str());
}

bool    CGenericService::isOfClass(const char * const pUID) const
{
   
   std::string uid = mUID.value();
   if(uid.compare( pUID) == 0) return true;
   else return false;
}

char const *    CGenericService::getUID() const
{
   std::string uid = mUID.value();
   return uid.c_str();
}

void CGenericService::setUID(const char * const pUID)
{
   mUID = pUID;
}

ERROR_CODE CGenericService::incomingProfileRequest( AXIS::Profile::Uid const& uid, AXIS::Profile::ApiUid const& papiUid)
{
   LOG4CPLUS_TRACE(msLogger, "CGenericService::incomingProfileRequest() " + uid.value() 
      + " api = " + papiUid.value());
   ERROR_CODE ret = ERR_FAIL;
   std::string path = CServiceManager::mXMLPath + mUID.value() + ".xml";
   pugi::xml_document doc;
   pugi::xml_parse_result result = doc.load_file(path.c_str());
   if (result)
   {
      
      std::map<std::string,std::string> params;
      if(mPair.empty()) mPair = doc.child("service").child("pair").attribute("name").value();
      std::string apiuid_str = papiUid.value();
      pugi::xml_node profile_node = doc.child("service").find_child_by_attribute("profile", "api",apiuid_str.c_str());
      std::string profile_api_alias(profile_node.attribute("alias").value());

      CError err = CError::NoError();
      IPMALPIM* pPim = CPMALComponentMgr::getInstance()->getPIM();
      assert(pPim);
      AXIS::Profile::Uid const& prfUid = uid;
      
      AXIS::Profile::CProfile * pProfile = NULL;
      tProfileCallbacksMap::iterator iter = mProfilesCbMap.find(papiUid);
      if(iter !=mProfilesCbMap.end())
      {
         LOG4CPLUS_TRACE(msLogger, "CGenericService::incomingProfileRequest() callbacks found, loding profile....");
         AXIS::Profile::IProfileCallbackProxy* pCallback = iter->second;
         err = pPim->loadProfile(prfUid, mUID, pCallback, pProfile);
         tProfileInfo info = {uid, pProfile};
         mProfilesMap.insert(std::pair<std::string ,tProfileInfo >(profile_api_alias, info));
         ret = ERR_OK;
      }
      else
      {
         LOG4CPLUS_ERROR(msLogger, "CGenericService::incomingProfileRequest() callbacks not found!");
         ret = ERR_NOTFOUND;
      }
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger, "CGenericService::incomingProfileRequest() XML parsing error!");
      ret = ERR_XML_PARSING;
   }
   return ret;
}

ERROR_CODE CGenericService::unloadProfile( AXIS::Profile::Uid uid)
{
   ERROR_CODE ret = ERR_OK;
   LOG4CPLUS_ERROR(msLogger, "CGenericService::unloadProfile()"+ uid.value());
   for(std::map<std::string, tProfileInfo >::iterator iter =mProfilesMap.begin(); iter!=mProfilesMap.end(); ++iter)
   {
      if(iter->second.UID == uid)
      {
         IPMALCore* pCore = CPMALComponentMgr::getInstance()->getCore();
         IPMALPIM* pPim = CPMALComponentMgr::getInstance()->getPIM();
         CPMALError err = pPim->unloadProfile(iter->second.pAPI);
         iter->second.pAPI = NULL;
         mProfilesMap.erase(iter);
         break;           
      }
   }

   return ret;
}

AXIS::Profile::CProfile* CGenericService::getProfile( const char* alias)
{
   AXIS::Profile::CProfile* pProfile = NULL;

   std::map<std::string, tProfileInfo>::iterator iter = mProfilesMap.find((std::string)alias);
   if(iter!= mProfilesMap.end())
   {
      pProfile = iter->second.pAPI;
   }
   return pProfile;
}

ERROR_CODE CGenericService::parseServiceXML(const char* filename)
{
   LOG4CPLUS_TRACE(msLogger, "CGenericService::parseServiceXML filename = '" + 
      std::string(filename ? filename : "(null)") + "'");
   ERROR_CODE ret = ERR_FAIL;

   pugi::xml_document doc;
   pugi::xml_parse_result result = doc.load_file(filename);
   if (result)
   {
      std::string service_name(doc.child("service").attribute("name").value());
      LOG4CPLUS_TRACE(msLogger, "CGenericService::parseServiceXML()" + service_name);
      mPair = doc.child("service").child("pair").attribute("name").value();

      for (pugi::xml_node profile_node = doc.child("service").child("profile"); profile_node; profile_node = profile_node.next_sibling("profile"))
      {
         AXIS::Profile::ApiUid profile_api_UID(profile_node.attribute("api").value());

         std::map<std::string,std::string> params;
         for (pugi::xml_node param_node = profile_node.child("param"); param_node; param_node = param_node.next_sibling("param"))
         {
            std::string param_name(param_node.attribute("name").value());
            std::string param_val(param_node.attribute("lowbound").value());
            param_val+=";" + (std::string)param_node.attribute("upbound").value() + ";" + (std::string)param_node.attribute("optimal").value();
            params.insert(std::pair<std::string ,std::string >(param_name, param_val));
         }

         IPMALCore* pCore = CPMALComponentMgr::getInstance()->getCore();
         std::list<AXIS::Profile::Uid> profiles;
         CPMALError err = pCore->findProfiles(profile_api_UID, params, profiles, true);
         if (!err.isNoError())
         {
            LOG4CPLUS_ERROR(msLogger, "CGenericService::parseServiceXML() error getting profile for api='" 
               + profile_api_UID.value() 
               + "' error: " + static_cast<std::string>(err));
            ret = ERR_NOTFOUND;
            break;
         }
         if (profiles.empty())
         {
            LOG4CPLUS_ERROR(msLogger, "CGenericService::parseServiceXML() no profiles for api='" 
               + profile_api_UID.value() + "'");
            ret = ERR_NOTFOUND;
            break;
         }
         
         std::string profile_api_alias(profile_node.attribute("alias").value());
         IPMALPIM* pPim = CPMALComponentMgr::getInstance()->getPIM();
         AXIS::Profile::Uid prfUid = profiles.front();
         profiles.clear();
      
         AXIS::Profile::CProfile * pProfile = NULL;
         tProfileCallbacksMap::iterator iter= mProfilesCbMap.find(profile_api_UID);
         if(iter != mProfilesCbMap.end())
         {
            LOG4CPLUS_TRACE(msLogger, "CGenericService::parseServiceXML() callbacks found, loding profile....");
            AXIS::Profile::IProfileCallbackProxy* pCallback = iter->second;
            err = pPim->loadProfile(prfUid, mPair, pCallback, pProfile);
            if (err.isNoError())
            {
               tProfileInfo info = {profile_api_UID, pProfile};
               mProfilesMap.insert(std::pair<std::string ,tProfileInfo >(profile_api_alias, info));
               ret = ERR_OK;  
            }
            else
            {
               LOG4CPLUS_ERROR(msLogger, "CGenericService::parseServiceXML() unable to load profile, error: "
                  + static_cast<std::string>(err));
               ret = ERR_NOTFOUND;
               break;
            }
         }
         else
         {
            LOG4CPLUS_ERROR(msLogger, "CGenericService::parseServiceXML() callbacks not found for profile api '"
               + profile_api_UID.value() +"' !");
            ret = ERR_NOTFOUND;
            break;
         }
      }  
   }
   else
   {
      LOG4CPLUS_ERROR(msLogger, "CGenericService::parseServiceXML() XML parsing error! " +
         std::string(result.description()));
      ret = ERR_XML_PARSING;
   }
   return ret;
}
