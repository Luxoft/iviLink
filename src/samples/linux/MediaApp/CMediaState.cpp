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
 

#include <string>
#include <stdio.h>

#include "CRWMutex.hpp"
#include "CMediaState.hpp"

    Logger CMediaState::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("samples.Applications.Media.CMediaState"));

    static const char* playBack_strs[] = {"play", "pause", "stop"}; 
    static const char* role_strs[] = {"client","server"};  
    static const char* connections_strs[] = {"connected", "disconnected"};  
    static const char* stream_strs[] = {"myside", "otherside", "both"};   


    void CMediaState::setPlayBackState(playBack state)
    {
        LOG4CPLUS_INFO(msLogger, "PlayBack state: " + std::string(playBack_strs[playBackState]) + " -> " +  std::string(playBack_strs[state]) );
        playBackState = state;
    }

    void CMediaState::setConnectionState(connections state)
    {
        LOG4CPLUS_INFO(msLogger, "connections state: " + std::string(connections_strs[connectionState]) + " -> " +  std::string(connections_strs[state]) );
        connectionState = state;
    }

    void CMediaState::setRoleState(role state)
    {
        LOG4CPLUS_INFO(msLogger, "role state: " + std::string(role_strs[roleState]) + " -> " +  std::string(role_strs[state]) );
        roleState = state;
    }

    void CMediaState::setStreamState(stream state)
    {
       LOG4CPLUS_INFO(msLogger, "stream state: " + std::string(stream_strs[streamState]) + " -> " +  std::string(stream_strs[state]) );
       streamState = state;
    }

    void CMediaState::setSource(std::string state)
    {
        LOG4CPLUS_INFO(msLogger, "source: " + source + " -> " +  state);
        source = state;
    }

    void CMediaState::setCurrTrack(std::string state)
    {
        LOG4CPLUS_INFO(msLogger, "currTrack: " + currTrack + " -> " +  state);
        currTrack = state;
    }

    void CMediaState::setAVFormat(std::string state)
    {
       LOG4CPLUS_INFO(msLogger, "AVFormat: " + AVFormat + " -> " +  state);
       AVFormat = state;
    }

    playBack CMediaState::getPlayBackState()
    {
        LOG4CPLUS_INFO(msLogger, "playBack state is: " + std::string(playBack_strs[playBackState]));
        return playBackState;
    }

    connections CMediaState::getConnectionState()
    {
        LOG4CPLUS_INFO(msLogger, "Connection state is: " + std::string(connections_strs[connectionState]));
        return connectionState;
    }

    role CMediaState::getRoleState()
    {
        LOG4CPLUS_INFO(msLogger, "Role state is: " + std::string(role_strs[roleState]));
        return roleState;
    }

    stream CMediaState::getStreamState()
    {
        LOG4CPLUS_INFO(msLogger, "Stream state is: " + std::string(stream_strs[streamState]));
        return streamState;
    }

    std::string CMediaState::getSource()
    {
        LOG4CPLUS_INFO(msLogger, "playBack state is: " + source);
        return source;
    }

    std::string CMediaState::getCurrTrack()
    {
        LOG4CPLUS_INFO(msLogger, "playBack state is: " + currTrack);
        return currTrack;
    }

    std::string CMediaState::getAVFormat()
    {
        LOG4CPLUS_INFO(msLogger, "playBack state is: " + AVFormat);
        return AVFormat;
    }


