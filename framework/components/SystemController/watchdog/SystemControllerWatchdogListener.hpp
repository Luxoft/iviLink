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
#ifndef SYSTEM_CONTROLLER_WATCHDOG_LISTENER_HPP
#define SYSTEM_CONTROLLER_WATCHDOG_LISTENER_HPP

#include "SystemControllerProxy.hpp"
#include "StackStartTimeoutWatcher.hpp"
#include "IStackRestarter.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace SystemController
{

class SystemControllerWatchdogListener : public SystemControllerMsgProtocol::SystemControllerProxy, 
                                         public IStackStartTimeoutWatcherCallback
{
public:
    SystemControllerWatchdogListener(IStackRestarter * restarter);
    virtual void onTimeout();
    virtual ~SystemControllerWatchdogListener() 
    {
        delete mStackStartTimeout;
    }

private:
    virtual void onSystemControllerNotAvaliable();

    virtual void onPhysicalLayerStarted();
    virtual void onChannelLayerStarted();
    virtual void onProfileLayerStarted();
    virtual void onApplicationLayerStarted();
    virtual void onAuthenticationStarted();
    virtual void onAuthenticationStopped();
    static Logger mLogger;

    IStackRestarter * mRestarter;
    StackStartTimeoutWatcher * mStackStartTimeout;
};

}; // namespace SystemController
}; // namespace iviLink

#endif //SYSTEM_CONTROLLER_WATCHDOG_LISTENER_HPP
