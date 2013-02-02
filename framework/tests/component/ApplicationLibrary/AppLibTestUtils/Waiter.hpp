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


#ifndef WAITER_HPP
#define	WAITER_HPP

#include <map>
#include "CSignalSemaphore.hpp"
#include "Logger.hpp"
#include "InterAppMessage.hpp"

//InterAppMessage - message itself
//bool - set to true if message received, otherwise false
typedef std::map<InterAppMessage, bool> MessageMap;

/** 
 * WaiterMode enum
 * specifies mode of the waiter
 */
enum WaiterMode
{
    SINGLE_MSG,		//waiter waites for 1 single message
    EITHER_MSG,		//waiter waites until either message received -  NOT SUPPORTED
    ALL_MSG			//waiter waiter until all the required messages received -  NOT SUPPORTED
};

/**
 * Waiter object is intended for keeping the message(s) it is waiting for
 */
class Waiter
{

public:
    explicit Waiter(WaiterMode mode);
    virtual ~Waiter();

    /**
     * Method adds message being waited to the Waiter
     * @param message - message to be waited
     * @return message being waited in case message received, otherwise returns TIMEOUT_MSG which means timeout
     */
    InterAppMessage waitMessage(InterAppMessage message, int timeout);

    /**
     * Method checks whether is waiter waits for the message
     * In case waiter is waiting for the message trying in this method call, the waitMessage unlocks and returns.
     * tryMessage shoudl be called from the external thread
     * @param message - message to be waited
     * @return none
     */
    void tryMessage(InterAppMessage message);

private:
    MessageMap mMessageMap;
    CSignalSemaphore mTimeoutSema;
    WaiterMode mWaiterMode;

    static Logger mLogger;

    bool checkWaitComplete();

};

#endif	/* WAITER_HPP */

