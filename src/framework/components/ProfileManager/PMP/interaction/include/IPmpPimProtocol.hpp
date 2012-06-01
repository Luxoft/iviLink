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
 * @file             IPmpPimProtocol.hpp
 * @brief            Header file for IPmpPimProtocol interface
 * @author           PIlin
 ***************************************************************************/


#ifndef IPMPPIMPROTOCOL_HPP_
#define IPMPPIMPROTOCOL_HPP_

#include "framework/components/ProfileManager/include/CProfile.hpp"

namespace AXIS {
namespace PMP {

/**
 *
 */
class IPmpPimProtocol
{
public:

   /**
    *
    * @param profileUid
    * @param piuid
    * @param sid
    * @return
    */
   virtual CError createProfile(AXIS::Profile::Uid const& profileUid,
      AXIS::Profile::IUid const& piuid,
      AXIS::Service::Uid const& sid) = 0;

   /**
    *
    * @param piuid
    * @param sid
    * @return
    */
   virtual CError profileDied(AXIS::Profile::IUid const& piuid,
      AXIS::Service::Uid const& sid) = 0;


   virtual ~IPmpPimProtocol() { }
};

}  // namespace ProfileManager
}  // namespace AXIS



#endif /* IPMPPIMPROTOCOL_HPP_ */
