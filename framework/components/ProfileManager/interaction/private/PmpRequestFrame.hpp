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


#ifndef PMPREQUESTFRAME_HPP_
#define PMPREQUESTFRAME_HPP_

#include "Types.hpp"

namespace iviLink
{
namespace PMP
{

typedef UInt32 MessageId;

enum ClientId
{
    PMP_CLIENT_CORE = 0,
    PMP_CLIENT_PI_SRV,
    PMP_CLIENT_CMP_MGR,
    PMP_CLIENT_SESSION,
};

enum RequestType
{
    PMP_REQ_CORE_GET_COMPLEMENTS_REQUEST = 0,
    PMP_REQ_CORE_GET_COMPLEMENTS_RESPONSE,
    PMP_REQ_CORE_RELOAD_PROFS_FROM_REPO,
    PMP_REQ_CORE_RESET_PROF_STATE,
    PMP_REQ_CORE_LOCK,
    PMP_REQ_CORE_UNLOCK,
    PMP_REQ_CORE_LOCK_ALL,
    PMP_REQ_CORE_UNLOCK_ALL,
    PMP_REQ_CORE_DIS_BY_CLIENT,
    PMP_REQ_CORE_ENABLE_BY_CLNT,
    PMP_REQ_CORE_ENABLE_BY_CLNT_ALL,

    PMP_REQ_PIM_CREATE_PROFILE,
    PMP_REQ_PIM_PROFILE_DIED,

    PMP_REQ_CMP_MGR_READY,
    PMP_REQ_SESSION_NEGOTIATE_NUMBER,
    PMP_REQ_SESSION_SEND_WAITING_SERVICES,
};

struct PMPFrame
{
    MessageId id;             //> is used in response map
    ClientId client;          //> PMP Core, PIM etc
    RequestType reqType;      //> function number in client
    UInt32 size;               //> size of frame (includes all PMPFrame parts)
    UInt8 data[];              //> request/response data (function arguments, return value etc)
} __attribute__((__packed__));

}
}

#endif /* PMPREQUESTFRAME_HPP_ */
