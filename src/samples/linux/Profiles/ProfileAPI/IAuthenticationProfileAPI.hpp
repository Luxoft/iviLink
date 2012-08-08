/* 
 * 
 * iviLINK SDK, version 1.0.1
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 * 
 */








#ifndef IAUTHENTICATION_PROFILE_API_HPP
#define IAUTHENTICATION_PROFILE_API_HPP

#include "framework/public/appLib/CProfileApiBase.hpp"

class IAuthenticationProfile_API : public iviLink::CProfileApiBase
{
   IVILINK_PROFILE_API_UID(AuthenticationProfile_PAPI_UID)
   public:

      class Callbacks : public iviLink::Profile::IProfileCallbackProxy
      {
      public:
         virtual void gotPIN(int first_digit, int second_digit, int third_digit, int fourth_digit) = 0;
         virtual void onAuthenticationIsNotRequired() = 0;
         virtual void onAuthenticationIsRequired() = 0;
      };

      virtual void sendPIN(int first_digit, int second_digit, int third_digit, int fourth_digit) = 0;
      virtual void sendPublicKey() = 0;
      virtual void writeRemoteUIDToTrustList() = 0;
};


#endif /* ICLIMATE_PROFILE_API_HPP */
