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

#include <cstring>
#include <iostream>
#include "Request.hpp"
#include "XmlHelper.hpp"

using namespace iviLink::ChannelSupervisor::Messages;

RequestTypesMap Request::mRequestTypes;

Request::RequestType Request::ParseRequestType(pugi::xml_document* doc)
{
    std::string name = XmlHelper::getMessageNameFromDoc(doc);
    if (name.empty())
    {
        return REQUESTTYPE_UNKNOWN;
    }
    else
    {
        RequestTypesMap::const_iterator it = mRequestTypes.begin();
        for (; it != mRequestTypes.end(); ++it)
        {
            if (name == it->second)
            {
                return it->first;
            }
        }
    }
    return REQUESTTYPE_UNKNOWN;
}

Request::Request(const char * tag, UInt32 chID, RequestType requestType)
    : Message(tag, chID)
    , mRequestType(requestType)
{
    if (mRequestType != REQUESTTYPE_UNKNOWN)
    {
        XmlHelper::appendName(mMessageDocMainNode, mRequestTypes[requestType].c_str());
    }
    XmlHelper::appendType(mMessageDocMainNode, "request");
}

Request::Request(pugi::xml_document* doc)
    : Message(doc)
    , mRequestType(ParseRequestType(doc))
{
}
