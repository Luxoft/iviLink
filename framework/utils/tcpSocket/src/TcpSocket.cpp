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
#include "TcpSocket.hpp"

Logger TcpSocket::logger = Logger::getInstance(LOG4CPLUS_TEXT("TcpSocket.lib"));

TcpSocket::TcpSocket(const UInt32 portNum)
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    portNumber = portNum;
}

void TcpSocket::initServer()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    listener = -1;
    while (listener < 0)
    {
        listener = socket(AF_INET, SOCK_STREAM, 0);
        LOG4CPLUS_ERROR(logger, "Socket initialize error");
    }
    LOG4CPLUS_INFO(logger, "Tcp listener initialized");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portNumber);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    while (bind(listener, (struct sockaddr*) &addr, sizeof(addr)) < 0);
    {
        LOG4CPLUS_ERROR(logger, "Socket initialize error");
    }
    LOG4CPLUS_INFO(logger, "Tcp socket binded");
    listen(listener, 1);
}

TcpSocket::~TcpSocket(){}

void TcpSocket::initClient()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    socketId = socket(AF_INET, SOCK_STREAM, 0);
    while (socketId < 0)
    {
        LOG4CPLUS_ERROR(logger, "Socket initialize error");
        socketId = socket(AF_INET, SOCK_STREAM, 0);    
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portNumber);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    while (connect(socketId, (struct sockaddr*) &addr, sizeof(addr)) < 0);
    {
        LOG4CPLUS_ERROR(logger, "Socket initialize error");
    }
    LOG4CPLUS_INFO(logger, "TcpClient initialized");
}

void TcpSocket::acceptSocket()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    socketId = accept(listener, NULL, NULL);
    while (socketId < 0)
    {
        LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
        socketId = accept(listener, NULL, NULL);
    }  
}

void TcpSocket::sendMessage(const UInt8 msg) const
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    while (send(socketId, &msg, sizeof(msg), 0) < 0)
    { 
        LOG4CPLUS_ERROR(logger, "Socket send error");
    }
}

UInt32 TcpSocket::receiveMessage(UInt8 &message) const
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    return recv(socketId, &message, sizeof(UInt8), 0);
}

void TcpSocket::closeSocket()
{
    LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
    close(socketId);
}
