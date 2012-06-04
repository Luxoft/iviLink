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
#include "framework/libraries/ServicesLib/src/include/CVehicleSystemService.hpp"
/********************************************************************
 *
 * The other includes
 *
 ********************************************************************/
#include "utils/misc/include/logging.hpp"

CVehicleSystemService::CVehicleSystemService(const char* pUID):CGenericService(pUID)
{

}
CVehicleSystemService::CVehicleSystemService(const char* pUID,tProfileCallbacksMap & cbMap):CGenericService(pUID,cbMap)
{

}
CVehicleSystemService::CVehicleSystemService(const CVehicleSystemService& rhs):CGenericService(rhs)
{
}

CVehicleSystemService::~CVehicleSystemService()
{
}

CGenericService* CVehicleSystemService::clone(tProfileCallbacksMap & cbMap)
{
   return new CVehicleSystemService(getUID(),cbMap);
}
