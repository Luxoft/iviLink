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

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <cassert>
#include <string>
#include "Notification.hpp"
#include "NegotiatorConstants.hpp"
#include "XmlHelper.hpp"

#define CLIENT_ID_XML_TAG "client-id"
#define REQUEST_ID_XML_TAG "request-id"

using namespace iviLink::ChannelSupervisor::Messages;

NotificationTypeMap Notification::mNotificationTypeMap;

Notification::NotificationType Notification::ParseNotificationType(pugi::xml_document* doc)
{
    std::string name = XmlHelper::getMessageNameFromDoc(doc);
    if (name.empty())
    {
        return NOTIFICATIONTTYPE_UNKNOWN;
    }
    else
    {
        NotificationTypeMap::const_iterator it = mNotificationTypeMap.begin();
        for (; it != mNotificationTypeMap.end(); ++it)
        {
            if (name == it->second)
            {
                return it->first;
            }
        }
    }
    return NOTIFICATIONTTYPE_UNKNOWN;
}

std::string Notification::getComplementingTag(std::string const& tag)
{
    size_t separatorPos = tag.find_last_of(TAG_SUFFIX_SEPARATOR);
    if (separatorPos == std::string::npos)
    {
        return tag;
    }
    else
    {
        std::string result = tag.substr(0, separatorPos);
        std::string suffix = tag.substr(separatorPos + 1, tag.size() - separatorPos - 1);
        if (suffix.compare(TAG_SERVER_SUFFIX) == 0)
        {
            suffix = TAG_CLIENT_SUFFIX;
        }
        else if (suffix.compare(TAG_CLIENT_SUFFIX) == 0)
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

Notification::Notification(const char * tag, UInt32 channelID, Int32 clientID, UInt32 reqID, NotificationType notificationType)
    : MessageWithError(tag, channelID)
    , mNotificationType(notificationType)
    , mClientID(clientID)
    , mRequestID(reqID)
{
    if (mNotificationType != NOTIFICATIONTTYPE_UNKNOWN)
    {
        XmlHelper::appendName(mMessageDocMainNode, mNotificationTypeMap[mNotificationType].c_str());
    }
    XmlHelper::appendIntegerNode(mMessageDocMainNode, CLIENT_ID_XML_TAG, mClientID);
    XmlHelper::appendIntegerNode(mMessageDocMainNode, REQUEST_ID_XML_TAG, mRequestID);
    XmlHelper::appendType(mMessageDocMainNode, "notification");
}

Notification::Notification(pugi::xml_document* doc)
    : MessageWithError(doc)
    , mNotificationType(ParseNotificationType(doc))
    , mClientID(0)
    , mRequestID(0)
{
    pugi::xml_node clientNode = mMessageDocMainNode.child(CLIENT_ID_XML_TAG);
    if (clientNode)
    {
        mClientID = XmlHelper::extractIntegerFromNode(clientNode);
    }
    pugi::xml_node requestNode = mMessageDocMainNode.child(REQUEST_ID_XML_TAG);
    if (requestNode)
    {
        mRequestID = XmlHelper::extractIntegerFromNode(requestNode);
    }
}
