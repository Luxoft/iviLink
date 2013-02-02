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


#ifndef MAP_H
#define MAP_H

#include <ctime>
#include <map>
#include <string>
#include "CRWMutex.hpp"
#include "CMutex.hpp"
#include "Types.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{

struct ChannelInfo;

typedef std::map<UInt32, ChannelInfo> ChannelMap;

struct ChannelInfo
{
    time_t mTimeStamp;  //timestamp when channel was
    std::string mTag;        //tag of the profile using the channel

    ChannelInfo()
        : mTag("")
        , mTimeStamp(0)
    {
    }

    ChannelInfo(const std::string& tag, const time_t time)
        : mTag(tag)
        , mTimeStamp(time)
    {
    }
};

class Map
{
public:
    static Map* getInstance();
    void deleteInstance();

    bool getChannelInfo(UInt32 const channelId, ChannelInfo & result);
    void addChannelInfo(UInt32 const channelId, ChannelInfo const& channelInfo);
    void updateChannelInfo(UInt32 const channelId, ChannelInfo const& channelInfo);
    UInt32 getNextFreeChannelId(UInt32 channelId = 0);
    void invalidateChannelInfo(UInt32 const channelId);
    bool isChannelok(UInt32 channelId);

private:
    Map();
    ~Map();

    UInt32 getFreeChannelId();
    void updateFreeChannel(UInt32 lastChannel);

    ChannelMap m_channelMap;
    CRWMutex m_mapMutex;
    static Map* m_MapInstance;
    static CMutex m_SingletonMutex;

    UInt32 m_FreeChannelId;
    static Logger msLogger;
};

}  // namespace ChannelSupervisor
}  // namespace AXIS

#endif //MAP_H
