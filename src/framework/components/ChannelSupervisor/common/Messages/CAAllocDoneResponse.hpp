/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#ifndef CAALLOCATEDONERESPONSE_HPP_
#define CAALLOCATEDONERESPONSE_HPP_

#include "pugixml.hpp"
#include "Response.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

class CAAllocateDoneResponse: public Response
{
public:

    CAAllocateDoneResponse(const char *tag, const unsigned int channelId);
    CAAllocateDoneResponse(pugi::xml_document* doc);

    virtual ~CAAllocateDoneResponse()
    {
    }

    virtual const char* GetMessageName() const
    {
        return m_responseTypes[RESPONSETYPE_ALLOCATE_CHANNEL].c_str();
    }

    const std::string& GetTag() const
    {
        return m_tag;
    }

    const int GetChannelId() const
    {
        return m_channelId;
    }

protected:

protected:

    std::string m_tag;
    int m_channelId;
};

}  // Messages
}  // ChannelSupervisor
}  // AXIS

#endif /* CAALLOCATEDONERESPONSE_HPP_ */
