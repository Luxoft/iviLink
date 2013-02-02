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
#include <stdlib.h>

#include "KickWatchdogThread.hpp"
#include "ChannelSupervisorTube.hpp"

#include "ChannelTagMap.hpp"

#define MAX_SLEEP_MS 50

using namespace iviLink::Channel;

KickWatchdogThread * KickWatchdogThread::mInstance = NULL;
CMutex KickWatchdogThread::mSingletonMutex;
Logger KickWatchdogThread::mLogger = Logger::getInstance("profileLib.Channel.KickCSWatchdogThread");

KickWatchdogThread::KickWatchdogThread()
: CThread("KickWatchdogThread"), mIsStarted(false)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    div_t divresult = div(iviLink::ChannelSupervisor::KICK_TIMEOUT_MS, MAX_SLEEP_MS);
    mSleepTimeoutParts = divresult.quot;
}

KickWatchdogThread * KickWatchdogThread::getInstance()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mSingletonMutex);
    if (mInstance == NULL)
    {
        mInstance = new KickWatchdogThread();
    }
    return mInstance;
}

void KickWatchdogThread::deleteInstance()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mSingletonMutex);
    if (mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

void KickWatchdogThread::startKicking()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mIsStartedMutex);
    if (!mIsStarted)
    {
        LOG4CPLUS_INFO(mLogger, "starting thread...");
        mIsStarted = true;
        CThread::start();
    }
}

void KickWatchdogThread::threadFunc()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    while (!getStopFlag())
    {
        LOG4CPLUS_TRACE_METHOD(mLogger, "in cycle");
        int sleepCount = 0;
        while (sleepCount < mSleepTimeoutParts && !getStopFlag())
        {
            CThread::sleep(MAX_SLEEP_MS); // so that it will be stopped faster if needed
            sleepCount ++;
        }
        if (getStopFlag())
        {
            continue;
        }
        LOG4CPLUS_INFO(mLogger, "in cycle after sleep");
        tChannelTagMap map;
        ChannelTagMap::getInstance()->getCopyOfMap(map);
        tChannelTagMap::iterator iter;
        for (iter = map.begin(); iter != map.end();  ++iter)
        {        
            BaseError err = iviLink::ChannelSupervisor::kickWatchdog(iter->first, iter->second);
            if (!err.isNoError())
            {
                LOG4CPLUS_ERROR(mLogger, "Error: channel " + convertIntegerToString(iter->first) + " wasn't kicked");
            }
            else
            {
                LOG4CPLUS_INFO(mLogger, "kicked channel " + convertIntegerToString(iter->first));
            }
        }
    }
    MutexLocker lock(mIsStartedMutex);
    mIsStarted = false;
}
