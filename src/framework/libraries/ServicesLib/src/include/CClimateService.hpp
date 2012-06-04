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




#ifndef CCLIMATE_SERVICE_HPP
#define CCLIMATE_SERVICE_HPP
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
#include "framework/libraries/ServicesLib/src/include/CGenericService.hpp"

using AXIS::Profile::CProfile;

class CClimateService:public CGenericService
{

   public:

   // Methods section
   CClimateService(const char* pUID);
   CClimateService(const char* pUID,tProfileCallbacksMap & cbMap);

   /**
    * Destructor
    */
   virtual ~CClimateService() ;

   /**
    * Creates new Service instance
    * @return  pointer to newly created service
    */
   virtual CGenericService* clone(tProfileCallbacksMap & cbMap);

   private:

   // Methods section

   /**
    * Copying constructor
    * @param classRef The reference to the object the class is constructing on
    */
   CClimateService(const CClimateService& );

   /**
    * Assigning operator
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is assiging from
    */
   CClimateService& operator=(const CClimateService& );

private:

   // Members section


 };

#endif
