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


#ifndef CONNECTIONINFO_HPP_
#define CONNECTIONINFO_HPP_

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <string>

namespace iviLink
{
namespace ConnectivityAgent
{
namespace HAL
{
    /// CTcpConnectionInfo struct
    /**
    * This structure is used to transfer connection information from CConnectivityAgent to CTcpCarrierAdapter
    * @see CTcpCarrierAdapter
    */
    struct CTcpConnectionInfo
    {
        /// Server or client side flag
        bool serverSocket;

        /// Host address
        std::string host;

        /// Network port
        std::string service;
    };

    struct CBluetoothInfo
    {
        bool isServer;
        #ifndef ANDROID
        /// Remote BT device address
        char *remoteBDAddr;

        /// Connection socket
        int sock;

        /// Local BT device adapter ID
        int adapterId;       
        #endif // ANDROID
    };
} // HAL
} // ConnectivityAgent
} // iviLink


#endif /* CONNECTIONINFO_HPP_ */
