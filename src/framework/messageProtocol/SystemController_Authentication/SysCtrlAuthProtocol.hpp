/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#ifndef SC_AUTHENTICATION_MESSAGE_HPP_
#define SC_AUTHENTICATION_MESSAGE_HPP_

#include "Types.hpp"

/**
 * Messages from Authentication Application to SystemController
 */
enum AuthenticationToSystemController
{
   AA_SC_AUTHENTICATION_OK = 1,
   AA_SC_AUTHENTICATION_NOK,
   AA_SC_AUTHENTICATION_CANCELED
};

enum SystemControllerToAuthentication
{
    SC_AA_DIE,
};


struct AuthenticationMessage
{
   UInt16 requestType;
} __attribute__((__packed__));

#endif /* MESSAGE_HPP_ */
