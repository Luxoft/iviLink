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


#ifndef SOCKETUTILS_HPP_
#define SOCKETUTILS_HPP_

#include <sys/socket.h>

namespace iviLink {
namespace ConnectivityAgent {
namespace HAL {

/**
 * Converts socket address structure to string presentation with the help of
 * appropriate function.
 *
 * @param sa - pointer to socket address structure
 * @param salen - length of address structure
 *
 * @return pointer to string representation of address (string allocated statically).
 */
const char * sockNtoPHost(sockaddr const* sa, socklen_t salen);

/**
 * Reentrant version of function sockNtoPHost(). It will alloc buffer for result
 * instead of using statically allocated buffer.
 *
 * @param sa - pointer to socket address structure
 * @param salen - length of address structure
 *
 * @return pointer to string representation of address. Buffer for string is 
 * allocated using @c new and must be freed using delete[]
 */
const char * sockNtoPHost_r(sockaddr const* sa, socklen_t salen);

}  // namespace HAL
}  // namespace ConnectivityAgent
}  // namespace AXIS


#endif /* SOCKETUTILS_HPP_ */
