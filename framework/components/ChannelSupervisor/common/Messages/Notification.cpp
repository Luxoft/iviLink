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


#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include "Notification.hpp"
#include "Common.hpp"

using namespace iviLink::ChannelSupervisor::Messages;

NotificationTypeMap Notification::m_notificationTypeMap;

Notification::NotificationType Notification::GetNotificationType(pugi::xml_document* doc)
{
    if (doc)
    {
        pugi::xml_node mainNode = doc->child("message");
        if (mainNode)
        {
            NotificationTypeMap::const_iterator it = m_notificationTypeMap.begin();
            for (; it != m_notificationTypeMap.end(); ++it)
            {
                if (strcmp(mainNode.attribute("name").value(), it->second.c_str()) == 0)
                    return it->first;
            }
        }
    }

    return NOTIFICATIONTTYPE_UNKNOWN;
}

const char * Notification::GetNotificationTag(pugi::xml_document* doc)
{
    if (doc)
    {
        pugi::xml_node mainNode = doc->child("message");
        if (mainNode)
        {
            pugi::xml_node tagNode = mainNode.child("tag");

            if (tagNode)
            {
                return tagNode.first_child().value();
            }
        }
    }
    return NULL;
}


std::string Notification::GetComplementingTag(std::string const& tag)
{
   size_t separatorPos = tag.find_last_of(TAG_SUFFIX_SEPARATOR);
   if( separatorPos == std::string::npos)
   {
      return tag;
   } 
   else 
   {
      std::string result = tag.substr(0, separatorPos);
      std::string suffix = tag.substr(separatorPos + 1, tag.size() - separatorPos - 1);
      if(suffix.compare(TAG_SERVER_SUFFIX) == 0) 
      {
         suffix = TAG_CLIENT_SUFFIX;
      } 
      else if(suffix.compare(TAG_CLIENT_SUFFIX) == 0) 
      {
         suffix = TAG_SERVER_SUFFIX;
      } 
      else
      {
         return tag;
      }
      result.append(TAG_SUFFIX_SEPARATOR).append(suffix);
      return result;
   }
}

std::string& Notification::GetNotificationTag()
{
    return m_tag;
}

pugi::xml_node Notification::AppendResultNode(pugi::xml_node resultNode, const char* tag)
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

pugi::xml_node Notification::AppendErrorNode(pugi::xml_node node, ErrorCode errorCode,
        const char* errorString)
{
    pugi::xml_node errorNode = node.append_child("error");
    if (errorNode)
    {
        SetError(errorNode, errorCode, errorString);
    }

    return errorNode;
}

void Notification::SetError(pugi::xml_node errorNode, ErrorCode errorCode, const char *errorString)
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

void Notification::GetError(pugi::xml_node errorNode, ErrorCode & errorCode,
        const char *& errorString)
{
    const char* errorCodeString = errorNode.attribute("code").value();
    char *ptr;
    if (errorCodeString != NULL && errorCodeString[0] != '\0')
    {
        errorCode = (ErrorCode) strtol(errorCodeString, &ptr, 10);
        errorString = errorNode.first_child().value();
    }
}

int Notification::ExtractIntegerNode(pugi::xml_node node)
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

char* Notification::ExtractCharNode(pugi::xml_node node)
{
    const char * value = node.first_child().value();
    // TODO const cast
    return (char*) value;
}

Notification::Notification(const char * tag, UInt32 channelID, NotificationType notificationType)
        : Message(), m_errorCode(ERRORCODE_SUCCESS), m_errorStr(""), m_notificationType(
                notificationType), m_tag(tag), mChannelID(channelID)
{    
    if (m_notificationType != NOTIFICATIONTTYPE_UNKNOWN)
    {
        if (m_messageDocMainNode.append_attribute("name").set_value(
                m_notificationTypeMap[m_notificationType].c_str()))
        {
            pugi::xml_attribute typeAttr = m_messageDocMainNode.append_attribute("type");
            if (typeAttr)
            {
                typeAttr.set_value("notification");
            }
        }
        if (tag != 0)
        {
            AppendCharStringNode(m_messageDocMainNode, "tag", m_tag.c_str());
        }
    }
    m_errorNode = AppendErrorNode(m_messageDocMainNode, ERRORCODE_SUCCESS, "Success");
    AppendIntegerNode(m_messageDocMainNode, "channel-id", mChannelID);
}

Notification::Notification(pugi::xml_document* doc, NotificationType notificationType)
        : Message(doc)
{
    pugi::xml_attribute typeAttr = m_messageDocMainNode.attribute("type");
    if (typeAttr)
    {
        if (strcmp(typeAttr.value(), "notification") == 0)
        {
            if (notificationType != NOTIFICATIONTTYPE_UNKNOWN)
            {
                //pugi::xml_attribute queryName = m_messageDocMainNode.attribute("name");
            }
            pugi::xml_node tagNode = m_messageDocMainNode.child("tag");

            if (tagNode)
            {
                m_tag = tagNode.first_child().value();
            }

            m_errorNode = m_messageDocMainNode.child("error");
            if (m_errorNode)
            {
                GetError(m_errorNode, m_errorCode, m_errorStr);
            }

            
        }
    }
    pugi::xml_node cidNode = m_messageDocMainNode.child("channel-id");
    if (cidNode)
    {
        const char* cid = cidNode.first_child().value();
        if (cid != NULL && cid[0] != '\0')
        {
            char *ptr;
            mChannelID = strtol(cid, &ptr, 10);
        }
    }
}
