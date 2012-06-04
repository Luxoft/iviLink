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





#ifndef IPMPCORECLB_HPP_
#define IPMPCORECLB_HPP_

#include <list>

#include "utils/misc/include/UID.hpp"
#include "utils/misc/include/CError.hpp"

namespace AXIS
{
   namespace PMP
   {

      /**
       * Interface in used to declare PMP Core protocol callbacks
       */
      class IPmpCoreClb
      {
      public:
         virtual void onGetAvailableProfileComplements(std::list<Profile::Uid> & complements) = 0;
         virtual void onReloadProfilesFromRepository () = 0;
         virtual void onResetProfilesState() = 0;
         virtual bool onLock(AXIS::CUid id) = 0;
         virtual bool onUnlock(AXIS::CUid id) = 0;
         virtual void onUnlockAll() = 0;
         virtual bool onDisableByClient(AXIS::CUid id) = 0;
         virtual bool onEnableByClient(AXIS::CUid id) = 0;
         virtual void onEnableByClientAll() = 0;
         virtual void onLostConnection() = 0;
      };
   }
}

#endif /* IPMPCORECLB_HPP_ */
