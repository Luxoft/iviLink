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
 * @file             IPmpCoreProtocol.hp
 * @brief            Header file for IPmpCoreProtocol interface
 * @author           VPlachkov
 ***************************************************************************/

#ifndef IPMPCOREPROTOCOL_HPP_
#define IPMPCOREPROTOCOL_HPP_

#include <list>

#include "utils/misc/include/CError.hpp"
#include "utils/misc/include/UID.hpp"

namespace AXIS
{
   namespace PMP
   {
      /**
       * Interface for PMP Core Protocol functionality
       */
      class IPmpCoreProtocol
      {
      public:
         virtual CError getAvailableProfileComplements(std::list<Profile::Uid> & complements) = 0;
         virtual CError reloadProfilesFromRepository() = 0;
         virtual CError resetProfileState() = 0;
         virtual CError lock(AXIS::CUid id) = 0;
         virtual CError unlock(AXIS::CUid id) = 0;
         virtual CError unlockAll() = 0;
         virtual CError disableByClient(AXIS::CUid id) = 0;
         virtual CError enableByClient(AXIS::CUid id) = 0;
         virtual CError enableByClientAll() = 0;

         virtual ~IPmpCoreProtocol(){};
      };
   }
}

#endif /* IPMPCOREPROTOCOL_HPP_ */
