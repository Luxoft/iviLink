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
#include "UdpSocket.hpp"
#include <strings.h>

Logger UdpSocket::logger = Logger::getInstance(LOG4CPLUS_TEXT("UdpSockets.lib"));

static const char* StatusNames[] =
{
    "SENDER",
    "RECEIVER",
    "NA"
};

UdpSocket::UdpSocket(const UInt32 portNum)
                    : mPortNumber(portNum),
                    mStatus(NA)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + printInfo());
}

void UdpSocket::initSender()
{
    mStatus = SENDER;
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + printInfo());
    mSocketId = socket(AF_INET, SOCK_DGRAM, 0);
    while (mSocketId < 0)
    {
        LOG4CPLUS_ERROR(logger, "Socket initialize error " + printInfo());
        mSocketId = socket(AF_INET, SOCK_DGRAM, 0);    
    }
    fcntl(mSocketId, F_SETFL, O_NONBLOCK);
    mAddr.sin_family = AF_INET;
    mAddr.sin_port = htons(mPortNumber);
    mAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    LOG4CPLUS_INFO(logger, "UdpClient initialized " + printInfo());
}

void UdpSocket::initReceiver()
{
    mStatus = RECEIVER;
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + printInfo());
    mSocketId = socket(AF_INET, SOCK_DGRAM, 0);
    while (mSocketId < 0)
    {
        LOG4CPLUS_ERROR(logger, "Socket initialize error " + printInfo());
        mSocketId = socket(AF_INET, SOCK_DGRAM, 0);
    }
    fcntl(mSocketId, F_SETFL, O_NONBLOCK);
    LOG4CPLUS_INFO(logger, "Udp listener initialized " + printInfo());
    mAddr.sin_family = AF_INET;
    mAddr.sin_port = htons(mPortNumber);
    mAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    while (bind(mSocketId, (struct sockaddr*) &mAddr, sizeof(mAddr)) < 0)
    {
        LOG4CPLUS_ERROR(logger, "Socket initialize error " + printInfo());
    }
    LOG4CPLUS_INFO(logger, "UDP socket binded " + printInfo());
}

UdpSocket::~UdpSocket() 
{ 
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + printInfo());
}

void UdpSocket::sendMessage(const UInt8 msg) const
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + printInfo());
    if (mStatus != SENDER)
    {
        LOG4CPLUS_WARN(logger, "Trying to use Receiver as a Sender!");
    }
    sendto(mSocketId, &msg, sizeof(msg), 0, (struct sockaddr *)&mAddr, sizeof(mAddr));
}

int UdpSocket::receiveMessage(UInt8 &message) const
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__ + printInfo());
    if (mStatus != RECEIVER)
    {
        LOG4CPLUS_WARN(logger, "Trying to use Sender as a Receiver!");
        return 0;
    }
    message = 0;
    const UInt32 messageSize = 1;
    UInt8 messageArray[messageSize];
    bzero(messageArray, messageSize);
    int bytesRead = recvfrom(mSocketId, messageArray, messageSize, 0, NULL, NULL);
    message = messageArray[0];
    LOG4CPLUS_INFO(logger, "Bytes read: " + (bytesRead > 0? convertIntegerToString(bytesRead) : "<0") 
        + " message: " + convertIntegerToString(message) + printInfo());
    return bytesRead;
}

void UdpSocket::closeSocket()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    close(mSocketId);
}

std::string UdpSocket::printInfo() const
{
    return " " + convertIntegerToString(mPortNumber) + ":" + StatusNames[mStatus];
}
