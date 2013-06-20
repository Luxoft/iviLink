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

#ifndef NOTIFICATION_HPP_
#define NOTIFICATION_HPP_

#include "MessageWithError.hpp"
#include "ErrorCode.hpp"
#include <map>

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

class NotificationTypeMap;

class Notification: public MessageWithError
{
public:
	enum NotificationType
	{
		NOTIFICATIONTTYPE_UNKNOWN,
		NOTIFICATIONTTYPE_ALLOCATE,
		NOTIFICATIONTTYPE_CHECK_MAP,
		NOTIFICATIONTTYPE_CA_ALLOCATE,
		NOTIFICATIONTTYPE_MAP_ADD_CID,
		NOTIFICATIONTTYPE_DEALLOCATE,
		NOTIFICATIONTTYPE_UPDATE_CHANNEL_INFO,
        NOTIFICATIONTTYPE_CLIENT_DISCONNECTED,
        NOTIFICATIONTTYPE_INVALIDATE_TIMEDOUT_REQUEST,
	};

	Notification(const char * tag, UInt32 chID, Int32 clientID, UInt32 requestID, NotificationType notificationType);

	Notification(pugi::xml_document* doc);
	virtual ~Notification() {}
	static NotificationType ParseNotificationType(pugi::xml_document* doc);
	static std::string getComplementingTag(const std::string & tag);

    NotificationType getType() const
    {
        return mNotificationType;
    }

    Int32 getClientID() const
    {
        return mClientID;
    }

    UInt32 getRequestID() const
    {
        return mRequestID;
    }

protected:
    static NotificationTypeMap mNotificationTypeMap;

private:
    NotificationType mNotificationType;
    Int32 mClientID;
    UInt32 mRequestID;
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
        (*this)[Notification::NOTIFICATIONTTYPE_CLIENT_DISCONNECTED] = "client-disconnected";
        (*this)[Notification::NOTIFICATIONTTYPE_INVALIDATE_TIMEDOUT_REQUEST] = "request-timed-out";
    }
};

}  // Messages
}  // ChannelSupervisor
}  // AXIS

#endif // NOTIFICATION_HPP_
