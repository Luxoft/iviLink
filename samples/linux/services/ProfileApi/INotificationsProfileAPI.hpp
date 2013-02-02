/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 


#ifndef IBASIC_SAMPLE_PROFILE_API_HPP
#define IBASIC_SAMPLE_PROFILE_API_HPP

#include <string>
//Profile Api Base header, must be included
#include "BaseProfileApi.hpp"

// IBasicSampleProfile_API class (abstract class describing profile API) inherits CProfileApiBase class
class INotificationsProfileAPI : public iviLink::BaseProfileApi
{
   // Macro returns the argument 
   IVILINK_PROFILE_API_UID(NotifcationsProfile_PAPI_UID)
   public:

      // Callbacks class (abstract class describing callbacks) inherits IProfileCallbackProxy class
      class Callbacks : public iviLink::Profile::IProfileCallbackProxy
      {
         public:
            virtual void gotString(const std::string & incomingString) = 0;
      };

      virtual void sendString(const std::string & outgoingString) = 0;
}; 


#endif /* IBASIC_SAMPLE_PROFILE_API_HPP */
