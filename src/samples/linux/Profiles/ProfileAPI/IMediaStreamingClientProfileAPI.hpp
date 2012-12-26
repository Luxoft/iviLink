/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#ifndef IMEDIASTREAMINGCLIENT_PROFILE_API_HPP
#define IMEDIASTREAMINGCLIENT_PROFILE_API_HPP


#include <string>

#include "CProfileApiBase.hpp"
#include <string>

class IMediaStreamingClientProfile_API : public iviLink::CProfileApiBase
{
   IVILINK_PROFILE_API_UID(MediaStreamingClientProfile_API_UID)
   public:

   class Callbacks : public iviLink::Profile::IProfileCallbackProxy
      {
      public:
         virtual void onStreamingClientAccepted() = 0;
         virtual void onStreamingClientUnaccepted() = 0;
      };

    virtual void streamingAccepted() = 0;
    virtual void streamingUnaccepted() = 0;
};


#endif /* IMEDIASTREAMINGCLIENT_PROFILE_API_HPP */
