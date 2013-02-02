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


#ifndef COMMON_H_
#define COMMON_H_

#include <string>

#include "Types.hpp"

#define TAG_SERVER_SUFFIX "SERVER"
#define TAG_CLIENT_SUFFIX "CLIENT"
#define TAG_SUFFIX_SEPARATOR "#"

namespace iviLink
{
namespace ChannelSupervisor
{

// NegotiatorIPCHandler will wait no longer than this timeout for response from the other side
const UInt32 OTHER_SIDE_RESP_TIMEOUT_MS = 5000;

const UInt32 WD_SLEEP_TIMEOUT_MS = 20000;
const UInt32 DEALLOCATE_TIMEOUT_MS = 30000;
const UInt32 KICK_TIMEOUT_MS = 10000;

enum CS_ERROR
{
    CS_SUCCESS = 0,
    CS_ERR_OTHER = 1000
};

struct UID
{
    int uid;

    UID()
            : uid(0)
    {
    }

    UID(int a)
    {
        uid = a;
    }
};

struct ChannelId
{
    unsigned int caChannelId;

    ChannelId()
            : caChannelId(0)
    {
    }
};
}
}

#endif /* COMMON_H_ */
