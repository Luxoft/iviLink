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


#ifndef ICLIMATE_PROFILE_API_HPP
#define ICLIMATE_PROFILE_API_HPP

#include "framework/components/ProfileManager/include/CProfile.hpp"

class IClimateProfile_API : public AXIS::Profile::CProfile
{
   public:

      class Callbacks : public AXIS::Profile::IProfileCallbackProxy
      {
         public:
            virtual void messageReceived(std::string profile_id, std::string message) = 0;
      };

      virtual bool processEvent(Event event, bool need_send = false) = 0;

   protected:

      IClimateProfile_API(AXIS::Profile::IUid const& id, AXIS::Service::Uid const& sid) : CProfile(id, sid) {}
      virtual ~IClimateProfile_API() { }

}; 


#endif /* ICLIMATE_PROFILE_API_HPP */
