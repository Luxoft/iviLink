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
 

#ifndef CMEDIASTATE_HPP
#define CMEDIASTATE_HPP

#include <string>
#include <stdio.h>

#include "Logger.hpp"
#include "CRWMutex.hpp"
#include "common.hpp"

class CMediaState
{
private:

    playBack playBackState;
    connections connectionState;
    role roleState;
    stream streamState;
    std::string source;         //server address and sream 
    std::string currTrack;      //current track uid
    std::string AVFormat;       //audio or video
    static Logger   msLogger;   //Logger

public:

    CMediaState(): playBackState(STOP), connectionState(DISCONNECTED), roleState(SERVER), streamState(BOTH), source(""), currTrack(""), AVFormat("") {}
    ~CMediaState() {}


    void setPlayBackState(playBack state);
    void setConnectionState(connections state);
    void setRoleState(role state);
    void setStreamState(stream state);
    void setSource(std::string state);
    void setCurrTrack(std::string state);
    void setAVFormat(std::string state);
    playBack getPlayBackState();
    connections getConnectionState();
    role getRoleState();
    stream getStreamState();
    std::string getSource();
    std::string getCurrTrack();
    std::string getAVFormat();

};


#endif // CMEDIASTATE_HPP
