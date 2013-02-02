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


#include <string>
#include <iostream>
#include <string.h>
#include "pugixml.hpp"
#include "Request.hpp"
#include "Message.hpp"
#include "DeallocateRequest.hpp"

using namespace iviLink::ChannelSupervisor::Messages;

DeallocateRequest::DeallocateRequest(const char* tag, const unsigned int channelId)
        : Request(REQUESTTYPE_DEALLOCATE_CHANNEL)
{
    if (channelId != 0 && strlen(tag) > 0)
    {
        m_tag = tag;
        AppendCharStringNode(m_messageDocMainNode, "tag", m_tag.c_str());
        m_offerredChId = channelId;
        AppendIntegerNode(m_messageDocMainNode, "channel-id", m_offerredChId);
    }
}

DeallocateRequest::DeallocateRequest(pugi::xml_document* doc)
        : Request(doc)
{
    pugi::xml_node channelIdNode = m_messageDocMainNode.child("channel-id");
    if (channelIdNode)
    {
        pugi::xml_node tagNode = m_messageDocMainNode.child("tag");

        if (tagNode)
        {
            m_tag = tagNode.first_child().value();
        }
    }
}

