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


#ifndef CHANNEL_SUPERVISOR_WATCHDOG_THREAD_HPP
#define CHANNEL_SUPERVISOR_WATCHDOG_THREAD_HPP

#include "CThread.hpp"
#include "Logger.hpp"
#include "ChannelSupervisorWatchdog.hpp"
#include "Types.hpp"
#include "Common.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{

/**
 * Watchdog thread that checks is alive channel or not
 */
class ChannelSupervisorWatchdogThread : public CThread
{
public:
    virtual ~ChannelSupervisorWatchdogThread() {}
    ChannelSupervisorWatchdogThread();

private:
    virtual void threadFunc();

private:
    UInt32 mSleepTimeoutParts;
};

}; // namespace ChannelSupervisor
}; // namespace iviLink

#endif // CHANNEL_SUPERVISOR_WATCHDOG_THREAD_HPP
