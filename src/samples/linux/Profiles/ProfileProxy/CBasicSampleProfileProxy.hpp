/* 
 * 
 * iviLINK SDK, version 1.1.2
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







#ifndef CBASICSAMPLEPROFILEPROXY_HPP_
#define CBASICSAMPLEPROFILEPROXY_HPP_

//abstract profile API class header, must be included
#include "samples/linux/Profiles/ProfileAPI/IBasicSampleProfileAPI.hpp"
//profile proxy header, must be included
#include "framework/public/appLib/CProfileProxy.hpp"

#define CURRENT_PROFILE_API IBasicSampleProfile_API
#define CURRENT_PROFILE_PROXY CBasicSampleProfileProxy
#define CURRENT_PROFILE_PROXY_API_UID "BasicSampleProfile_PAPI_UID"

IVILINK_PROFILE_PROXY_BEGIN(CURRENT_PROFILE_PROXY, CURRENT_PROFILE_API, CURRENT_PROFILE_PROXY_API_UID)

/**
 * Sending the summands
 * @param a, b - operands
 * @return none
 */
virtual void sendOperands(int a, int b)
{
   // macro calls CURRENT_PROFILE_API->sendOperands(a,b), if the function exists
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,sendOperands, a, b)
}

/**
 * Sending the obtained result
 * @param a - result of operation
 * @return none
 */
virtual void sendResult(int a)
{
   // macro calls CURRENT_PROFILE_API->sendResult(a), if the function exists
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,sendResult,a)
}

IVILINK_PROFILE_PROXY_END

#endif /* CBASICSAMPLEPROFILEPROXY_HPP_ */
