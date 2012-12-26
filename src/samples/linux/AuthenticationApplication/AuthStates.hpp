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
 

#ifndef __AUTH_STATES_HPP
#define __AUTH_STATES_HPP
namespace authentication {

enum LocalStates {
   INITIAL,                  // initial state
   WAITING_ANY_PIN,          // after that slave/master roles will be decided
   WAITING_REMOTE_PIN,       // is master
   WAITING_LOCAL_PIN,        // is slave
   WAITING_PIN_CONFIRMATION, // is slave and waiting ok from master
   WAITING_REMOTE_DEATH,   // is master and sent slave his ok and waiting for service dropped callback
   SUCCESS,                  // tell System Controller about success
};

enum ExternalStates {
   //ext_AUTH_REQ,
   //ext_AUTH_NREQ,
   ext_AUTH_OK, // confirmation on success
   ext_NA,
};

} // namespace Authentication

#endif //__AUTH_STATES_HPP
