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


#ifndef CMEDIACONTROLSERVERPROFILEPROXY_HPP
#define CMEDIACONTROLSERVERPROFILEPROXY_HPP

#include <iostream>
 
#include "IMediaControlServerProfileAPI.hpp"
#include "ProfileProxy.hpp"

#define CURRENT_PROFILE_API IMediaControlServerProfile_API
#define CURRENT_PROFILE_PROXY CMediaControlServerProfileProxy
#define CURRENT_PROFILE_PROXY_API_UID "MediaControlServerProfile_API_UID"

IVILINK_PROFILE_PROXY_BEGIN(CURRENT_PROFILE_PROXY, CURRENT_PROFILE_API, CURRENT_PROFILE_PROXY_API_UID)

virtual void serverStop()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,serverStop);
}

virtual void changeHighlightedTrack(int newPosition) 
{
    IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,changeHighlightedTrack,newPosition)
}

virtual void pause()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,pause);
}

virtual void play(std::string const& trackName, std::string const&  trackUid, std::string const& avform, bool needLock)
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,play,trackName,trackUid, avform, needLock)
}

virtual void stopStreaming()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,stopStreaming)
}

virtual void toggle()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,toggle)
}

virtual void resume(bool needLock)
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,resume, needLock)
}

virtual void sync()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,sync)
}

virtual void unsync()
{
   IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,unsync)
}

virtual void setPath(std::string const& path) 
{
  IVILINK_PROXY_VOID_FUNCTION(CURRENT_PROFILE_API,setPath, path)
}

IVILINK_PROFILE_PROXY_END

#endif /* CMEDIACONTROLSERVERPROFILEPROXY_HPP */
