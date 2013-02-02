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
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "pugixml.hpp"
#include "Response.hpp"

using namespace iviLink::ChannelSupervisor::Messages;

ResponseTypesMap Response::m_responseTypes;

Response::ResponseType Response::GetResponseType(pugi::xml_document* doc)
{
    if (doc)
    {
        pugi::xml_node mainNode = doc->child("message");
        if (mainNode)
        {
            ResponseTypesMap::const_iterator it = m_responseTypes.begin();
            for (; it != m_responseTypes.end(); ++it)
            {
                if (strcmp(mainNode.attribute("name").value(), it->second.c_str()) == 0)
                {
                    return it->first;
                }
            }
        }
    }

    return RESPONSETYPE_UNKNOWN;
}

Response::Response(ResponseType responseType)
        : Message(), m_errorCode(ERRORCODE_SUCCESS), m_errorStr(""), m_responseType(responseType)
{
    if (responseType != RESPONSETYPE_UNKNOWN)
    {
        if (m_messageDocMainNode.append_attribute("name").set_value(
                m_responseTypes[m_responseType].c_str()))
        {
            pugi::xml_attribute typeAttr = m_messageDocMainNode.append_attribute("type");
            if (typeAttr)
            {
                typeAttr.set_value("response");
            }
        }
    }
    m_errorNode = AppendErrorNode(m_messageDocMainNode, ERRORCODE_SUCCESS, "Success");
}

Response::Response(pugi::xml_document* doc)
        : Message(doc)
{
    pugi::xml_attribute typeAttr = m_messageDocMainNode.attribute("type");
    if (typeAttr)
    {
        if (strcmp(typeAttr.value(), "response") == 0)
        {
            m_errorNode = m_messageDocMainNode.child("error");
            if (m_errorNode)
            {
                GetError(m_errorNode, m_errorCode, m_errorStr);
            }
        }
    }
}

pugi::xml_node Response::AppendResultNode(pugi::xml_node resultNode, const char* tag)
{
    pugi::xml_node resNode = resultNode.append_child("res");
    if (resNode)
    {
        pugi::xml_node mNode = resNode.append_child("tag");
        if (mNode)
        {
            mNode.append_child(pugi::node_pcdata).set_value(tag);
        }
    }
    return resNode;
}

pugi::xml_node Response::AppendErrorNode(pugi::xml_node node, ErrorCode errorCode,
        const char* errorString)
{
    pugi::xml_node errorNode = node.append_child("error");
    if (errorNode)
    {
        SetError(errorNode, errorCode, errorString);
    }

    return errorNode;
}

void Response::SetError(pugi::xml_node errorNode, ErrorCode errorCode, const char *errorString)
{
    pugi::xml_attribute errorCodeAttr = errorNode.attribute("code");
    if (!errorCodeAttr)
    {
        errorCodeAttr = errorNode.append_attribute("code");
    }
    if (errorCodeAttr.set_value(errorCode))
    {
        pugi::xml_node errorStringNode = errorNode.first_child();
        if (!errorStringNode)
        {
            errorStringNode = errorNode.append_child(pugi::node_pcdata);
        }
        errorStringNode.set_value(errorString);
    }
}

void Response::GetError(pugi::xml_node errorNode, ErrorCode & errorCode, const char *& errorString)
{
    const char* errorCodeString = errorNode.attribute("code").value();
    char *ptr;
    if (errorCodeString != NULL && errorCodeString[0] != '\0')
    {
        errorCode = (ErrorCode) strtol(errorCodeString, &ptr, 10);
        errorString = errorNode.first_child().value();
    }
}

int Response::ExtractIntegerNode(pugi::xml_node node)
{
    int value = 0;

    const char* valueStr = node.first_child().value();
    if (valueStr != NULL && valueStr[0] != '\0')
    {
        char *ptr;
        value = strtol(valueStr, &ptr, 10);
    }
    return value;
}

char* Response::ExtractCharNode(pugi::xml_node node)
{
    const char * value = node.first_child().value();
    return (char*) value;
}
