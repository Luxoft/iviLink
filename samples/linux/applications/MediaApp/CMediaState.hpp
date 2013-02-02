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


#ifndef CMEDIASTATE_HPP
#define CMEDIASTATE_HPP

#include <string>
#include <stdio.h>

#include "Logger.hpp"
#include "CRWMutex.hpp"
#include "CMutex.hpp"
#include "MediaAppCommon.hpp"

class CMediaState
{
    friend class StateLocker;
private:
    playBack playBackState;
    connections connectionState;
    role roleState;
    stream streamState;
    std::string source;         //server address and sream 
    std::string currTrack;      //current track uid
    std::string AVFormat;       //audio or video
    int mCurrPos;
    static Logger   msLogger;   //Logger
    CMutex mMutex;

public:

    CMediaState();
    ~CMediaState() {}

    void setPlayBackState(const playBack state);
    void setConnectionState(const connections state);
    void setRoleState(const role state);
    void setStreamState(const stream state);
    void setSource(const std::string& state);
    void setCurrTrack(const std::string& state);
    void setAVFormat(const std::string& state);
    void setCurrentPosition(const int& newPosition);
    playBack getPlayBackState() const;
    connections getConnectionState() const;
    role getRoleState() const;
    stream getStreamState() const;
    std::string getSource() const;
    std::string getCurrTrack() const;
    std::string getAVFormat() const;
    int getCurrentPosition() const;

    void lock();
    void unlock();
};

class StateLocker
{
    CMediaState* mMediaState;
    bool mNeedLock;
public:
    explicit StateLocker(CMediaState* mediaState, bool needLock)
    {
        mNeedLock = needLock;
        mMediaState = mediaState;
        if(mNeedLock)
        {
            mMediaState->lock();
        }
    }

    ~StateLocker()
    {
        if(mNeedLock)
        {
            mMediaState->unlock();
        }
    }
private:
    StateLocker( const StateLocker& );
    StateLocker& operator=( const StateLocker& );
};

#endif // CMEDIASTATE_HPP
