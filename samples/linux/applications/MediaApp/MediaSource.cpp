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


#include <fnmatch.h>
#include <iostream>
#include <cassert>

#include "DirectoriesScanner.hpp"

#include "MediaSource.hpp"
#include "get_mac.hpp"

#define MAX_SCAN_DEPTH 7


Logger Playlist::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.Media.CMediaSource"));

namespace
{
std::list<std::string> GetAllFiltersInList()
{
    std::list<std::string> result;
    for (int i = 0 ; i < sizeof(supported_audio_extensions)/sizeof(char*); i++)
    {
        result.push_back(supported_audio_extensions[i]);
    }
    for (int i = 0 ; i < sizeof(supported_video_extensions)/sizeof(char*); i++)
    {
        result.push_back(supported_video_extensions[i]);
    }
    return result;
}
}

Playlist::Playlist(const std::string& pathToFiles)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    mMutex = new CRWMutex();
    mPathToFiles =  pathToFiles;
}

Playlist::~Playlist()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    delete mMutex;
}

void Playlist::obtainTrackList()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::time_t trackTime;
    DirectoriesScanner scanner;
    std::list<std::string> commonScanResult 
        = scanner.scan(mPathToFiles.c_str(), GetAllFiltersInList(),
                        getFolderBlacklist(), MAX_SCAN_DEPTH);

    mMutex->lockWrite();
    mTrackList.clear();
    char mac_addr[13] = "";
    get_mac(mac_addr);
    std::string mac(mac_addr);
    trackInfo currentTrack;
    for(std::list<std::string>::iterator iter = commonScanResult.begin();
        iter != commonScanResult.end(); iter ++)
    {       
        std::string file = (*iter);
        unsigned found=file.find("//");
        if (found != std::string::npos)
        {
            file.replace(found, 2, "/");
        }
        found = file.find(mPathToFiles);
        if (found != std::string::npos)
        {
            file.replace(found, mPathToFiles.size(), "");
        }
        currentTrack.trackName = file;
        currentTrack.owner = true;
        // TODO check if this time-based art of UID is really necessary
        // TODO mark files as audio or video here
        trackTime = std::time(0);
        std::stringstream timeInStr;
        timeInStr << trackTime;
        mLocalTrackList[file + mac + timeInStr.str()] = file;
        mTrackList[file + mac + timeInStr.str()] = currentTrack;
    }

    uidNameMap::iterator itList = mRemoteTrackList.begin();
    uidNameMap::iterator endItList = mRemoteTrackList.end();

    trackInfo remoteTrack;
    remoteTrack.owner = false;

    for (; itList != endItList;) {
        
        remoteTrack.trackName = itList->second;
        mTrackList[itList->first] = remoteTrack;
        ++itList;
    }
    mMutex->unlockWrite();
}

void Playlist::updateTrackList(const uidNameMap&  newList)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    recallRemoteTrackList();

    uidNameMap::const_iterator itList = newList.begin();
    uidNameMap::const_iterator endItList = newList.end();

    trackInfo remoteTrack;
    remoteTrack.owner = false;

    mMutex->lockWrite();
    for (; itList != endItList;) 
    {
        remoteTrack.trackName = itList->second;
        mTrackList[itList->first] = remoteTrack;
        ++itList;
    }
    mRemoteTrackList = newList;
    mMutex->unlockWrite();

}

void Playlist::recallRemoteTrackList()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mMutex->lockWrite();
    commonTrackListMap::iterator iter = mTrackList.begin();
    commonTrackListMap::iterator endIter = mTrackList.end();

    for (; iter != endIter;) {
        if (iter->second.owner == false) {
            mTrackList.erase(iter++);
        } else {
            ++iter;
        }
    }
        mMutex->unlockWrite();
}

void Playlist::setRemoteTrackList(const std::string& tracks)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (tracks.empty())
    {
        return;
    }
    uidNameMap newList;
    std::vector<std::string> delimiters;
    delimiters.push_back(":");
    delimiters.push_back(";");
    split(tracks, delimiters, newList);
    updateTrackList(newList);
}

void Playlist::getLocalTrackList(std::string& tracks) const
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mMutex->lockRead();
    uidNameMap::const_iterator iter = mLocalTrackList.begin();
    uidNameMap::const_iterator endItList = mLocalTrackList.end();
    mMutex->unlockRead();

    for (; iter != endItList;) 
    {
        tracks = tracks + iter->first + ":" + iter->second + ";";
        ++iter;
    }
}

