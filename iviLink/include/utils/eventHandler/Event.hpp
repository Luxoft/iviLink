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
 * \file Event.hpp
 * This file contains Event abstract class and IEvent interface
 */


#ifndef REQUESTBASE_HPP_
#define REQUESTBASE_HPP_

namespace iviLink
{
namespace Utils
{


/**
* \class IEvent
* This class provides base event interface
*/
class IEvent
{
public:

    /**
     * Pure virtual function for inheritors to implement events' task
     **/
    virtual void handleEvent() = 0;

    /**
     * Is used to add timeout event if needed to process anything after timeout
     */
    virtual IEvent * popTimeoutEvent() = 0;

    /**
     * Returns time of timeout in ms
     */
    virtual int getTimeout() = 0;

    /**
     * Virtual destructor
     */
    virtual ~IEvent();
};


/**
* \class Event
* This class provides class that implements base Event functions and realizes IEvent interface
*/
class Event : public IEvent
{
public:

    /**
     * Constructor
     */
    Event();

    /**
     * Virtual destructor
     */
    virtual ~Event();

    /**
	 * Returns time of timeout in ms
	 */
    virtual int getTimeout();

protected:

    /**
     * Is used to insert timeout event
     * @param timeoutEvent is pointer on Event inheritor
     */
    void pushTimeoutEvent(Event * const timeoutEvent);

    /**
     * Is used to add timeout event if needed to process anything after timeout
     */
    virtual IEvent * popTimeoutEvent();

private:

    Event * mTimeoutEvent; ///< Pointer of inheritor of Event class
};


} /* namespace Utils */
} /* namespace iviLink */
#endif /* REQUESTBASE_HPP_ */
