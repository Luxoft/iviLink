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
 

#ifndef CMEDIASOURCE_H
#define CMEDIASOURCE_H

#include <list>
#include <map>
#include <string>
#include <vector>

#include "../common.hpp"
#include "../IMediaSource.hpp"
#include "Logger.hpp"
#include "CRWMutex.hpp"


class CRWMutex;

class CMediaSource : public IMediaSource
{
public:
    CMediaSource(std::string pathToFiles);
    virtual ~CMediaSource();
    virtual void recallRemoteTrackList();   //on connection lost
    virtual void obtainTrackList();
    virtual void setRemoteTrackList(std::string& tracks);
    virtual void getLocalTrackList(std::string& tracks);
    virtual bool getFirstTrack(std::string& track, std::string& form, std::string &trackUid);
    virtual bool getLastTrack(std::string& track, std::string& form, std::string &trackUid);
    virtual bool getNextTrack(std::string& track, std::string& form, std::string &trackUid, int currPos);
    virtual bool getPrevTrack(std::string& track, std::string& form, std::string &trackUid, int currPos);
    virtual bool getInfoAboutTrack(int pos, std::string &track, std::string &form, std::string &trackUid);
    virtual commonTrackListMap getTrackList();
    virtual std::string getPathToFiles();
    virtual int getPosition(const std::string& trackUid);
    virtual void startVlcServer(int pipeReadDescriptor) {}


private:
    void updateTrackList(const uidNameMap&  newList);

    void split(const std::string& str, const std::vector<std::string>& delimiters , uidNameMap& list);

    uidNameMap                          mLocalTrackList;
    uidNameMap                          mRemoteTrackList;
    commonTrackListMap                  mTrackList;
    std::string                         mPathToFiles;
    CRWMutex*                           mMutex;
    static Logger                       msLogger;

};

#endif // CMEDIASOURCE_H
