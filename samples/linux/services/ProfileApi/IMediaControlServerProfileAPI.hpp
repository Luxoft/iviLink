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


#ifndef IMEDIACONTROLSERVER_PROFILE_API_HPP
#define IMEDIACONTROLSERVER_PROFILE_API_HPP

#include <iostream>
#include <string>

#include "BaseProfileApi.hpp"


class IMediaControlServerProfile_API : public iviLink::BaseProfileApi
{
    IVILINK_PROFILE_API_UID (MediaControlServerProfile_API_UID)
public:

    class Callbacks: public iviLink::Profile::IProfileCallbackProxy
    {
    public:
        virtual void onServerToggle(bool needLock) = 0; //just receive
        virtual void onServerSync(bool needLock) = 0; //just receive
        virtual void onServerUnsync(bool needLock) = 0; //just receive
        virtual void onServerPlay(std::string const& trackName, std::string const& trackUid,
                std::string const& source, std::string const& avform, bool needLock) = 0; //start new broadcast and send the msg to app
        virtual void onServerResume(bool needLock) = 0;
        virtual void onServerPause(bool needLock) = 0; //pause broadcast and send the msg to app
        virtual void onServerStop(bool needLock) = 0; //send the msg to app
        virtual std::string onMediaPathRequest(bool needLock) = 0;
        virtual void startVlcServer(int pipeReadDescriptor, bool needLock) = 0;
    };

    virtual void serverStop() = 0; //send stop to the vlc server without any callback to other side
    virtual void play(std::string const& trackName, std::string const& trackUid,
            std::string const& avform, bool needLock) = 0; //start new broadcast and send the msg to another side
    virtual void resume(bool needLock) = 0;
    virtual void pause() = 0; //pause broadcast and send the msg to another side
    virtual void stopStreaming() = 0; //close streaming channel and send stop to another side
    virtual void toggle() = 0; //just send
    virtual void sync() = 0; //just send
    virtual void unsync() = 0; //just send
    virtual void setPath(std::string const& path) = 0; //set source path
    virtual void changeHighlightedTrack(int newPosition) = 0;

};

#endif /* IMEDIACONTROLSERVER_PROFILE_API_HPP */