void Playlist::split(const std::string& str, const std::vector<std::string>& delimiters , uidNameMap& uidMap) const
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    std::list<std::string> list;
    std::string::size_type lastPos = str.find_first_not_of(delimiters[1], 0);
    std::string::size_type pos = str.find_first_of(delimiters[1], lastPos);
    std::string current_data;

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        list.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters[1], pos);
        pos = str.find_first_of(delimiters[1], lastPos);
    }
    std::list<std::string>::iterator iter = list.begin();
    std::list<std::string>::iterator endIter = list.end();
    for (;iter != endIter; )
    {
        current_data = iter->data();
        pos = current_data.find_first_of(delimiters[0], 0);
        if (pos == std::string::npos)
        {
             LOG4CPLUS_ERROR(msLogger, "Can not parse input string properly!");
        }
        uidMap[current_data.substr(0,pos)] = current_data.substr(pos + 1);
        ++iter; 
    }
}


bool Playlist::getLastTrack(std::string &track, std::string &form,  std::string &trackUid) const
{
    mMutex->lockRead();
    track = (--mTrackList.end())->second.trackName;
    trackUid = (--mTrackList.end())->first;
    form = getFormFromName(track);
    bool owner = (--mTrackList.end())->second.owner;
    mMutex->unlockRead();
    return owner;
}

bool Playlist::getFirstTrack(std::string &track, std::string &form,  std::string &trackUid) const
{
    mMutex->lockRead();
    track = mTrackList.begin()->second.trackName;
    trackUid = mTrackList.begin()->first;
    form = getFormFromName(track);
    bool owner = mTrackList.begin()->second.owner;
    mMutex->unlockRead();
    return owner;
}

bool Playlist::getPrevTrack(std::string &track, std::string &form, std::string &trackUid, const int currPos) const
{
    commonTrackListMap::const_iterator iter = mTrackList.begin();
    mMutex->lockRead();
    for (int i = 0; i < currPos; i++)
    {
	   ++iter;
    }
    mMutex->unlockRead();
    if (iter != mTrackList.begin())
    {
        --iter;
        track = iter->second.trackName;
        trackUid = iter->first;
        form = getFormFromName(track);
        return iter->second.owner;
    }
    else
    {
        return getLastTrack(track, form, trackUid);
    }
}

bool Playlist::getNextTrack(std::string &track, std::string &form,  std::string &trackUid, const int currPos) const
{
    commonTrackListMap::const_iterator iter = mTrackList.begin();
    mMutex->lockRead();
    for (int i = 0; i < currPos; i++)
    {
	++iter;
    }
    mMutex->unlockRead();
    if (++iter != mTrackList.end())
    {
        track = iter->second.trackName;
        trackUid = iter->first;
        form = getFormFromName(track);
        return iter->second.owner;
    }
    else
    {
        return getFirstTrack(track, form, trackUid);
    }

}

bool Playlist::getInfoAboutTrack(int newpos, std::string &track, std::string &form, std::string &trackUid) const
{
    mMutex->lockRead();
    commonTrackListMap::const_iterator iter = mTrackList.begin();
    for (int i = 0; i < newpos; i++)
    {
	   ++iter;
    }
    mMutex->unlockRead();
    track = iter->second.trackName;
    trackUid = iter->first;
    form = getFormFromName(track);
    return iter->second.owner;
}

int Playlist::getPosition(const std::string& trackUid) const
{
	mMutex->lockRead();
    commonTrackListMap::const_iterator itList = mTrackList.begin();
    commonTrackListMap::const_iterator endItList = mTrackList.end();
    int res = 0;
    for (; itList != endItList; ++itList) 
    {
        LOG4CPLUS_INFO(msLogger, "in get position: " + itList->first + " - " + trackUid);
        if(itList->first == trackUid)
			return res;
        ++res;
    }
	mMutex->unlockRead();
	return 0;
}

commonTrackListMap Playlist::getTrackList() const
{
    return mTrackList;
}

std::string Playlist::getPathToFiles() const
{
    return mPathToFiles;
}

bool Playlist::isFileSupportedAudio(const std::string& filename) const
{
    static const int audioExtensionsSize = sizeof(supported_audio_extensions)/sizeof(char*);
    for(int i=0; i < audioExtensionsSize; i++)
    {   
        if (fnmatch(supported_audio_extensions[i], filename.c_str(), FNM_CASEFOLD) == 0) 
        {
            return true;
        }
    }
    return false;
}

bool Playlist::isFileSupportedVideo(const std::string& filename) const
{
    static const int videoExtensionsSize = sizeof(supported_video_extensions)/sizeof(char*);
    for(int i=0; i < videoExtensionsSize; i++)
    {   
        if (fnmatch(supported_video_extensions[i], filename.c_str(), FNM_CASEFOLD) == 0) 
        {
            return true;
        }
    }
    return false;
}

std::string Playlist::getFormFromName(const std::string& filename) const
{
    if(isFileSupportedVideo(filename))
    {
        return VIDEO;
    } 
    else if(isFileSupportedAudio(filename))
    {
        return AUDIO;
    } 
    else 
    {
        assert(false);
        return "";
    }
}

bool Playlist::isAudio(const std::string& trackName) const
{
    return getFormFromName(trackName) == AUDIO;
}
