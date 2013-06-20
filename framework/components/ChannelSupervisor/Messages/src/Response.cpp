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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "pugixml.hpp"
#include "Response.hpp"
#include "XmlHelper.hpp"

using namespace iviLink::ChannelSupervisor::Messages;

ResponseTypesMap Response::mResponseTypes;

Response::ResponseType Response::ParseResponseType(pugi::xml_document* doc)
{
    std::string name = XmlHelper::getMessageNameFromDoc(doc);
    if (name.empty())
    {
        return RESPONSETYPE_UNKNOWN;
    }
    else
    {
        ResponseTypesMap::const_iterator it = mResponseTypes.begin();
        for (; it != mResponseTypes.end(); ++it)
        {
            if (name == it->second)
            {
                return it->first;
            }
        }
    }
    return RESPONSETYPE_UNKNOWN;
}

Response::Response(const char * tag, UInt32 chID, ResponseType responseType)
    : MessageWithError(tag, chID)
    , mResponseType(responseType)
{
    if (responseType != RESPONSETYPE_UNKNOWN)
    {
        XmlHelper::appendName(mMessageDocMainNode, mResponseTypes[mResponseType].c_str());
    }
    XmlHelper::appendType(mMessageDocMainNode, "response");
}

Response::Response(pugi::xml_document* doc)
    : MessageWithError(doc)
    , mResponseType(ParseResponseType(doc))
{
}
