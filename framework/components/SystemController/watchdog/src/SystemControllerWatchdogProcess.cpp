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
#include <pthread.h>

#include "SystemControllerWatchdog.hpp"
#include "TcpSocket.hpp"
#include "SystemControllerWatchdogListener.hpp"
#include "SystemControllerWatchdogProcess.hpp"

namespace iviLink
{
namespace SystemController
{

Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("SystemControllerWatchdog.main"));

int watchdogProcessEntryPoint(IStackRestarter * restarter)
{
    LOG4CPLUS_INFO(logger, "System Controller Watchdog started...");
    SystemControllerWatchdog * watchdog = SystemControllerWatchdog::getInstance();
    SystemControllerWatchdogListener listener(restarter);

    while (true)
    {
        if (!watchdog->waitLoop())
        {
            LOG4CPLUS_ERROR(logger, "iviLink will be restarted from System Controller Watchdog...");
            restarter->restartIviLinkFromWatchdog();
        }
        sleep(SLEEP_TIMEOUT);
    }
    return 0;
}

} // namespace SystemController
} // namespace iviLink


// Ubuntu's process entry point
#ifndef ANDROID
#include "UbuntuStackRestarter.hpp"

using namespace iviLink::SystemController;
int main(int argc, char ** argv)
{
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    UbuntuStackRestarter * restarter = new UbuntuStackRestarter();
    return iviLink::SystemController::watchdogProcessEntryPoint(restarter);
}
#endif //ANDROID
