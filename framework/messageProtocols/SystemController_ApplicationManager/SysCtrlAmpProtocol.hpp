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


#ifndef SC_APPMAN_MESSAGE_HPP__
#define SC_APPMAN_MESSAGE_HPP__


#include "Types.hpp"

/**
 * Messages from SystemController to AppMan
 */
enum eSCtoAMMessages
{
    SC_AM_SHUTDOWN = 1,
    SC_AM_LINK_UP_NOTIFY,
    SC_AM_LINK_DOWN_NOTIFY,
};

/**
 * Messages from AppManager to SystemController
 */
enum eAMtoSCMessages
{
   AM_SC_CONNECTION_ESTABLISHED = 1,
   AM_SC_CONNECTION_LOST,
   AM_SC_GET_LINK_STATE,  // request, should return true if link is up false otherwise
};

#endif // SC_APPMAN_MESSAGE_HPP__
