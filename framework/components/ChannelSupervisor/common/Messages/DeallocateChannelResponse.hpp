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


#ifndef DEALLOCATECHANNELRESPONSE_HPP_
#define DEALLOCATECHANNELRESPONSE_HPP_

#include "pugixml.hpp"
#include "Response.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

class DeallocateChannelResponse: public Response
{
public:

	DeallocateChannelResponse(const UInt32 channelId);
	DeallocateChannelResponse(pugi::xml_document* doc);

	virtual ~DeallocateChannelResponse()
	{
	}

	virtual const char* GetMessageName() const
	{
		return m_responseTypes[RESPONSETYPE_DEALLOCATE_CHANNEL].c_str();
	}

	const int GetChannelId() const
	{
		return m_offerredChId;
	}

protected:
	int m_offerredChId;
};

}  // Messages
}  // ChannelSupervisor
}  // AXIS

#endif /* DEALLOCATECHANNELRESPONSE_HPP_ */
