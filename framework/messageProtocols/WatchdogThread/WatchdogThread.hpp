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
#ifndef WATCH_DOG_THREAD_HPP
#define WATCH_DOG_THREAD_HPP

#include "CThread.hpp"
#include "UdpSocket.hpp"
#include "Types.hpp"

#define SLEEP_TIMEOUT_MILLIS 3000
#define THREAD_NAME "WatchdogThread"
#define WATCHDOG_BASE_UDP_PORT 3600
#define DEFAULT_MESSAGE 1

class WatchdogThread: public CThread
{

    const UInt32 mUdpPort;
    const UInt32 mMessageToSend;
public:
    WatchdogThread(const UInt32 udpPortOffset, const UInt32 message = DEFAULT_MESSAGE)
    : CThread(THREAD_NAME),
      mUdpPort(WATCHDOG_BASE_UDP_PORT + udpPortOffset),
      mMessageToSend(message)
    {
    }
private:
     virtual void threadFunc()
     {    
        UdpSocket udpSocket(mUdpPort);
        udpSocket.initSender();
        while (true)
        {
            udpSocket.sendMessage((UInt8) mMessageToSend);
            sleep(SLEEP_TIMEOUT_MILLIS);
        }
     }

};

#endif //WATCH_DOG_THREAD_HPP
