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
#ifndef UDPSOCK_HPP
#define UDPSOCK_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "Logger.hpp"
#include "Types.hpp"

class UdpSocket
{

    enum Status
    {
        SENDER,
        RECEIVER,
        NA
    };
private:
    static Logger logger;
    UInt32 mPortNumber;
    int mSocketId;
    struct sockaddr_in mAddr;
    Status mStatus;
public:
    UdpSocket(const UInt32 portNum);

    /*
    * init client side
    */
    void initSender();

    /*
    * init server side
    */
    void initReceiver();

    virtual ~UdpSocket();

    /*
    * send message 
    */
    void sendMessage(const UInt8) const;

    /*
    * receive message
    */
    int receiveMessage(UInt8 &message) const;

    std::string printInfo() const;

    void closeSocket();
};

#endif /* UDPSOCKET_HPP */
