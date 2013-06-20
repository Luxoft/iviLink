/* 
 * 
 * iviLINK SDK, version 1.1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 * 
 */

#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "pugixml.hpp"
#include "MessageWithError.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

class ResponseTypesMap;

class Response: public MessageWithError
{
public:

    enum ResponseType
    {
        RESPONSETYPE_UNKNOWN = 0,
        RESPONSETYPE_ALLOCATE_CHANNEL = 1,
        RESPONSETYPE_DEALLOCATE_CHANNEL,
        RESPONSETYPE_CA_ALLOCATION_DONE,
        RESPONSETYPE_UPDATE_CHANNEL_INFO,
    };

    Response(const char * tag, UInt32 chID, ResponseType responseType);

    Response(pugi::xml_document* doc);

    virtual ~Response()
    {
    }

    static ResponseType ParseResponseType(pugi::xml_document* _doc);

protected:
    ResponseType mResponseType;
    
    static ResponseTypesMap mResponseTypes;
};

class ResponseTypesMap: public std::map<Response::ResponseType, std::string>
{
public:
    ResponseTypesMap()
    {
        (*this)[Response::RESPONSETYPE_ALLOCATE_CHANNEL] = "allocate-channel";
        (*this)[Response::RESPONSETYPE_DEALLOCATE_CHANNEL] = "deallocate-channel";
        (*this)[Response::RESPONSETYPE_CA_ALLOCATION_DONE] = "ca-allocation-done";
        (*this)[Response::RESPONSETYPE_UPDATE_CHANNEL_INFO] = "update-channleInfo";
    }
};

}  // Messages
}  // ChannelSupervisor
}  // AXIS

#endif // RESPONSE_HPP_
