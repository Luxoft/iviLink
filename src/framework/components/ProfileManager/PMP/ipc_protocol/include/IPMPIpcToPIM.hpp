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

/*
 * IPMPIpcToPIM.hpp
 *
 *  Created on: Mar 12, 2012
 *      Author: pilin
 */

#ifndef IPMPIPCTOPIM_HPP_
#define IPMPIPCTOPIM_HPP_

#include "utils/misc/include/CError.hpp"
#include "utils/misc/include/CUid.hpp"
#include "utils/ipc/include/common.hpp"

#include "framework/components/ProfileManager/include/CProfile.hpp"

namespace AXIS {
namespace PMP {

class IPMPIpcToPIM
{
public:

   /**
    * @todo description
    *
    * @param profileUid
    * @param piuid
    * @param serviceUID
    * @param dirId id of direction of connection
    *
    * @retval NO_ERROR
    * @retval ERROR_OTHER ipc errors 
    * 
    * @todo proper error codes
    */
   virtual CError incomingProfile(AXIS::CUid const& profileUid,
      AXIS::Profile::ApiUid const& papiUid,
      AXIS::Profile::IUid const& piuid,
      AXIS::Service::Uid const& serviceUID,
      AXIS::Ipc::DirectionID const& dirId) = 0;

   /**
    * @todo description
    *
    * @param piuid
    * @param dirId id of direction of connection
    *
    * @retval NO_ERROR
    * @retval ERROR_OTHER ipc errors 
    * 
    * @todo proper error codes
    */
   virtual CError profileDied(AXIS::Profile::IUid const& piuid,
      AXIS::Ipc::DirectionID const& dirId) = 0;

protected:

   virtual ~IPMPIpcToPIM()
   {
   }
};

}  // namespace ProfileManager
}  // namespace AXIS

#endif /* IPMPIPCTOPIM_HPP_ */
