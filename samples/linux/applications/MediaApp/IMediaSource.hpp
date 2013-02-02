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
#ifndef I_MEDIA_SOURCE_HPP
#define I_MEDIA_SOURCE_HPP

#include <string>
#include <map>
#include <ctime>
#include <sstream>

#include "MediaAppCommon.hpp"

class IMediaSource 
{
public:
    virtual ~IMediaSource() {}
    virtual void recallRemoteTrackList() = 0;
    virtual void obtainTrackList() = 0;
    virtual void setRemoteTrackList(const std::string& tracks) = 0;
    virtual void getLocalTrackList(std::string& tracks) const = 0;
    virtual bool getFirstTrack(std::string& track, std::string& form,  std::string &trackUid) const = 0;
    virtual bool getLastTrack(std::string& track, std::string& form,  std::string &trackUid) const = 0;
    virtual bool getNextTrack(std::string& track, std::string& form, std::string &trackUid, const int currPos) const = 0;
    virtual bool getPrevTrack(std::string& track, std::string& form, std::string &trackUid, const int currPos) const = 0;
    virtual bool getInfoAboutTrack(int pos, std::string &track, std::string &form, std::string &trackUid) const = 0;
    virtual commonTrackListMap getTrackList() const = 0;
    virtual int getPosition(const std::string& trackUid) const = 0;
    virtual std::string getPathToFiles() const = 0;
    virtual void startVlcServer(const int pipeReadDescriptor) = 0;
    virtual bool isAudio(const std::string& trackName) const = 0;
};

#endif // I_MEDIA_SOURCE_HPP
