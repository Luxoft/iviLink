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
 * IIpcToPIM.hpp
 *
 *  Created on: Feb 16, 2012
 *      Author: pilin
 */

#ifndef IIPCTOPIM_HPP_
#define IIPCTOPIM_HPP_

#include "utils/misc/include/CError.hpp"

#include "framework/components/ProfileManager/PMAL/PIM/include/CProfileInternal.hpp"

namespace AXIS {
namespace PMAL {

class IPMALIpcToPIM
{
public:
   /**
    * Requests the PIUID
    * @param[out] piuid
    * @retval
    */
   virtual CError generatePIUID(AXIS::Profile::IUid& piuid) = 0;

   /**
    * Marks PIUID as free
    * @param piuid
    * @return
    */
   virtual CError releasePIUID(AXIS::Profile::IUid const& piuid) = 0;

   /**
    * Message to other side meaning that profile has been instantiated and it needs
    * its second half.
    * @param profileUid is profile implementation uid used to load profile
    * @param piuid is profile instance uid
    * @retvar
    */
   virtual CError createProfile(AXIS::CUid const& profileUid,
         AXIS::Profile::IUid const& piuid,
         AXIS::Service::Uid const& sid) = 0;

   /**
    * Sends message about died profile
    * @param piuid
    * @return
    */
   virtual CError profileDied(AXIS::Profile::IUid const& piuid) = 0;

   /**
    * Sends message about application ready to use service
    * @param sesId
    * @return
    */
   virtual CError readyToServe(AXIS::Service::SessionUid const& sesId) = 0;

protected:
   virtual ~IPMALIpcToPIM() { }
};

}  // namespace ProfileManager
}  // namespace AXIS


#endif /* IIPCTOPIM_HPP_ */
