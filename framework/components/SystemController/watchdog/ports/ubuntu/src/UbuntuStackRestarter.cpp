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
#include <string>
#include <sys/types.h>
#include <stdlib.h>

#include "UbuntuStackRestarter.hpp"

namespace iviLink
{

namespace SystemController
{

Logger UbuntuStackRestarter::mLogger = Logger::getInstance("SystemControllerWatchdog.Restarter");

void UbuntuStackRestarter::restartIviLinkFromWatchdog()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    pid_t pid = fork();
    assert(pid == 0);
    if (pid == 0)
    {
        std::string systemControllerName(SYSTEM_CONTROLLER_PROCESS_NAME);
        systemControllerName = "./" + systemControllerName;
        execl(systemControllerName.c_str(), systemControllerName.c_str(), "-r", "-1", NULL);
        exit(1);
    }
}

}
}
