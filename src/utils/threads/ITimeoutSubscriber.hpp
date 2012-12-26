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
 

#ifndef ITIMEOUTSUBSCRIBER_HPP_
#define ITIMEOUTSUBSCRIBER_HPP_

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "Types.hpp"

/// ITimeoutSubscriber interface
/**
 * CTimeoutManager uses this interface to subscribe on timeout events and to call needed function
 */

class ITimeoutSubscriber
{
public:
   // Methods section

   /**
    * Called by CTimeout manager after time has passed.
    */
   virtual void onTimeout() = 0;

   virtual ~ITimeoutSubscriber() {}

private:
   

};

#endif /* ITIMEOUTSUBSCRIBER_HPP_ */
