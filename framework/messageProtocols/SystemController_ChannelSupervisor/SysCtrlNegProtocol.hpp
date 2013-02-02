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


#ifndef SC_NEGOTIATOR_MESSAGE_HPP_
#define SC_NEGOTIATOR_MESSAGE_HPP_


/**
 * Messages from SystemController to ChannelSupervisor
 */
enum SystemControllerToNegotiator
{
	SC_CS_SHUTDOWN = 1
};

/**
 * Messages from ChannelSupervisor to SystemController
 */
enum NegotiatorToSystemController
{
    CS_SC_CONNECTION_ESTABLISHED = 1,
    CS_SC_CONNECTION_LOST
 };

#endif /* MESSAGE_HPP_ */
