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





#ifndef IPMPPIMTOIPC_HPP_
#define IPMPPIMTOIPC_HPP_

#include "utils/ipc/include/common.hpp"
#include "framework/components/ProfileManager/include/CProfile.hpp"

namespace AXIS {
namespace PMP {

class IPMPPIMToIpc
{
public:

   virtual void generatePIUID(AXIS::Profile::IUid& piuid,
      AXIS::Ipc::DirectionID const& dirId) = 0;

   virtual void releasePIUID(AXIS::Profile::IUid const& piuid,
      AXIS::Ipc::DirectionID const& dirId) = 0;

   virtual void createProfile(AXIS::CUid const& profileUid,
      AXIS::Profile::IUid const& piuid,
      AXIS::Service::Uid const& sid,
      AXIS::Ipc::DirectionID const& dirId) = 0;

   virtual void profileDied(AXIS::Profile::IUid const& piuid,
      AXIS::Ipc::DirectionID const& dirId) = 0;

   virtual void readyToServe(AXIS::Service::SessionUid const& sesUid,
      AXIS::Ipc::DirectionID const& dirId) = 0;

   virtual void onConnection(AXIS::Ipc::DirectionID const& dirId) = 0;

   virtual void onConnectionLost(AXIS::Ipc::DirectionID const& dirId) = 0;

protected:
   virtual ~IPMPPIMToIpc() { }
};



}  // namespace ProfileManager
}  // namespace AXIS

#endif /* IPMPPIMTOIPC_HPP_ */
