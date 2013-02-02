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


#ifndef REQUEST_HPP_
#define REQUEST_HPP_

#include "Message.hpp"
#include <map>

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

class RequestTypesMap;

class Request: public Message
{
public:

    enum RequestType
    {
        REQUESTTYPE_UNKNOWN = 0,
        REQUESTTYPE_ALLOCATE_CHANNEL = 1,
        REQUESTTYPE_CA_ALLOCATION_DONE,
        REQUESTTYPE_DEALLOCATE_CHANNEL,
        REQUESTTYPE_UPDATE_CHANNEL_INFO
    };

    Request(RequestType requestType = REQUESTTYPE_UNKNOWN);

    Request(pugi::xml_document* doc, RequestType requestType = REQUESTTYPE_UNKNOWN);

    virtual ~Request()
    {
    }

    static RequestType GetRequestType(pugi::xml_document* doc);

protected:

    RequestType m_requestType;
    static RequestTypesMap m_requestTypes;
};

class RequestTypesMap: public std::map<Request::RequestType, std::string>
{
public:
    RequestTypesMap()
    {
        (*this)[Request::REQUESTTYPE_ALLOCATE_CHANNEL] = "allocate-channel";
        (*this)[Request::REQUESTTYPE_DEALLOCATE_CHANNEL] = "deallocate-channel";
        (*this)[Request::REQUESTTYPE_CA_ALLOCATION_DONE] = "ca-allocation-done";
        (*this)[Request::REQUESTTYPE_UPDATE_CHANNEL_INFO] = "update-channelInfo";
    }
};

}  // Messages
}  // ChannelSupervisor
}  // iviLink

#endif // REQUEST_HPP_
