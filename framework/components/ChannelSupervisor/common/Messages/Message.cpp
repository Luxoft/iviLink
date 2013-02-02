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


#include <cstdio>
#include <iostream>
#include "pugixml.hpp"

#include "Message.hpp"

#define PROTOCOL_VERSION "1.0"
#define PROTOCOL_XMLNS "http://ivilink.com/message/"

using namespace iviLink::ChannelSupervisor::Messages;

Message::Message()
{
    m_messageDoc = new pugi::xml_document;
    m_messageDoc->reset();

    m_messageDocMainNode = m_messageDoc->append_child("message");
    if (m_messageDocMainNode)
    {
        pugi::xml_attribute versionAttr = m_messageDocMainNode.append_attribute("version");
        if (versionAttr)
        {
            if (versionAttr.set_value(PROTOCOL_VERSION))
            {
                pugi::xml_attribute xmlnsAttr = m_messageDocMainNode.append_attribute("xmlns");
                if (xmlnsAttr)
                {
                    xmlnsAttr.set_value(PROTOCOL_XMLNS);
                }
            }
        }
    }
}

Message::Message(pugi::xml_document* doc)
{
    if (!doc)
    {
        ;
    }
    m_messageDoc = doc;

    m_messageDocMainNode = doc->child("message");
    if (m_messageDocMainNode)
    {
        pugi::xml_attribute versionAttr = m_messageDocMainNode.attribute("version");
        if (versionAttr)
        {
            versionAttr.set_value(PROTOCOL_VERSION);
        }
    }
}

void Message::Write(std::ostream& stream)
{
    m_messageDoc->save(stream);
}

void Message::Write(pugi::xml_writer& writer)
{
    m_messageDoc->save(writer);
}

pugi::xml_node Message::AppendIntegerNode(pugi::xml_node node, const char *name, int value,
        const char *format)
{
    char valueStr[30];
    sprintf(valueStr, format, value);
    pugi::xml_node intNode = node.append_child(name);
    if (intNode)
    {
        intNode.append_child(pugi::node_pcdata).set_value(valueStr);
    }
    return intNode;
}

pugi::xml_node Message::AppendCharStringNode(pugi::xml_node node, const char * name,
        const char * value)
{
    pugi::xml_node charNode = node.append_child(name);
    if (charNode)
    {
        if (value != NULL && value[0] != '\0')
        {
            charNode.append_child(pugi::node_pcdata).set_value(value);
        }
    }
    return charNode;
}
