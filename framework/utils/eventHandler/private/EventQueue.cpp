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


#include "EventQueue.hpp"
#include "CMutex.hpp"

#include <cstddef>
#include <cassert>

namespace iviLink
{
namespace Utils
{

Logger EventQueue::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("utils.EventHandler"));


EventQueue::EventQueue()
    : mQueueMutex(new CMutex())
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mQueueMutex);
}

EventQueue::~EventQueue()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    while (!mQueue.empty())
    {
        delete mQueue.front();
        mQueue.pop();
    }

    delete mQueueMutex;
}

IEvent * EventQueue::pop()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    IEvent * event = NULL;
    assert(mQueueMutex);
    mQueueMutex->lock();
    if (!mQueue.empty())
    {
        event = mQueue.front();
        mQueue.pop();
    }
    mQueueMutex->unlock();
    return event;
}

void EventQueue::push(IEvent * const event)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mQueueMutex);
    mQueueMutex->lock();
    mQueue.push(event);
    mQueueMutex->unlock();
}

} /* namespace Utils */
} /* namespace iviLink */
