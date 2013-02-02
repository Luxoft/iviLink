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


#ifndef CMEDIASOURCE_H
#define CMEDIASOURCE_H

#include <list>
#include <map>
#include <string>
#include <vector>

#include "MediaAppCommon.hpp"
#include "IMediaSource.hpp"
#include "Logger.hpp"
#include "CRWMutex.hpp"

class CRWMutex;

class Playlist: public IMediaSource
{
public:
    Playlist(const std::string& pathToFiles);
    virtual void recallRemoteTrackList();   //on connection lost
    virtual void obtainTrackList();
    virtual void setRemoteTrackList(const std::string& tracks);
    virtual void getLocalTrackList(std::string& tracks) const;
    virtual bool getFirstTrack(std::string& track, std::string& form, std::string &trackUid) const;
    virtual bool getLastTrack(std::string& track, std::string& form, std::string &trackUid) const;
    virtual bool getNextTrack(std::string& track, std::string& form, std::string &trackUid,
            const int currPos) const;
    virtual bool getPrevTrack(std::string& track, std::string& form, std::string &trackUid,
            const int currPos) const;
    virtual bool getInfoAboutTrack(int pos, std::string &track, std::string &form,
            std::string &trackUid) const;
    virtual commonTrackListMap getTrackList() const;
    virtual std::string getPathToFiles() const;
    virtual int getPosition(const std::string& trackUid) const;
    virtual bool isAudio(const std::string& trackName) const;
    virtual void startVlcServer(const int pipeReadDescriptor)
    {
    }

    virtual std::list<std::string> getFolderBlacklist() const
    {
        return std::list<std::string>();
    }

    virtual ~Playlist();
private:
    void updateTrackList(const uidNameMap& newList);

    void split(const std::string& str, const std::vector<std::string>& delimiters,
            uidNameMap& list) const;

    bool isFileSupportedVideo(const std::string& filename) const;

    bool isFileSupportedAudio(const std::string& filename) const;

    std::string getFormFromName(const std::string& filename) const;

    uidNameMap mLocalTrackList;
    uidNameMap mRemoteTrackList;
    commonTrackListMap mTrackList;
    std::string mPathToFiles;
    CRWMutex* mMutex;
    static Logger msLogger;

};

#endif // CMEDIASOURCE_H
