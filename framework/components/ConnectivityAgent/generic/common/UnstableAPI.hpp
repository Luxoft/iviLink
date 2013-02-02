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


#ifndef CONNECTIVITY_AGENT_UnstableAPI_HPP
#define CONNECTIVITY_AGENT_UnstableAPI_HPP

#include "Types.hpp"
#include "PublicError.hpp"

#include <string>

class ConnectionInformation
{
private:
    std::string strLocalAddr;
    std::string strRemoteAddr;
    std::string strConnectionType;
public:

    ConnectionInformation() : strLocalAddr("n/a"), strRemoteAddr("n/a"), strConnectionType("n/a") {}

    ConnectionInformation(const char * localAddr, const char * remoteAddr, const char * connectionType);

    std::string getLocalAddress();

    std::string getRemoteAddress();

    std::string getConnectionType();

    UInt8 * serialize();

    void deserialize(UInt8 * data);

    UInt32 getSerializedSize();
};

/**
 * @return ERROR_NOT_FOUND if there is no connection
 * ERROR_REQUEST_FAILED if could not reach ConnectivityAgent via IPC
 * IVILINK_NO_ERROR in case of success
 */
iviLink::Error getConnectionInformation(ConnectionInformation* data);

#endif
