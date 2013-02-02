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

#include "ChannelSupervisorWatchdogThread.hpp"

using namespace iviLink::ChannelSupervisor;

#define CSWATCHDOG_THREAD_NAME "ChannelSupervisorWatchdogThread"

#define WD_MAX_SLEEP_TIMEOUT_MS 50

ChannelSupervisorWatchdogThread::ChannelSupervisorWatchdogThread()
:CThread(CSWATCHDOG_THREAD_NAME)
{
    div_t sleepyTimes = div(WD_SLEEP_TIMEOUT_MS, WD_MAX_SLEEP_TIMEOUT_MS);
    mSleepTimeoutParts = sleepyTimes.quot;
}

void ChannelSupervisorWatchdogThread::threadFunc()
{
    ChannelSupervisorWatchdog * watchdog = ChannelSupervisorWatchdog::getInstance();
    while (!getStopFlag())
    {
        watchdog->checkChannels();
        UInt32 sleepCounter = 0;
        while (sleepCounter < mSleepTimeoutParts && !getStopFlag())
        {
            CThread::sleep(WD_MAX_SLEEP_TIMEOUT_MS); // to speed up exiting
            sleepCounter ++;
        }
    }   
    ChannelSupervisorWatchdog::deleteInstance();
}
