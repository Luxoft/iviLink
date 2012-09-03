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









#ifndef DEALLOCATE_NOTIFICATION_HPP_
#define DEALLOCATE_NOTIFICATION_HPP_

#include "utils/xml/pugixml.hpp"
#include "Notification.hpp"
#include "Message.hpp"

namespace iviLink {
namespace ChannelSupervisor {
namespace Messages {

class DeallocateNotification : public Notification
{
public:

	DeallocateNotification( const char* tag, const unsigned int channelId = 0 );

	DeallocateNotification(pugi::xml_document* doc);

	virtual ~DeallocateNotification()
	{
	}

	const int GetChannelId() const
	{
		return m_offerredChId;
	}


	virtual const char* GetMessageName() const { return m_notificationTypeMap[NOTIFICATIONTTYPE_DEALLOCATE].c_str(); }


protected:

	int			m_offerredChId;
};

}  // Message
}  // ChannelSupervisor
}  // AXIS

#endif // DEALLOCATE_NOTIFICATION_HPP_