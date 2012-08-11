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

#include "samples/linux/Profiles/ProfileAPI/ESeatRequests.hpp"

class IProfileObserver
{
public:

    /**
     * Callback for profile created on the other side;
     * change the current visible seat request
     * @param event the holder of requested seat
     */
    virtual void onTabRequest(EPersonEvent event) = 0;

     /**
      * Callback for profile created on the other side; 
      * change the seat heater request
      * @param event  the holder of requested seat heater
      */
     virtual void onHeaterRequest(EPersonEvent event) = 0;

     /**
      * Callback for profile created on the other side; 
      * change the settings of current visible seat request
      * @param event the direction of movement
      */
     virtual void onMoveRequest(EMoveEvent event) = 0;

     /**
      * Callback for profile created on the other side;
      * initialization state request
      */
     virtual void onInitRequest() = 0;

     virtual ~IProfileObserver() {}

};
#endif // IPROFILE_OBSERVER_HPP
