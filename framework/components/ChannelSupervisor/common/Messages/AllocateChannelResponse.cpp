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
#include "AllocateChannelResponse.hpp"

using namespace iviLink::ChannelSupervisor::Messages;

AllocateChannelResponse::AllocateChannelResponse(const char* tag, const unsigned int channelId)
        : Response(RESPONSETYPE_ALLOCATE_CHANNEL)
{
    m_tag = tag;
    AppendCharStringNode(m_messageDocMainNode, "tag", m_tag.c_str());

    m_channelId = channelId;
    AppendIntegerNode(m_messageDocMainNode, "channel-id", m_channelId);
}

AllocateChannelResponse::AllocateChannelResponse(pugi::xml_document* doc)
        : Response(doc)
{
    pugi::xml_attribute nameAttr = m_messageDocMainNode.attribute("name");
    if (nameAttr)
    {
        if (strcmp(nameAttr.value(), GetMessageName()) == 0)
        {
            pugi::xml_node tagNode = m_messageDocMainNode.child("tag");
            if (tagNode)
            {
                const char* tag = tagNode.first_child().value();
                if (tag != NULL && tag[0] != '\0')
                {
                    m_tag = tag;
                }
            }
            pugi::xml_node cidNode = m_messageDocMainNode.child("channel-id");
            if (cidNode)
            {
                const char* cid = cidNode.first_child().value();
                if (cid != NULL && cid[0] != '\0')
                {
                    char *ptr;
                    m_channelId = strtol(cid, &ptr, 10);
                }
            }
        }
    }
}

