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


#include <cstring>
#include <iostream>
#include "Request.hpp"

using namespace iviLink::ChannelSupervisor::Messages;

RequestTypesMap Request::m_requestTypes;

Request::RequestType Request::GetRequestType(pugi::xml_document* doc)
{
    if (doc)
    {
        pugi::xml_node mainNode = doc->child("message");
        if (mainNode)
        {
            RequestTypesMap::const_iterator it = m_requestTypes.begin();
            for (; it != m_requestTypes.end(); ++it)
            {
                if (strcmp(mainNode.attribute("name").value(), it->second.c_str()) == 0)
                    return it->first;
            }
        }
    }

    return REQUESTTYPE_UNKNOWN;
}

Request::Request(RequestType requestType)
        : Message(), m_requestType(requestType)
{
    if (m_requestType != REQUESTTYPE_UNKNOWN)
    {
        if (m_messageDocMainNode.append_attribute("name").set_value(
                m_requestTypes[requestType].c_str()))
        {
            pugi::xml_attribute typeAttr = m_messageDocMainNode.append_attribute("type");
            if (typeAttr)
            {
                typeAttr.set_value("request");
            }
        }
    }
}

Request::Request(pugi::xml_document* doc, RequestType requestType)
        : Message(doc)
{
    pugi::xml_attribute typeAttr = m_messageDocMainNode.attribute("type");
    if (typeAttr)
    {
        if (strcmp(typeAttr.value(), "request") == 0)
        {
            if (requestType != REQUESTTYPE_UNKNOWN)
            {
                //pugi::xml_attribute queryName = m_messageDocMainNode.attribute("name");
            }
        }
    }
}
