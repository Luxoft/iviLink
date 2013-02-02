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


#ifndef SYSTEM_CONTROLLER_WD_COMMON_HPP
#define SYSTEM_CONTROLLER_WD_COMMON_HPP

#include <map>

#include "UdpSocket.hpp"
#include "Components.hpp"

#define UDP_PORT 3600
#define TCP_PORT 4023
#define MAX_FAIL_COUNT 5
#define SLEEP_TIMEOUT  3
#define IVILINK_START_TIMEOUT_SECONDS 180

namespace iviLink
{
namespace SystemController
{

enum Statuses
{
    IS_UP,
    IS_DOWN,
    NOT_STARTED,

};

struct Component
{
    Components component;
    Statuses status;
    int failCount;
    UdpSocket socket;

    public: 
    Component(Components comp)
            :component(comp),
            status(IS_UP),
            failCount(0),
            socket((UInt32) UDP_PORT + (UInt32)comp)
    {
        socket.initReceiver();
    }

    void resetCounters()
    {
        status = IS_UP;
        failCount = 0;
    }

    ~Component()
    {
        socket.closeSocket();
    }
};

typedef std::map<int, Component*> tComponentStatusMap;

typedef std::map<SystemController::Components, std::string> tComponentProcessNameMap;
}
}

#endif //SYSTEM_CONTROLLER_WD_COMMON_HPP
