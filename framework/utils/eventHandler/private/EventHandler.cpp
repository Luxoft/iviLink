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


#include "EventHandler.hpp"
#include "EventQueue.hpp"
#include "TimeoutManager.hpp"

#include <cstddef>
#include <cassert>

namespace iviLink
{
namespace Utils
{

Logger EventHandler::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("utils.EventHandler"));

EventHandler::EventHandler()
    : CThread("EventHandler", true)
    , mStartSemaphore (new CSignalSemaphore())
    , mEventSemaphore(new CSignalSemaphore())
    , mEventQueue(new EventQueue())
    , mExceptionQueue(new EventQueue())
    , mTimeoutManager(new TimeoutManager())
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mStartSemaphore);
    assert(mEventSemaphore);
    assert(mEventQueue);
    assert(mExceptionQueue);
    assert(mTimeoutManager);
}

EventHandler::~EventHandler()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    if (mTimeoutManager)
    {
        mTimeoutManager->finish();
    }

    delete mStartSemaphore;
    delete mEventQueue;
    delete mExceptionQueue;
    delete mEventSemaphore;
    delete mTimeoutManager;
}

void EventHandler::init()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    start();
    assert(mStartSemaphore);
    mStartSemaphore->wait();
}

void EventHandler::deinit()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    stop();
    assert(mEventSemaphore);
    mEventSemaphore->signal();
}

void EventHandler::threadFunc()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mStartSemaphore);
    mStartSemaphore->signal();
    do
    {
        assert(mEventQueue);
        assert(mExceptionQueue);
        if (IEvent * event = mExceptionQueue->pop())
        {
            handleEvent(event);
        }
        else if (IEvent * event = mEventQueue->pop())
        {
            handleEvent(event);
        }
        else
        {
            LOG4CPLUS_INFO(msLogger, "thread func before wait");
            mEventSemaphore->wait();
            LOG4CPLUS_INFO(msLogger, "thread func after wait");
        }
    }
    while (!getStopFlag());
}

void EventHandler::handleEvent(IEvent * event)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(event);
    event->handleEvent();
    IEvent * timeoutEvent = event->popTimeoutEvent();
    if (timeoutEvent)
    {
        addTimeoutEvent(timeoutEvent, event->getTimeout());
    }
    delete event;
}

void EventHandler::pushEvent(Event * const event)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    pushEvent(static_cast<IEvent* const>(event));
}

void EventHandler::pushException(Event * const exception)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    IEvent * event = static_cast<IEvent*>(exception);
    if (event)
    {
        assert(mEventSemaphore);
        assert(mExceptionQueue);
        mExceptionQueue->push(event);
        mEventSemaphore->signal();
    }
}

void EventHandler::pushEvent(IEvent * const event)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    if (event)
    {
        assert(mEventSemaphore);
        assert(mEventQueue);
        mEventQueue->push(event);
        mEventSemaphore->signal();
    }
}

void EventHandler::addTimeoutEvent(IEvent * const timeoutEvent, int timeoutMs)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mTimeoutManager);

    /**
     * This class is used to contain pointer on Event inheritor for timeout manager
     */
    class EventTimeout : public ITimeoutSubscriber
    {
    public:
        EventTimeout(EventHandler * handler, IEvent * event)
            : mHandler(handler)
            , mEvent(event)
        {
            assert(mHandler);
            assert(mEvent);
        }

        ~EventTimeout()
        {
            delete mEvent;
        }

    private:
        virtual void onTimeout()
        {
            assert(mHandler);
            mHandler->pushEvent(mEvent);
            mEvent = 0;
        }

        EventHandler * mHandler;
        IEvent * mEvent;
    };


    if (timeoutEvent)
    {
        mTimeoutManager->addSubscriber(new EventTimeout(this,timeoutEvent), timeoutMs);
    }
}

} /* namespace Utils */
} /* namespace iviLink */
