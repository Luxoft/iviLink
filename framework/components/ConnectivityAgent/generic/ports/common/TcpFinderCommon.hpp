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
#ifndef TCP_FINDER_COMMON
#define TCP_FINDER_COMMON

/// Text used in broadcast messages for basic verification
#define BRD_MESSAGE_TEXT "IVILINK?"

#define FINDER_SLEEP_BETWEEN_RETRIES_MS 300

namespace iviLink
{
namespace ConnectivityAgent
{
namespace HAL
{
    enum eMessageResult {
        eSERVER,
        eCLIENT,
        eERROR
    };
    
    /// Type for broadcast message data
    struct tBrdMessage
    {
        char handshake[sizeof(BRD_MESSAGE_TEXT)];   /// used to perform handshake
        UInt8 random;                               /// used to hold random number
    } __attribute__((__packed__));
}
}
}

#endif //TCP_FINDER_COMMON
