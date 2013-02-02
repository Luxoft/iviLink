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


#ifndef COMMANDTIMEOUTRESPONSE_HPP_
#define COMMANDTIMEOUTRESPONSE_HPP_


#include "pugixml.hpp"
#include "Response.hpp"
#include "Request.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

class CommandTimeoutResponse: public Response
{
public:

    CommandTimeoutResponse(const char *requestName);
    CommandTimeoutResponse(pugi::xml_document* doc);

    virtual ~CommandTimeoutResponse()
    {
    }

    virtual const char* GetMessageName() const
    {
        return m_responseTypes[RESPONSETYPE_COMMAND_TIMEOUT].c_str();
    }

    const std::string& GetRequestName() const
    {
        return m_requestName;
    }

protected:
    std::string m_requestName;
};

}  // Messages
}  // ChannelSupervisor
}  // AXIS

#endif /* COMMANDTIMEOUTRESPONSE_HPP_ */
