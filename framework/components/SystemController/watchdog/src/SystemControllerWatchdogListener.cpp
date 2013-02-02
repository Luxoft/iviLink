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
#include "SystemControllerWatchdogListener.hpp"
#include "SystemControllerWatchdogCommon.hpp"
#include "SystemControllerWatchdog.hpp"

Logger iviLink::SystemController::SystemControllerWatchdogListener::mLogger = Logger::getInstance("SystemControllerWatchdog.Listener");

namespace iviLink
{
namespace SystemController
{

SystemControllerWatchdogListener::SystemControllerWatchdogListener(IStackRestarter * restarter)
: SystemControllerProxy("SysCtrl_Watchdog")
, mRestarter(restarter)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mStackStartTimeout = new StackStartTimeoutWatcher(this);

    BaseError err = SystemControllerProxy::connectSystemController();

    if (!err.isNoError())
    {
        LOG4CPLUS_FATAL(mLogger, "Unable to establish connection to system controller " + (std::string)err);
        mRestarter->restartIviLinkFromWatchdog();
    }
    mStackStartTimeout->startWait();
    SystemControllerWatchdog::getInstance()->addComponent(new Component(SYSTEM_CONTROLLER));
}

void SystemControllerWatchdogListener::onTimeout()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mRestarter->restartIviLinkFromWatchdog();
}

void SystemControllerWatchdogListener::onSystemControllerNotAvaliable()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_WARN(mLogger, "IPC disconnected");
}

void SystemControllerWatchdogListener::onPhysicalLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "CONNECTIVITY_AGENT_LAUNCHED");
    SystemControllerWatchdog::getInstance()->addComponent(new Component(CONNECTIVITY_AGENT));
}

void SystemControllerWatchdogListener::onChannelLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "CHANNEL_SUPERVISOR_LAUNCHED");
    SystemControllerWatchdog::getInstance()->addComponent(new Component(CHANNEL_SUPERVISOR));
}

void SystemControllerWatchdogListener::onProfileLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "PROFILE_MANAGER_LAUNCHED");
    SystemControllerWatchdog::getInstance()->addComponent(new Component(PROFILE_MANAGER));
}

void SystemControllerWatchdogListener::onApplicationLayerStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "APPLICATION_MANAGER_LAUNCHED");
    SystemControllerWatchdog::getInstance()->addComponent(new Component(APPLICATION_MANAGER));
}

void SystemControllerWatchdogListener::onAuthenticationStarted()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "AUTHENTICATION_APP");
    SystemControllerWatchdog::getInstance()->addComponent(new Component(AUTHENTICATION_APP));
}

void SystemControllerWatchdogListener::onAuthenticationStopped()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "AUTHENTICATION_FINISHED");          
    SystemControllerWatchdog::getInstance()->removeComponent(SystemController::AUTHENTICATION_APP); 
    mStackStartTimeout->stopWait();
}

} // namespace SystemController
} // namespace iviLink
