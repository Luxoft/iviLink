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


#ifndef CHANNEL_SUPERVISOR_WATCHDOG_HPP
#define CHANNEL_SUPERVISOR_WATCHDOG_HPP

#include <string>
#include <cstring>

#include "Common.hpp"
#include "Map.hpp"
#include "CMutex.hpp"
#include "CSError.hpp"
#include "Logger.hpp"
#include "Types.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{

/**
 * ChannelSupervisorWatchdog implemented as Singleton.
 * Its task is observing the usage of the all allocated channels.
 * 
 */
class ChannelSupervisorWatchdog
{
private:

    /**
     * ChannelSupervisor Watchdog singleton instance
     */
    static ChannelSupervisorWatchdog * mWatchdogInstance;
    
    /**
     * Mutex needed to provide right behavior in case of concurrent calls
     */
    static CMutex mSingletonMutex;

    /**
     * Constructor is private to provide sigleton behavior
     */
    ChannelSupervisorWatchdog();
	
    /**
     * Destructor
     */
    ~ChannelSupervisorWatchdog();
	
public:
    /**
     * Interface function to provide sigleton behavior
     * @return pointer to existing instance of singleton and creates it if not exists
     */
    static ChannelSupervisorWatchdog* getInstance();

    /**
     * Interface function to destroy the singleton instance
     */
    static void deleteInstance();

    void checkChannels();

private:
    static Logger mLogger;
};

} // namespace ChannelSupervisor
} // namespace iviLink

#endif // CHANNEL_SUPERVISOR_WATCHDOG_HPP
