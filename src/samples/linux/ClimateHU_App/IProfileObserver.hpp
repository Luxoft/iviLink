/* 
 * 
 * iviLINK SDK, version 1.0.1
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
 * 
 */






#ifndef IPROFILE_OBSERVER_HPP
#define IPROFILE_OBSERVER_HPP

#include "samples/linux/Profiles/ProfileAPI/EClimateRequests.hpp"

class IProfileObserver
{
public:

     /**
      * Callback for profile created on the other side; 
      * change temperature settings request
      * @param event specify the temperature event
      */
     virtual void onTempRequest(ETempEvent event) = 0;

     /**
      * Callback for profile created on the other side; 
      * change fan setting request
      * @param event specify the fan event
      */
     virtual void onFanRequest(EFanEvent event) = 0;

     /**
      * Callback for profile created on the other side; 
      * change heater settings request
      * @param event specify the heater event
      */
     virtual void onHeaterRequest(EHeaterEvent event) = 0;

     /**
      * Callback for profile created on the other side; 
      * change general ettings request
      * @param event kind of settings
      */
     virtual void onGeneralRequest(EGeneralRequest event) = 0;

     /**
      * Callback for profile created on the other side;
      * initialization state request
      */
     virtual void onInitRequest() = 0;
};
#endif // IPROFILE_OBSERVER_HPP
