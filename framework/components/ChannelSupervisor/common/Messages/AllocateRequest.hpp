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


#ifndef ALLOCATE_CHANNEL_REQUEST_HPP_
#define ALLOCATE_CHANNEL_REQUEST_HPP_

#include "pugixml.hpp"
#include "Request.hpp"
#include "Message.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

class AllocateRequest: public Request
{
public:

    AllocateRequest(const char* tag, const unsigned int channelId);

    AllocateRequest(pugi::xml_document* doc);

    virtual ~AllocateRequest()
    {
    }

    const std::string& GetTag() const
    {
        return m_tag;
    }

    const int GetChannelId() const
    {
        return m_offerredChId;
    }

    virtual const char* GetMessageName() const
    {
        return m_requestTypes[REQUESTTYPE_ALLOCATE_CHANNEL].c_str();
    }

protected:

    std::string m_tag;
    int m_offerredChId;
};

}  // Messages
}  // ChannelSupervisor
}  // AXIS

#endif // ALLOCATE_CHANNEL_REQUEST_HPP_
