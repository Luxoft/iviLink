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


#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <ostream>
#include "pugixml.hpp"
#include "Types.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

class Message
{
public:

    Message();

    Message(pugi::xml_document* doc);

    virtual ~Message()
    {
        if (m_messageDoc)
        {
            delete m_messageDoc;
            m_messageDoc = 0;
        }
    }
    
    virtual const char* GetMessageName() const = 0;

    virtual void Write(std::ostream& stream);

    virtual void Write(pugi::xml_writer& writer);

    pugi::xml_document* GetMessageDoc()
    {
        return m_messageDoc;
    }

    static pugi::xml_node AppendIntegerNode(pugi::xml_node node, const char* name, int value,
            const char* format = "%d");

    static pugi::xml_node AppendCharStringNode(pugi::xml_node node, const char* name,
            const char* value);

protected:

    pugi::xml_document* m_messageDoc;
    pugi::xml_node m_messageDocMainNode;

};

}  // Messages
}  // ChannelSupervisor
}  // iviLink

#endif // MESSAGE_HPP
