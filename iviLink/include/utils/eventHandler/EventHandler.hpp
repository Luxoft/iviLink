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


/**
 * \file EventHandler.hpp
 * This file contains EventHandler class and represents its interface
 */


#ifndef REQUESTHANDLER_HPP_
#define REQUESTHANDLER_HPP_


#include "CThread.hpp"
#include "Event.hpp"
#include "Logger.hpp"

/**
 * Forward declaration
 */
class CSignalSemaphore;

namespace iviLink
{
namespace Utils
{

/**
 * Forward declaration
 */
class EventQueue;
class TimeoutManager;

/**
* \class EventHandler
* This class provides utility for event processing by one thread
*/
class EventHandler : protected CThread
{
public:

    EventHandler();

    virtual ~EventHandler();

    /**
     * Is used to make initialization of Event Handler. Must be called before calling other
     * functions of Event Handler
     */
    void init();

    /**
     * Makes de-initialization of Event Handler. Must be called before deleting
     */
    void deinit();

    /**
     * Adds event to the queue of events that wait processing
     * @param event is pointer on inheritor of Event class
     */
    void pushEvent(Event * const event);

    /**
     * Adds event into exception queue
     */
    void pushException(Event * const event);

private:

    /**
     * Adds event to the queue of events that wait processing
     * @param event is pointer on inheritor of Event class
     */
    void pushEvent(IEvent * const event);

    /**
     * Not implemented
     */
    EventHandler(const EventHandler&);

    /**
     * Not implemented
     */
    EventHandler& operator = (const EventHandler&);

    /**
     * Main function of event handlers' thread
     */
    virtual void threadFunc();

    /**
     * This function handles one event and deletes it after handling
     * @param event is pointer on event handler
     */
    void handleEvent(IEvent * event);

    /*
     * Added event into timeout manager
     * @param timeoutEvent is pointer on Event inheritor
     */
    void addTimeoutEvent(IEvent * const timeoutEvent, int timeoutMs);

    CSignalSemaphore * mStartSemaphore; ///< Semaphore is used for block on initializing
    CSignalSemaphore * mEventSemaphore; ///< Semaphore is used for blocking if no events in queue
    EventQueue *       mEventQueue;     ///< Event queue. Events wait for processing
    EventQueue *       mExceptionQueue; ///< Exception queue. Events wait for prioritized processing
    TimeoutManager *   mTimeoutManager; ///< This timeout manager is used for timeout events

    static Logger msLogger; ///< object of logger
};


} /* namespace Utils */
} /* namespace iviLink */
#endif /* REQUESTHANDLER_HPP_ */
