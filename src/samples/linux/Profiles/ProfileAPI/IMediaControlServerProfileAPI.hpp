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



#ifndef IMEDIACONTROLSERVER_PROFILE_API_HPP
#define IMEDIACONTROLSERVER_PROFILE_API_HPP

#include <iostream>
#include "framework/public/appLib/CProfileApiBase.hpp"


class IMediaControlServerProfile_API : public iviLink::CProfileApiBase
{
   IVILINK_PROFILE_API_UID(MediaControlServerProfile_API_UID)
   public:

      class Callbacks : public iviLink::Profile::IProfileCallbackProxy
      {
      public:
         virtual void onServerToggle() = 0; //just receive
         virtual void onServerSync() = 0; //just receive
         virtual void onServerUnsync() = 0; //just receive
	 virtual void onServerPlay(std::string  const& trackName, std::string const& source, std::string const& avform) = 0; //start new broadcast and send the msg to app
         virtual void onServerResume() = 0;
         virtual void onServerPause() = 0; //pause broadcast and send the msg to app
         virtual void onServerStop() = 0; //send the msg to app
      };

      virtual void play(std::string const trackName, std::string const avform) = 0; //start new broadcast and send the msg to another side
      virtual void resume() = 0;
      virtual void pause() = 0; //pause broadcast and send the msg to another side
      virtual void stopStreaming() = 0; //close streaming channel and send stop to another side
      virtual void toggle() = 0; //just send
      virtual void sync() = 0; //just send
      virtual void unsync() = 0; //just send
      virtual void setPath(std::string const& path) = 0; //set source path

};


#endif /* IMEDIACONTROLSERVER_PROFILE_API_HPP */
