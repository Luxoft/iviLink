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
#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#define stringify( name ) # name


namespace iviLink
{
namespace SystemController
{

enum Components
{
    PROFILE_MANAGER = 0,
    CHANNEL_SUPERVISOR,
    CONNECTIVITY_AGENT,
    APPLICATION_MANAGER,
    AUTHENTICATION_APP,
    SPLASH_SCREEN,
    SYSTEM_CONTROLLER,
    SYSTEM_CONTROLLER_WATCHDOG,
};

static const char* ComponentsNames[] = 
{
    stringify(PROFILE_MANAGER),
    stringify(CHANNEL_SUPERVISOR),
    stringify(CONNECTIVITY_AGENT),
    stringify(APPLICATION_MANAGER),
    stringify(AUTHENTICATION_APP),
    stringify(SPLASH_SCREEN),
    stringify(SYSTEM_CONTROLLER)
};

} // namespace SystemController
}

#endif //COMPONENTS_HPP
