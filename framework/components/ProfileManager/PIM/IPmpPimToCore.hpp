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


#ifndef IPMPPIMTOCORE_HPP_
#define IPMPPIMTOCORE_HPP_

#include "UID.hpp"

namespace iviLink
{
namespace PMP
{

class IPmpPimToCore
{
public:
    /**
    * @param profileId is UID of profile to check
    * @retval true if system has any instance of Profile
    * @retval false otherwise
    */
    virtual bool hasInstance(Profile::Uid const& id) = 0;

    /**
    * Unloads all instances of target Profile
    * @param profileId is UID of profile to unload
    */
    virtual void unloadInstances(Profile::Uid const& id) = 0;

protected:

    virtual ~IPmpPimToCore()
    {
    }

};
}
}

#endif /* IPMPPIMTOCORE_HPP_ */
