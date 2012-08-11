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





#ifndef CSEATHUPROFILECALLBACKS_HPP
#define CSEATHUPROFILECALLBACKS_HPP

#include <iostream>
#include <stdio.h>

#include "utils/misc/Logger.hpp"
//Seat HU Profile API header
#include "samples/linux/Profiles/ProfileAPI/ISeatHUProfile.hpp"
#include "IProfileObserver.hpp"

class CSeatHUProfileCallbacks
        :
        public ISeatHUProfile::Callbacks
{
public:

        explicit CSeatHUProfileCallbacks(IProfileObserver* pObserver):pOwner(pObserver){}

        virtual void handleError(CError const & error)
        {
            std::cout << static_cast<std::string>(error) << std::endl;
        }


        //From ISeatClientProfile::Callbacks

        /**
         * Callback for profile created on the other side;
         * change the current visible seat request
         * @param event the holder of requested seat
         */
        virtual void onTabRequest(EPersonEvent event)
        {
            pOwner->onTabRequest(event);
        }

        /**
         * Callback for profile created on the other side; 
         * change the seat heater request
         * @param event  the holder of requested seat heater
         */
        virtual void onHeaterRequest(EPersonEvent event)
        {
            pOwner->onHeaterRequest(event);
        }

        /**
         * Callback for profile created on the other side; 
         * change the settings of current visible seat request
         * @param event the direction of movement
         */
        virtual void onMoveRequest(EMoveEvent event)
        {
            pOwner->onMoveRequest(event);
        }

        /**
         * Callback for profile created on the other side;
         * initialization state request
         */
        virtual void onInitRequest()
        {
            pOwner->onInitRequest();
        }

private:
        IProfileObserver* pOwner;
};

#endif // CSEATHUPROFILECALLBACKS_HPP
