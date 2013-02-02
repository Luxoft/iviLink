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


#ifndef QUEUE_HPP
#define	QUEUE_HPP

#include <iostream>
#include <queue>
#include "stdlib.h"
#include "CMutex.hpp"
#include "InterAppMessage.hpp"

#define MAXQUEUESIZE 10

/**
 * MessageQueue class provides implementation of the queue with ability to add and remove messages from the queue
 */
class MessageQueue
{
public:
    MessageQueue()
            : mQueueSize(0)
    {
        ;
    }

    /**
     * Method getting q size
     * @param none
     * @return size of the q
     */
    int queueSize()
    {
        return mQueueSize;
    }

    /**
     * Method adding message to the queue
     * @param InterAppMessage msg - message to be added
     * @return true if message successfully added, otherwise false
     */
    bool enqueue(InterAppMessage msg)
    {
        bool retval = false;

        mQueueMutex.lock();
        if (mMessageQueue.size() < MAXQUEUESIZE)
        {
            mMessageQueue.push(msg);
            mQueueSize++;
            retval = true;
        }
        else
        {
            ;
        }
        mQueueMutex.unlock();

        return retval;
    }

    /**
     * Method dequeueing the message from the q
     * @param none
     * @return InterAppMessage - message from the q
     */
    InterAppMessage dequeue()
    {
        InterAppMessage msg = (InterAppMessage) 0;

        mQueueMutex.lock();
        if (mMessageQueue.size() > 0)
        {

            msg = mMessageQueue.front();  //take the message
            mMessageQueue.pop();  //remove from the q
            mQueueSize--;
        }
        mQueueMutex.unlock();
        return msg;
    }

private:
    std::queue<InterAppMessage> mMessageQueue;  //messages
    int mQueueSize;     //q size
    CMutex mQueueMutex;    //q mutex
};

#endif	/* QUEUE_HPP */

