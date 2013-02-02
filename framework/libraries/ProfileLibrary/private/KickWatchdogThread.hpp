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
#ifndef __KICK_WD_THREAD_HPP
#define __KICK_WD_THREAD_HPP

#include "CThread.hpp"
#include "Logger.hpp"
#include "CMutex.hpp"

namespace iviLink
{

namespace Channel
{

class KickWatchdogThread : public CThread
{
    KickWatchdogThread();
    virtual ~KickWatchdogThread() {}
public:
    void startKicking();
    static KickWatchdogThread * getInstance();
    static void deleteInstance();

private:
    virtual void threadFunc();
private:
    bool mIsStarted;
    CMutex mIsStartedMutex;

    UInt32 mSleepTimeoutParts; // will sleep KICK_TIMEOUT_MS between kicks, but split in portions

    static KickWatchdogThread * mInstance;
    static CMutex mSingletonMutex;
    static Logger mLogger;
};

}

}

#endif // __KICK_WD_THREAD_HPP
