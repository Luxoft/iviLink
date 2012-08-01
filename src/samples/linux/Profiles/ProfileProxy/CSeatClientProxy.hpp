/* 
 * 
 * iviLINK SDK, version 1.0
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



#ifndef CSEATCLIENTPROXY_HPP_
#define CSEATCLIENTPROXY_HPP_

#include "samples/linux/Profiles/ProfileAPI/ISeatClientProfile.hpp"
#include "framework/appLib/appLib/CProfileProxy.hpp"

#define CURRENT_PROFILE_API ISeatClientProfile
#define CURRENT_PROFILE_PROXY CSeatClientProxy
#define CURRENT_PROFILE_PROXY_API_UID "SeatClientProfile_API_UID"

IVILINK_PROFILE_PROXY_BEGIN(CURRENT_PROFILE_PROXY, CURRENT_PROFILE_API, CURRENT_PROFILE_PROXY_API_UID)

virtual void moveRequest(EMoveEvent event)
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,moveRequest, event)
}

virtual void tabRequest(EPersonEvent event)
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,tabRequest, event)
}

virtual void heaterRequest(EPersonEvent event)
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,heaterRequest, event)
}

virtual void initRequest()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,initRequest)
}

IVILINK_PROFILE_PROXY_END

#endif /* CSEATCLIENTPROXY_HPP_ */
