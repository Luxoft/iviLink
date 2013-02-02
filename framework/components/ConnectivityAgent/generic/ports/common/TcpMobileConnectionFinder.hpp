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
#ifndef TCP_MOBILE_CONNECTION_FINDER
#define TCP_MOBILE_CONNECTION_FINDER

#include <sys/socket.h>
#include <arpa/inet.h>

#include "CConnectionFinder.hpp"
#include "TcpFinderCommon.hpp"
#include "ConnectionInfo.hpp"

namespace iviLink
{
namespace ConnectivityAgent
{
namespace HAL
{
/**
 *  A simplified version of CTcpAutoConnectionFinder - does not use netlink sockets.
 */
class TcpMobileConnectionFinder : public CConnectionFinder
{
private:
    static Logger mLogger;

public:
    TcpMobileConnectionFinder(IFoundConnectionHandler& handler, EGenderType gender);
    virtual ~TcpMobileConnectionFinder() {}

protected:
    virtual void performSearch();

private:
    bool createClientCarrierAdapter(std::string const& addr);
    CCarrierAdapter*  createServerCarrierAdapter();

    void initSockaddrIn(sockaddr_in& addr, unsigned long s_addr);

    CTcpConnectionInfo createConnectionInfo(bool isServer, const std::string& host);
private:
    int mBroadcastSock;
    tBrdMessage mMessage;
    sockaddr_in mBroadcastAddr;
    UInt8 mRoundRandNumber;
};

}
}
}

#endif //TCP_MOBILE_CONNECTION_FINDER
