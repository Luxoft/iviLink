/* 
 * 
 * iviLINK SDK, version 1.0
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



#ifndef CCLIMATECLIENTPROFILECALLBACKS_HPP
#define CCLIMATECLIENTPROFILECALLBACKS_HPP

#include "samples/linux/Profiles/ProfileAPI/IClimateClientProfile.hpp"
#include "IProfileObserver.hpp"

class CClimateClientProfileCallbacks
        :
        public IClimateClientProfile::Callbacks
{
public:

    explicit CClimateClientProfileCallbacks(IProfileObserver* pObserver):pOwner(pObserver){}

    virtual void handleError(CError const &)
    {
    }

    //From ISeatClientProfile::Callbacks
    /**
     * Callback for profile created on the other side; notification about conditioner state
     * @param condState the state of conditioner
     */
    virtual void incomingNotification(Json::Value condState)
    {
        pOwner->incomingNotification(condState);
    }

private:

    IProfileObserver* pOwner;
};

#endif // CCLIMATECLIENTPROFILECALLBACKS_HPP
