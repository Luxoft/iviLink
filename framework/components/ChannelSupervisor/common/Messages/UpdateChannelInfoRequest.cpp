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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string.h>
#include "pugixml.hpp"
#include "Request.hpp"
#include "Message.hpp"
#include "UpdateChannelInfoRequest.hpp"

using namespace iviLink::ChannelSupervisor::Messages;

UpdateChannelInfoRequest::UpdateChannelInfoRequest(const char* tag, const UInt32 channelID)
        : Request(REQUESTTYPE_UPDATE_CHANNEL_INFO)
{
    if (strlen(tag) > 0)
    {
        mTag = tag;
        AppendCharStringNode(m_messageDocMainNode, "tag", mTag.c_str());

        mChannelId = channelID;
        AppendIntegerNode(m_messageDocMainNode, "channel-id", mChannelId);
    }
}

UpdateChannelInfoRequest::UpdateChannelInfoRequest(pugi::xml_document* doc)
        : Request(doc)
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
                    mTag = tag;
                }
            }
            pugi::xml_node cidNode = m_messageDocMainNode.child("channel-id");
            if (cidNode)
            {
                const char* cid = cidNode.first_child().value();
                if (cid != NULL && cid[0] != '\0')
                {
                    char *ptr;
                    mChannelId = strtol(cid, &ptr, 10);
                }
            }
        }
    }
}

