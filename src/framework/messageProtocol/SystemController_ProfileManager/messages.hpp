/* 
 * 
 * iviLINK SDK, version 1.0.1
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









#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_


#include "utils/misc/Types.hpp"

/**
 * Messages from SystemController to ProfileManager
 */
enum eSCtoCSMessages
{
   SC_PM_SHUTDOWN = 1,
   SC_PM_UNLOCK_AUTHENTICATION_PROFILE,
   SC_PM_UNLOCK_PROFILES
};

/**
 * Messages from ProfileManager to SystemController
 */
enum ePMtoSCMessages
{
   PM_SC_CONNECTION_ESTABLISHED = 1,
   PM_SC_CONNECTION_LOST
};

struct MessageHeader
{
   UInt16 type;
   UInt32 size;
} __attribute__((__packed__));

struct Message
{
   MessageHeader header;
   UInt8 data[0];
} __attribute__((__packed__));


#endif /* MESSAGE_HPP_ */
