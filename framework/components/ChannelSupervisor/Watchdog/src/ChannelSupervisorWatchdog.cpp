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


#include <time.h>
#include "ChannelSupervisorWatchdog.hpp"
#include "ChannelSupervisorTube.hpp"
#include "Channel.hpp"
#include "BaseError.hpp"

 #include "PrivateAPI.hpp"

namespace iviLink
{

namespace ChannelSupervisor
{

CMutex ChannelSupervisorWatchdog::mSingletonMutex;
Logger ChannelSupervisorWatchdog::mLogger = Logger::getInstance("NegotiatorProcess.CSWatchdog.channelsWatchdog");
ChannelSupervisorWatchdog* ChannelSupervisorWatchdog::mWatchdogInstance = 0;

ChannelSupervisorWatchdog::ChannelSupervisorWatchdog() 
{
    LOG4CPLUS_TRACE_METHOD(mLogger,__PRETTY_FUNCTION__);   
}

ChannelSupervisorWatchdog::~ChannelSupervisorWatchdog()
{
    LOG4CPLUS_TRACE_METHOD(mLogger,__PRETTY_FUNCTION__);
}

ChannelSupervisorWatchdog* ChannelSupervisorWatchdog::getInstance()
{
    LOG4CPLUS_TRACE_METHOD(mLogger,__PRETTY_FUNCTION__);
    MutexLocker lock(mSingletonMutex);
    if (mWatchdogInstance == 0) 
    {
        mWatchdogInstance = new ChannelSupervisorWatchdog();
        LOG4CPLUS_INFO(mLogger, "ChannelSupervisorWatchdog created");
    }
    return mWatchdogInstance;
}
void ChannelSupervisorWatchdog::deleteInstance()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mSingletonMutex);
    if (mWatchdogInstance)
    {
        delete mWatchdogInstance;
        mWatchdogInstance = NULL;
    }
}

void ChannelSupervisorWatchdog::checkChannels()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    time_t currentTime;
    ChannelInfo channelInfo;
    UInt32 channelId = CHANNEL_ID_START_NUMBER + 1;
    for (; channelId < Map::getInstance()->getNextFreeChannelId(); ++channelId)
    {
        time(&currentTime);
        if (Map::getInstance()->getChannelInfo(channelId, channelInfo))
        {
            if (channelInfo.mTag.empty())
            {
                LOG4CPLUS_WARN(mLogger, "tag for channel id is empty " + convertIntegerToString(channelId));
                continue;
            }
            UInt32 diffMillis = (UInt32) difftime(currentTime, channelInfo.mTimeStamp) * 1000;
            LOG4CPLUS_INFO(mLogger, "diffMillis for " + convertIntegerToString(channelId) + 
                    "|" + channelInfo.mTag + " is " + convertIntegerToString(diffMillis));
            if (diffMillis > DEALLOCATE_TIMEOUT_MS)
            {
                LOG4CPLUS_ERROR(mLogger, "ChannelSupervisorWatchdog::checkChannels: channel " 
                    + convertIntegerToString(channelId) + "|" + channelInfo.mTag + " not responsive");
                ::forceChannelDeallocation(channelId);
                Map::getInstance()->invalidateChannelInfo(channelId);
                LOG4CPLUS_INFO(mLogger, "ChannelSupervisorWatchdog::checkChannels: channel " 
                    + convertIntegerToString(channelId) + " deallocated");
            }
        }
        else
        {
            LOG4CPLUS_WARN(mLogger, "channel id not in map: " + convertIntegerToString(channelId));
        }
    }
}

} // namespace iviLink
} // namespace ChannelSupervisor
