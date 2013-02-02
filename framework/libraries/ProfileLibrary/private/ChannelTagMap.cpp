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
#include "ChannelTagMap.hpp"

using namespace iviLink::Channel;

CMutex ChannelTagMap::mSingletonMutex;
ChannelTagMap * ChannelTagMap::mInstance = NULL;
Logger ChannelTagMap::mLogger = Logger::getInstance("profileLib.Channel.TagMap");

ChannelTagMap * ChannelTagMap::getInstance()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mSingletonMutex);
    if (mInstance == NULL)
    {
        mInstance = new ChannelTagMap();
    }
    return mInstance;
}

void ChannelTagMap::deleteInstance()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mSingletonMutex);
    if (mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

void ChannelTagMap::insertIntoMap(std::string const& tag, UInt32 channelID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "insert: " + tag + "::" + convertIntegerToString(channelID));
    mMapMutex.lockWrite();
    mMap[channelID] = tag;
    mMapMutex.unlockWrite();
}

void ChannelTagMap::eraseFromMap(UInt32 channelID)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "erase: " + convertIntegerToString(channelID));
    mMapMutex.lockWrite();
    if (mMap.find(channelID) != mMap.end())
    {
        mMap.erase(channelID);
    }
    mMapMutex.unlockWrite();
}

void ChannelTagMap::getCopyOfMap(tChannelTagMap & map)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    map.clear();
    mMapMutex.lockRead();
    if (!mMap.empty())
    {
        map.insert(mMap.begin(), mMap.end());
    }
    mMapMutex.unlockRead();
}

bool ChannelTagMap::hasChannelInMap(UInt32 channelID)
{
    bool result = false;
    mMapMutex.lockRead();
    result = (mMap.find(channelID) != mMap.end());
    mMapMutex.unlockRead();
    return result;
}
