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







#ifndef ALLOCATECHANNELREQUEST_HPP_
#define ALLOCATECHANNELREQUEST_HPP_


#include "utils/xml/pugixml.hpp"
#include "Request.hpp"
#include "Message.hpp"

namespace iviLink {
namespace ChannelSupervisor {
namespace Messages {

class AllocateChannelRequest: public Request
{
public:

	AllocateChannelRequest( const char* tag );

	AllocateChannelRequest(pugi::xml_document* doc);

	virtual ~AllocateChannelRequest()
	{
	}

	const std::string& GetTag() const
	{
		return m_tag;
	}

	virtual const char* GetMessageName() const { return m_requestTypes[REQUESTTYPE_ALLOCATE_CHANNEL].c_str(); }

protected:

	std::string m_tag;
};

}  // Message
}  // ChannelSupervisor
}  // AXIS

#endif /* ALLOCATECHANNELREQUEST_HPP_ */
