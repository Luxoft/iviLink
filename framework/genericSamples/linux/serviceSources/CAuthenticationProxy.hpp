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


#ifndef CAUTHENTICATIONPROXY_HPP_
#define CAUTHENTICATIONPROXY_HPP_

#include "IAuthenticationProfileAPI.hpp"
#include "ProfileProxy.hpp"

#define CURRENT_PROFILE_API IAuthenticationProfile_API
#define CURRENT_PROFILE_PROXY CAuthenticationProxy
#define CURRENT_PROFILE_PROXY_API_UID "AuthenticationProfile_PAPI_UID"

IVILINK_PROFILE_PROXY_BEGIN(CURRENT_PROFILE_PROXY, CURRENT_PROFILE_API, CURRENT_PROFILE_PROXY_API_UID)


virtual void sendPIN(const int first_digit, const int second_digit, const int third_digit, const int fourth_digit)
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,sendPIN, first_digit, second_digit, third_digit, fourth_digit);
}

virtual void sendPublicKey()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,sendPublicKey)
}

virtual void writeRemoteUIDToTrustList()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,writeRemoteUIDToTrustList)
}

virtual void readyToExit()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API, readyToExit)
}

IVILINK_PROFILE_PROXY_END

#endif /* CAUTHENTICATIONPROXY_HPP_ */
