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


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include "Response.hpp"
#include "CommandRejectResponse.hpp"

using namespace iviLink::ChannelSupervisor::Messages;

CommandRejectResponse::CommandRejectResponse(const char* requestName)
        : Response(RESPONSETYPE_COMMAND_REJECT)
{
    m_requestName = requestName;
    AppendCharStringNode(m_messageDocMainNode, "request-name", m_requestName.c_str());
}

CommandRejectResponse::CommandRejectResponse(pugi::xml_document* doc)
        : Response(doc)
{
    pugi::xml_attribute nameAttr = m_messageDocMainNode.attribute("name");
    if (nameAttr)
    {
        if (strcmp(nameAttr.value(), GetMessageName()) == 0)
        {
            pugi::xml_node reqNameNode = m_messageDocMainNode.child("request-name");
            if (reqNameNode)
            {
                const char* requestName = reqNameNode.first_child().value();
                if (requestName != NULL && requestName[0] != '\0')
                {
                    m_requestName = requestName;
                }
            }
        }
    }
}

