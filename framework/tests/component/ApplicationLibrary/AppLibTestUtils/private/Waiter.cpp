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


#include <iostream>
#include <map>
#include <utility>
#include "CSignalSemaphore.hpp" 
#include "Waiter.hpp"
#include "InterAppMessage.hpp"

Logger Waiter::mLogger = Logger::getInstance(LOG4CPLUS_TEXT("AppLibTest.Utils.Waiter"));

Waiter::Waiter(WaiterMode mode)
        : mWaiterMode(mode)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

}

Waiter::~Waiter()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

/**
 * Method adds message being waited to the Waiter
 * @param message - message to be waited
 * @return message being waited in case message received, otherwise returns TIMEOUT_MSG which means timeout
 */
InterAppMessage Waiter::waitMessage(InterAppMessage message, int timeout)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    InterAppMessage msg = ERROR_MSG;

    //cleaning-up the previously stored messages if there are any and adding message to be waited
    mMessageMap.clear();
    mMessageMap.insert(std::make_pair(message, false));  //map because someday it will be possible to wait several messages

    //wait for timeout
    LOG4CPLUS_INFO(mLogger, "start waiting for message " + convertIntegerToString((int) message));
    if (mTimeoutSema.waitTimeout(timeout))
    {
        LOG4CPLUS_WARN(mLogger, "Timeout occurred ");
        msg = TIMEOUT_MSG;
    }
    else
    {
        msg = message;
    }

    LOG4CPLUS_INFO(mLogger, "wait result: " + convertIntegerToString((int) msg));

    return msg;
}

//should be executed from other thread
/**
 * Method checks whether is waiter waits for the message
 * In case waiter is waiting for the message trying in this method call, the waitMessage unlocks and returns.
 * tryMessage shoudl be called from the external thread
 * @param message - message to be waited
 * @return none
 */

void Waiter::tryMessage(InterAppMessage message)
{

    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    LOG4CPLUS_INFO(mLogger, "Try message: " + convertIntegerToString((int) message));

    MessageMap::iterator iterator;

    for (iterator = mMessageMap.begin(); iterator != mMessageMap.end(); ++iterator)
    {
        if ((*iterator).first == message)
        {
            (*iterator).second = true;

            //check all the messages are received
            if (checkWaitComplete())
            {
                LOG4CPLUS_INFO(mLogger, "Signaling Sema");
                mTimeoutSema.signal();
            }
            break;
        }
    }
    return;
}

bool Waiter::checkWaitComplete()
{
    MessageMap::iterator iterator;
    bool ifComplete = true;

    if (mWaiterMode == EITHER_MSG)
    {
        ifComplete = false;
        for (iterator = mMessageMap.begin(); iterator != mMessageMap.end(); ++iterator)
        {
            if ((*iterator).second == true)
            {
                ifComplete = true;
                break;
            }
        }
    }
    else
    {
        for (iterator = mMessageMap.begin(); iterator != mMessageMap.end(); ++iterator)
        {
            if ((*iterator).second == false)
            {
                ifComplete = false;
                break;
            }

        }

        return ifComplete;
    }
}
