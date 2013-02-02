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


#ifndef NOTIFICATION_HPP_
#define NOTIFICATION_HPP_

#include "Message.hpp"
#include "ErrorCode.hpp"
#include <map>

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

class NotificationTypeMap;

class Notification: public Message
{
public:

	//TODO: not sure all the types are used
	enum NotificationType
	{
		NOTIFICATIONTTYPE_UNKNOWN,
		NOTIFICATIONTTYPE_ALLOCATE,
		NOTIFICATIONTTYPE_CHECK_MAP,
		NOTIFICATIONTTYPE_CA_ALLOCATE,
		NOTIFICATIONTTYPE_MAP_ADD_CID,
		NOTIFICATIONTTYPE_DEALLOCATE,
		NOTIFICATIONTTYPE_UPDATE_CHANNEL_INFO,
	};

	Notification(const char * tag = NULL, UInt32 chID = 0, NotificationType notificationType = NOTIFICATIONTTYPE_UNKNOWN);

	Notification(pugi::xml_document* doc, NotificationType notificationType = NOTIFICATIONTTYPE_UNKNOWN);

	virtual ~Notification()
	{
	}

	static NotificationType 	GetNotificationType(pugi::xml_document* doc);
	static const char *			GetNotificationTag(pugi::xml_document* doc);

	static std::string 			GetComplementingTag(const std::string & tag);

	std::string& 				GetNotificationTag();


	void SetError(ErrorCode errorCode, const char* errorString)
	{
		SetError(m_errorNode, errorCode, errorString);
	}

	void GetError(ErrorCode& errorCode, const char*& errorString)
	{
		GetError(m_errorNode, errorCode, errorString);
	}

    UInt32 GetChannelId() const
    {
        return mChannelID;
    }

protected:

    static pugi::xml_node AppendResultNode(pugi::xml_node resultsNode, const char* modelId);
    static pugi::xml_node AppendErrorNode(pugi::xml_node node, ErrorCode errorCode,
            const char* errorString);

    static void SetError(pugi::xml_node errorNode, ErrorCode errorCode, const char* errorString);
    static void GetError(pugi::xml_node errorNode, ErrorCode& errorCode, const char*& errorString);

    static int ExtractIntegerNode(pugi::xml_node node);
    static char* ExtractCharNode(pugi::xml_node node);

    pugi::xml_node m_errorNode;

    ErrorCode m_errorCode;
    const char *m_errorStr;

    NotificationType m_notificationType;
    static NotificationTypeMap m_notificationTypeMap;
    std::string m_tag;

    UInt32 mChannelID;
};

class NotificationTypeMap: public std::map<Notification::NotificationType, std::string>
{
public:
    NotificationTypeMap()
    {
        (*this)[Notification::NOTIFICATIONTTYPE_ALLOCATE] = "allocate-channel";
        (*this)[Notification::NOTIFICATIONTTYPE_CHECK_MAP] = "check-the-map";
        (*this)[Notification::NOTIFICATIONTTYPE_CA_ALLOCATE] = "allocate-in-connectivity-agent";
        (*this)[Notification::NOTIFICATIONTTYPE_MAP_ADD_CID] = "add-cid-in-map";
        (*this)[Notification::NOTIFICATIONTTYPE_DEALLOCATE] = "deallocate-channel";
        (*this)[Notification::NOTIFICATIONTTYPE_UPDATE_CHANNEL_INFO] = "update-channelInfo";
    }
};

}  // Messages
}  // ChannelSupervisor
}  // AXIS

#endif // NOTIFICATION_HPP_
