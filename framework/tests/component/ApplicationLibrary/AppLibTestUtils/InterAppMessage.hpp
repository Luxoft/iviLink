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


#ifndef INTER_APP_MESSAGE_HPP
#define	INTER_APP_MESSAGE_HPP

//Note! Messages are represented as Enum for simplification. 

/** 
 * InterAppMessage enum
 * contains Messages for inter-test-application communication
 */
enum InterAppMessage
{
    //status messages
    ERROR_MSG = 0,
    TIMEOUT_MSG = 1,
    TEST_FAILED_MSG = 2,
    EVERYTHING_OK = 3,
    TEST_PASSED_MSG = 4,

    //application messages
    APPLICATION_UP_MSG = 100,
    INCOMING_BEFORE_MSG = 101,
    INCOMING_AFTER_MSG = 102,
    SERVICE_DROPPED_MSG = 103,
    UNLOAD_SERVICE_DONE_MSG = 104,

};

#endif	// INTER_APP_MESSAGE_HPP
