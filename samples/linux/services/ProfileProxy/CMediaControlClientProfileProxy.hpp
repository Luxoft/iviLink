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


#ifndef CMEDIACONTROLCLIENTPROFILEPROXY_HPP
#define CMEDIACONTROLCLIENTPROFILEPROXY_HPP

#include <iostream>

#include "IMediaControlClientProfileAPI.hpp"
#include "ProfileProxy.hpp"

#define CURRENT_PROFILE_API IMediaControlClientProfile_API
#define CURRENT_PROFILE_PROXY CMediaControlClientProfileProxy
#define CURRENT_PROFILE_PROXY_API_UID "MediaControlClientProfile_API_UID"

IVILINK_PROFILE_PROXY_BEGIN(CURRENT_PROFILE_PROXY, CURRENT_PROFILE_API, CURRENT_PROFILE_PROXY_API_UID)

virtual void serverStop()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,serverStop);
}

virtual void stop()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,stop);
}

virtual void play(std::string const&  trackName, std::string const&  trackUid, std::string const& avform)
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,play, trackName, trackUid, avform)
}

virtual void resume()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,resume)
}

virtual void pause()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,pause)
}

virtual void toggle()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,toggle)
}

virtual void sync()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,sync)
}

virtual void unsync()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,unsync)
}

IVILINK_PROFILE_PROXY_END

#endif /* CMEDIACONTROLCLIENTPROFILEPROXY_HPP */
