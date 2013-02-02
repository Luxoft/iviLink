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


#include <iostream>
#include <cassert>
#include "CMutex.hpp"
#include "Types.hpp"
#include "Map.hpp"

using iviLink::ChannelSupervisor::Map;

CMutex Map::m_SingletonMutex;
Map* Map::m_MapInstance = 0;

Logger Map::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("NegotiatorProcess.Map"));

Map* Map::getInstance()
{
    m_SingletonMutex.lock();
    if (m_MapInstance == NULL)
    {
        m_MapInstance = new Map();
    }
    m_SingletonMutex.unlock();
    return m_MapInstance;
}

void Map::deleteInstance()
{
    m_SingletonMutex.lock();
    delete this;
    m_MapInstance = NULL;
    m_SingletonMutex.unlock();
}

Map::Map()
{
    m_FreeChannelId = CHANNEL_ID_START_NUMBER;
}

Map::~Map()
{
}

bool Map::getChannelInfo(UInt32 const channelId, ChannelInfo& result)
{
    LOG4CPLUS_TRACE(msLogger,
            "Map::getChannelInfo()=> channel id: " + convertIntegerToString(channelId));

    bool res = false;

    m_mapMutex.lockRead();
    ChannelMap::iterator it = m_channelMap.find(channelId);
    if (it != m_channelMap.end())
    {
        LOG4CPLUS_INFO(msLogger, "Map::getChannelInfo()=>channel exist");

        result = it->second;
        res = true;
    }
    m_mapMutex.unlockRead();
    return res;
}

void Map::addChannelInfo(UInt32 const channelId, ChannelInfo const& channelInfo)
{
    LOG4CPLUS_TRACE(msLogger,
            "Map::addChannelInfo()=> channel id: " + convertIntegerToString(channelId));

    m_mapMutex.lockWrite();

    ChannelMap::iterator it = m_channelMap.find(channelId);
    if (it != m_channelMap.end())
    {
        LOG4CPLUS_WARN(msLogger, "Map::addChannelInfo()=>channel id already exist, replacing");
    }

    m_channelMap.insert(std::make_pair(channelId, channelInfo));
    updateFreeChannel(channelId);

    m_mapMutex.unlockWrite();
}

void Map::updateChannelInfo(UInt32 const channelId, ChannelInfo const& channelInfo)
{
    LOG4CPLUS_TRACE_METHOD(msLogger,
            "Map::updateChannelInfo()=> channel id: " + convertIntegerToString(channelId));

    m_mapMutex.lockWrite();

    ChannelMap::const_iterator it = m_channelMap.find(channelId);
    if (it == m_channelMap.end())
    {
        //not found
        LOG4CPLUS_WARN(msLogger, "Map::updateChannelInfo()=>channel id not found, inserting new");
        updateFreeChannel(channelId);
    }

    m_channelMap[channelId] = channelInfo;
    m_mapMutex.unlockWrite();
}

UInt32 Map::getNextFreeChannelId(UInt32 channelId /* = 0 */)
{
    if (channelId < m_FreeChannelId)
    {
        return m_FreeChannelId;
    }
    updateFreeChannel(channelId);
    return m_FreeChannelId;
}

void Map::invalidateChannelInfo(UInt32 const channelId)
{
    ChannelInfo info("", 0);
    updateChannelInfo(channelId, info);
}

bool Map::isChannelok(UInt32 channelId)
{
    return (channelId >= m_FreeChannelId);
}

UInt32 Map::getFreeChannelId()
{
    return m_FreeChannelId;
}

void Map::updateFreeChannel(UInt32 lastChannel)
{
    if (lastChannel >= m_FreeChannelId)
    {
        m_FreeChannelId = ++lastChannel;   
    }
}
