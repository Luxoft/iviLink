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
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>   
#include <sys/socket.h>
#include <errno.h>

#include "TcpMobileConnectionFinder.hpp"
#include "CTcpCarrierAdapter.hpp"


#define MAX_SOCK_ADDR 128
#define SERVER_CONNECT_TRY_COUNT 10


using namespace iviLink::ConnectivityAgent::HAL;

Logger TcpMobileConnectionFinder::mLogger = Logger::getInstance("ConnectivityAgent.HAL.TcpMobileConnectionFinder");

TcpMobileConnectionFinder::TcpMobileConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender)
:
    CConnectionFinder(handler, gender)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    srandom(time(NULL));
    mBroadcastSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    memcpy(mMessage.handshake, BRD_MESSAGE_TEXT, sizeof(mMessage.handshake));
    
    initSockaddrIn(mBroadcastAddr, INADDR_BROADCAST);
    
    int optval = 1;
    setsockopt(mBroadcastSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    setsockopt(mBroadcastSock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(mBroadcastSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    sockaddr_in servaddr;
    initSockaddrIn(servaddr, INADDR_ANY);
    bind(mBroadcastSock, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr));
}

void TcpMobileConnectionFinder::initSockaddrIn(sockaddr_in& addr, unsigned long s_addr)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(AXIS_TCP_AUTO_CONNECTION_FINDER_BRD_PORT);
    addr.sin_addr.s_addr = htonl(s_addr);
}

void TcpMobileConnectionFinder::performSearch()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mRoundRandNumber = random() % std::numeric_limits<UInt8>::max();
    mMessage.random = mRoundRandNumber;
    LOG4CPLUS_INFO(mLogger, "Current turn random number: " + convertIntegerToString(mRoundRandNumber));
    sendto(mBroadcastSock, &mMessage, sizeof(mMessage), 0, (struct sockaddr *)&mBroadcastAddr, sizeof(mBroadcastAddr));

    char buffer[sizeof(mMessage)];
    tBrdMessage const* const receivedMessage = reinterpret_cast<tBrdMessage const* const>(buffer);
    sockaddr_in cli_addr;
    sockaddr* const p_cli_addr_base = reinterpret_cast<sockaddr*>(&cli_addr);
    socklen_t cli_addr_len = sizeof(cli_addr);
    bool wasLocal = false;
    do {
        wasLocal = false;
        memset(buffer, 0, sizeof(mMessage));
        ssize_t size = recvfrom(mBroadcastSock, buffer, sizeof(buffer), 0, p_cli_addr_base, &cli_addr_len);
        if(size == -1 )
        {
            LOG4CPLUS_ERROR(mLogger, "Error receiving: " + convertIntegerToString(errno) + " : " + strerror(errno));
        }
        else
        {
            if (size != sizeof(tBrdMessage) || strncmp(receivedMessage->handshake, BRD_MESSAGE_TEXT, sizeof(BRD_MESSAGE_TEXT)))
            {
                LOG4CPLUS_WARN(mLogger, "Got incorrect message");
                return;
            }
            sockaddr_in const* sin = reinterpret_cast<sockaddr_in const*>(p_cli_addr_base);
            const size_t addressLength = MAX_SOCK_ADDR;
            char remoteAddrBuffer[MAX_SOCK_ADDR];
            if (inet_ntop(AF_INET, &sin->sin_addr, remoteAddrBuffer, addressLength) != 0)
            {
                std::string remoteAddrStr(remoteAddrBuffer);
                LOG4CPLUS_INFO(mLogger, "Got address: " + remoteAddrStr);
                if(receivedMessage->random != mRoundRandNumber)
                {
                    LOG4CPLUS_INFO(mLogger, "Our number: " + convertIntegerToString(mRoundRandNumber) 
                        + " remote number: " + convertIntegerToString(receivedMessage->random));
                    if (receivedMessage->random < mRoundRandNumber)
                    {
                        LOG4CPLUS_INFO(mLogger, "Will be client");
                        if(!createClientCarrierAdapter(remoteAddrStr)) 
                        {
                            LOG4CPLUS_WARN(mLogger, "Could not connect as a client");
                            this->sleep(FINDER_SLEEP_BETWEEN_RETRIES_MS);
                        }
                        return;
                    }
                    else if (receivedMessage->random > mRoundRandNumber)
                    {
                        LOG4CPLUS_INFO(mLogger, "Will be server");
                        CCarrierAdapter* pSrvAdapter = createServerCarrierAdapter();
                        bool connected = false;
                        for (int i = 0; (i < SERVER_CONNECT_TRY_COUNT) && pSrvAdapter; ++i)
                        {
                            if (!pSrvAdapter->isBroken())
                            {
                                LOG4CPLUS_INFO(mLogger, "Connected as a server!");
                                setReady(pSrvAdapter);
                                pSrvAdapter = NULL;
                                connected = true;
                                break;
                            }
                            else
                            {
                                this->sleep(FINDER_SLEEP_BETWEEN_RETRIES_MS);
                            }
                        }
                        if (!connected)
                        {
                            LOG4CPLUS_WARN(mLogger, "Could not connect as a server");
                            delete pSrvAdapter;
                        }
                    }
                }
                else
                {
                    LOG4CPLUS_INFO(mLogger, "Numbers match, message was local");
                    wasLocal = true;
                }
            }
        }
                
    } while (wasLocal);
}

bool TcpMobileConnectionFinder::createClientCarrierAdapter(std::string const& addr)
{
   LOG4CPLUS_TRACE_METHOD(mLogger, (std::string)__PRETTY_FUNCTION__ + " addr: " + addr);

   CTcpCarrierAdapter* pAdapter = new CTcpCarrierAdapter(createConnectionInfo(false, addr));

   if (!pAdapter->connect().isNoError())
   {
      delete pAdapter;
      return false;
   }
   else
   {
      setReady(pAdapter);
      return true;
   }
}

CCarrierAdapter* TcpMobileConnectionFinder::createServerCarrierAdapter()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    CTcpCarrierAdapter* pAdapter = new CTcpCarrierAdapter(createConnectionInfo(true, "0.0.0.0"));
    if (!pAdapter->connect().isNoError())
    {
        delete pAdapter;
        pAdapter = NULL;
    }
    return pAdapter;
}

CTcpConnectionInfo TcpMobileConnectionFinder::createConnectionInfo(bool isServer, const std::string& host)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, (std::string)__PRETTY_FUNCTION__ + " host: " + host);
    CTcpConnectionInfo info;
    info.serverSocket = isServer;
    info.host = host;
    info.service = std::string(AXIS_PORT);
    return info;
}
