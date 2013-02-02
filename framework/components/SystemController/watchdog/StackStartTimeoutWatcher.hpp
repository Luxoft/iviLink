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
#ifndef STACK_START_TIMEOUT_WATCHER_HPP
#define STACK_START_TIMEOUT_WATCHER_HPP

#include "CThread.hpp"
#include "CSignalSemaphore.hpp"
#include "SystemControllerWatchdogCommon.hpp"

namespace iviLink
{

namespace SystemController
{

class IStackStartTimeoutWatcherCallback
{
public:
    /**
     * The callback is invoked if stopWait() has not been called after startWait() in time
     */
    virtual void onTimeout() = 0;
    virtual ~IStackStartTimeoutWatcherCallback()
    {
    }
};

class StackStartTimeoutWatcher : public CThread
{
public:

    StackStartTimeoutWatcher(IStackStartTimeoutWatcherCallback * callback);
    virtual ~StackStartTimeoutWatcher()
    {
    }

    /**
     * Starts the internal thread that waits no longer than IVILINK_START_TIMEOUT_SECONDS for stopWait() to be called.
     * In case stopWait() has not been called in time, IStackStartTimeoutWatcherCallback's onTimeout() will be invoked.
     */
    void startWait();

    void stopWait();

private:
    CSignalSemaphore mSemaphore;
    IStackStartTimeoutWatcherCallback * mCallback;

    virtual void threadFunc();

    static Logger mLogger;
};

}

}

#endif //STACK_START_TIMEOUT_WATCHER_HPP
