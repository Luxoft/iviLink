/* 
 * 
 * iviLINK SDK, version 1.0
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






#include <string>
#include <iostream>
#include <string.h>
#include "utils/xml/pugixml.hpp"
#include "Request.hpp"
#include "Message.hpp"
#include "AllocateChannelRequest.hpp"

using namespace iviLink::ChannelSupervisor::Messages;

AllocateChannelRequest::AllocateChannelRequest(	const char* tag)
	: Request(REQUESTTYPE_ALLOCATE_CHANNEL)
{
	if ( strlen(tag) > 0 )
	{
		m_tag = tag;
		AppendCharStringNode(m_messageDocMainNode, "tag", m_tag.c_str());
	}
}

AllocateChannelRequest::AllocateChannelRequest(pugi::xml_document* doc) :  Request(doc)
{
   pugi::xml_node tagNode = m_messageDocMainNode.child("tag");

   if (tagNode)
   {
      m_tag = tagNode.first_child().value();
   }
}




