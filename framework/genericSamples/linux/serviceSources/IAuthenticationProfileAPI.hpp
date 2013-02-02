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


#ifndef IAUTHENTICATION_PROFILE_API_HPP
#define IAUTHENTICATION_PROFILE_API_HPP

#include "BaseProfileApi.hpp"

class IAuthenticationProfile_API : public iviLink::BaseProfileApi
{
   IVILINK_PROFILE_API_UID(AuthenticationProfile_PAPI_UID)
   public:
      class Callbacks : public iviLink::Profile::IProfileCallbackProxy
      {
      public:
         virtual void onProfileError() = 0;
         virtual void gotPIN(const int first_digit, const int second_digit, const int third_digit, const int fourth_digit) = 0;
         virtual void onAuthenticationIsNotRequired() = 0;
         virtual void onAuthenticationIsRequired() = 0;
         virtual void onReadyToExit() = 0;
         virtual std::string getInternalPath() = 0;
      };

      virtual void sendPIN(const int first_digit, const int second_digit, const int third_digit, const int fourth_digit) = 0;
      virtual void sendPublicKey() = 0;
      virtual void writeRemoteUIDToTrustList() = 0;
      virtual void readyToExit() = 0;
};


#endif /* ICLIMATE_PROFILE_API_HPP */
