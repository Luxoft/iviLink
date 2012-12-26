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
 

#include <dirent.h>
#include <iostream>
#include <cassert>

#include "CMediaSource.h"
#include "get_mac.hpp"

Logger CMediaSource::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.Media.CMediaSource"));

CMediaSource::CMediaSource(std::string pathToFiles)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    mMutex = new CRWMutex();
    mPathToFiles =  pathToFiles;
}

CMediaSource::~CMediaSource()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    delete mMutex;
}

void CMediaSource::obtainTrackList()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    struct dirent *ent;

    DIR *dir = NULL;
    std::time_t trackTime;

    trackInfo currentTrack;
    dir = opendir(mPathToFiles.c_str());  
    if (!dir)
    {
        LOG4CPLUS_ERROR(msLogger, "No such directory!");
        LOG4CPLUS_ERROR(msLogger, "Please specify the correct path to your media files in common.hpp as LOCAL_DIRECTORY.");
        return;
    }
    mMutex->lockWrite();

    mTrackList.clear();
    char mac_addr[13] = "";
    get_mac(mac_addr);
    std::string mac(mac_addr);

    while ((ent = readdir(dir)) != false) 
    {
        std::string file(ent->d_name);
        size_t pos_audio = file.find(AUDIO_EXTENSION);
        size_t pos_video = file.find(VIDEO_EXTENSION);
        if (pos_audio != std::string::npos || pos_video != std::string::npos)
        {
            LOG4CPLUS_INFO(msLogger, "new file found " + file);
            currentTrack.trackName = file;
            currentTrack.owner = true;
            trackTime = std::time(0);
            std::stringstream timeInStr;
            timeInStr << trackTime;
            mLocalTrackList[file + mac + timeInStr.str()] = file;
            mTrackList[file + mac + timeInStr.str()] = currentTrack;
        }
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

void CMediaSource::updateTrackList(const uidNameMap&  newList)
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

void CMediaSource::recallRemoteTrackList()
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

void CMediaSource::setRemoteTrackList(std::string& tracks)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    uidNameMap newList;
    std::vector<std::string> delimiters;
    delimiters.push_back(":");
    delimiters.push_back(";");
    split(tracks, delimiters, newList);
    updateTrackList(newList);
}

void CMediaSource::getLocalTrackList(std::string& tracks)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    mMutex->lockRead();
    uidNameMap::iterator iter = mLocalTrackList.begin();
    uidNameMap::iterator endItList = mLocalTrackList.end();
    mMutex->unlockRead();

    for (; iter != endItList;) 
    {
        tracks = tracks + iter->first + ":" + iter->second + ";";
        ++iter;
    }
}

void CMediaSource::split(const std::string& str, const std::vector<std::string>& delimiters , uidNameMap& uidMap)
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


bool CMediaSource::getLastTrack(std::string &track, std::string &form,  std::string &trackUid)
{
    mMutex->lockRead();
    track = (--mTrackList.end())->second.trackName;
    trackUid = (--mTrackList.end())->first;
    mMutex->unlockRead();

    if (track.find(AUDIO_EXTENSION) != std::string::npos)
    {
        form = AUDIO;
    }
    else if  (track.find(VIDEO_EXTENSION) != std::string::npos)
    {
        form = VIDEO;
    }

    return (--mTrackList.end())->second.owner;
}

bool CMediaSource::getFirstTrack(std::string &track, std::string &form,  std::string &trackUid)
{
    mMutex->lockRead();
    track = mTrackList.begin()->second.trackName;
    trackUid = mTrackList.begin()->first;
    mMutex->unlockRead();
    if (track.find(AUDIO_EXTENSION) != std::string::npos)
    {
        form = AUDIO;
    }
    else if  (track.find(VIDEO_EXTENSION) != std::string::npos)
    {
        form = VIDEO;
    }

    return mTrackList.begin()->second.owner;
}

bool CMediaSource::getPrevTrack(std::string &track, std::string &form, std::string &trackUid, int currPos)
{
    commonTrackListMap::iterator iter = mTrackList.begin();
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
        if (track.find(AUDIO_EXTENSION) != std::string::npos)
        {
            form = AUDIO;
        }
        else if  (track.find(VIDEO_EXTENSION) != std::string::npos)
        {
            form = VIDEO;
        }
        return iter->second.owner;
    }
    else
    {
        return getLastTrack(track, form, trackUid);
    }
}

bool CMediaSource::getNextTrack(std::string &track, std::string &form,  std::string &trackUid, int currPos)
{
    commonTrackListMap::iterator iter = mTrackList.begin();
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
        if (track.find(AUDIO_EXTENSION) != std::string::npos)
        {
            form = AUDIO;
        }
        else if  (track.find(VIDEO_EXTENSION) != std::string::npos)
        {
            form = VIDEO;
        }
        return iter->second.owner;
    }
    else
    {
        return getFirstTrack(track, form, trackUid);
    }

}

bool CMediaSource::getInfoAboutTrack(int newpos, std::string &track, std::string &form, std::string &trackUid)
{
    mMutex->lockRead();
    commonTrackListMap::iterator iter = mTrackList.begin();
    for (int i = 0; i < newpos; i++)
    {
	++iter;
    }
    mMutex->unlockRead();
    track = iter->second.trackName;
    trackUid = iter->first;
    if (track.find(AUDIO_EXTENSION) != std::string::npos)
    {
            form = AUDIO;
    }
    else if  (track.find(VIDEO_EXTENSION) != std::string::npos)
    {
            form = VIDEO;
    }
    return iter->second.owner;
}

int CMediaSource::getPosition(const std::string& trackUid)
{
	mMutex->lockRead();
        commonTrackListMap::iterator itList = mTrackList.begin();
        commonTrackListMap::iterator endItList = mTrackList.end();
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

commonTrackListMap CMediaSource::getTrackList() 
{
    return mTrackList;
}

std::string CMediaSource::getPathToFiles()
{
    return mPathToFiles;
}
