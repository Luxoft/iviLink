/* 
 * 
 * iviLINK SDK, version 1.1.26
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2013, Luxoft Professional Corp., member of IBS group
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


#ifndef APPLIB_TEST_PROFILE_PROXY_HPP
#define APPLIB_TEST_PROFILE_PROXY_HPP

#include <string>

//profile proxy header, must be included
#include "ProfileProxy.hpp"

//abstract profile API class header, must be included
#include "IAppLibTestProfileAPI.hpp"

#define CURRENT_PROFILE_API IAppLibTestProfile_API
#define CURRENT_PROFILE_PROXY AppLibTestProfileProxy
#define CURRENT_PROFILE_PROXY_API_UID "AppLibTest_PAPI_UID"

IVILINK_PROFILE_PROXY_BEGIN(CURRENT_PROFILE_PROXY, CURRENT_PROFILE_API, CURRENT_PROFILE_PROXY_API_UID)

virtual void sendData(std::string data)
{
   // macro calls CURRENT_PROFILE_API->sendData(data), if the function exists
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,sendData, data)
}

IVILINK_PROFILE_PROXY_END

#endif // APPLIB_TEST_PROFILE_PROXY_HPP
