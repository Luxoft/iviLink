/* 
 * 
 * iviLINK SDK, version 1.1.2
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

    Message(const char * tag, UInt32 channelId);

    Message(pugi::xml_document* doc);

    virtual ~Message();

    virtual std::string toString();

    std::string getTag() const
    {
        return mTag;
    }

    UInt32 getChannelID() const
    {
        return mChannelID;
    }

protected:
    pugi::xml_document* getMessageDoc()
    {
        return mMessageDoc;
    }
    pugi::xml_document* mMessageDoc;
    pugi::xml_node mMessageDocMainNode;

    std::string mTag;
    UInt32 mChannelID;
};

}  // Messages
}  // ChannelSupervisor
}  // iviLink

#endif // MESSAGE_HPP
