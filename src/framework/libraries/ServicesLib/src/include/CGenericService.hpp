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
* @file             CGenericService.hpp
* @brief            The CGenericService class header file
* @author           KSkidanov
***************************************************************************/
#ifndef CGENERIC_SERVICE_HPP
#define CGENERIC_SERVICE_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <map>
#include <string>
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/misc/include/Types.hpp"
#include "utils/threads/include/CMutex.hpp"
#include "framework/components/ProfileManager/include/CProfile.hpp"
#include "utils/misc/include/UID.hpp"
#include "utils/misc/include/Logger.hpp"

using AXIS::Profile::CProfile;
using AXIS::Profile::IProfileCallbackProxy;
using AXIS::Profile::ApiUid;

class CGenericService
{
   public:

   //Types section

   /**
    * describes profile
    */
   struct tProfileInfo
   {
      AXIS::Profile::Uid UID;
      AXIS::Profile::CProfile*  pAPI;
   };

   typedef std::map< AXIS::Profile::ApiUid, AXIS::Profile::IProfileCallbackProxy* > tProfileCallbacksMap;

   //Methods  section

   /**
    * enables service
    */

   inline void enable(){mEnabled = true;};

   /**
    * disables service
    */
   inline void disable(){mEnabled = false;};

   /**
    * test if the UID matches
    * @return  true if the UID matches
    */
   bool    isOfClass(const char * const pUID) const;

   /**
    * return the unique class name
    * @return  the unique class Name
    */
   char const *    getUID() const;

   /**
    * change the mpUID
    */
   void setUID(const char * const pUID);
 
   /**
    * Called when profile on the other side is set up and paired profile needed
    * @param uid  UID of profile that should be loaded
    * @param papiUid api uid of profile that should be loaded
    * @return  ERR_OK or error code
    */
   virtual ERROR_CODE incomingProfileRequest(AXIS::Profile::Uid const& uid, AXIS::Profile::ApiUid const& papiUid);

   /**
    * Called when profile on the other side is unloaded and paired profile needed
    * @param uid  UID of profile that should be unloaded
    * @param recursive  on true sends request to unload paired profile, on false - delete locally
    * @return  ERR_OK or error code
    */
   virtual ERROR_CODE unloadProfile( AXIS::Profile::Uid uid);

   /**
    * Loads profiles containing in service, and enables
    * @return  ERR_OK or error code
    */
   virtual ERROR_CODE instantiate();

   /**
    * Creates new Service instance
    * @return  pointer to newly created service
    */
   virtual CGenericService* clone(tProfileCallbacksMap & cbMap) = 0;

   /**
    * Checks whether service contains profile API with given alias name and give it to client
    * @param alias profile API alias in current service(see service xml-file)
    * @return  pointer to profile API or NULL
    */
   virtual AXIS::Profile::CProfile* getProfile( const char* alias);

   /**
    * Destructor
    */
   virtual ~CGenericService() = 0;

   protected:

   // Methods section
   explicit CGenericService(const char* pUID);
   /**
    * Constructor is protected to forbid creation objects of this type. Only inherited allowed.
    * @param pUID service UID
    */
   CGenericService(const char* pUID, tProfileCallbacksMap & cbMap);

   /**
    * Parses service XML description and loads needed profile APIs
    * @param filename filename of service XML description
    * @return ERR_OK or ERR_XML_PARSING
    */
   ERROR_CODE parseServiceXML(const char* filename);

   /**
    * Copying constructor
    * @param classRef The reference to the object the class is constructing on
    */
   CGenericService(const CGenericService& rhs);

   private:

   // Methods section

   /**
    * Assigning operator
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is assiging from
    */
   CGenericService& operator=(const CGenericService& );

   // Members section
   AXIS::Service::Uid                                    mUID;               ///> Service UID
   AXIS::Service::Uid                                    mPair;             ///> Service pair UID
   std::map<std::string, tProfileInfo>                   mProfilesMap;
   bool                                                  mEnabled;
   tProfileCallbacksMap                                  mProfilesCbMap;
   static Logger                                         msLogger;
 };

#endif
