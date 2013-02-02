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
#ifndef __CHANNEL_TAG_MAP_HPP
#define __CHANNEL_TAG_MAP_HPP

#include <map>

#include "Logger.hpp"
#include "CRWMutex.hpp"
#include "CMutex.hpp"
#include "Channel.hpp"

namespace iviLink
{

namespace Channel
{

typedef std::map<tChannelId, std::string> tChannelTagMap;

class ChannelTagMap 
{
public:
    static ChannelTagMap * getInstance();
    static void deleteInstance();
    void getCopyOfMap(tChannelTagMap& map);

    void insertIntoMap(std::string const& tag, UInt32 channelID);
    void eraseFromMap(UInt32 channelID);

    bool hasChannelInMap(UInt32 channelID);
private:
    ChannelTagMap() {}
    ~ChannelTagMap() {}
    ChannelTagMap(ChannelTagMap const&); // not implemented
    ChannelTagMap& operator=(ChannelTagMap const&); // not implemented
private:
    tChannelTagMap mMap;
    CRWMutex mMapMutex;

    static ChannelTagMap * mInstance;
    static CMutex mSingletonMutex;
    static Logger mLogger;
};

}

}


#endif // __CHANNEL_TAG_MAP_HPP
