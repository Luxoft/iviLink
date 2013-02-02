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


#ifndef AMPREQUESTS_HPP_
#define AMPREQUESTS_HPP_

#include "IAmpHandler.hpp"
#include "Event.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace PMP
{

class PmpComponentManager;
class PmpPim;

class AmpConnectionLost : public Utils::Event
{
public:
    AmpConnectionLost(PmpComponentManager * manager);

private:
    virtual void handleEvent();

    PmpComponentManager * mManager;
    Logger mLogger;
};

class AmpAppRequestError : public Utils::Event
{
public:
    AmpAppRequestError(PmpPim * pim, iviLink::Service::SessionUid session);

private:
    virtual void handleEvent();

    PmpPim * mPim;
    Service::SessionUid mSessionUid;
    Logger mLogger;
};

} /* namespace PMP */
} /* namespace iviLink */
#endif /* AMPREQUESTS_HPP_ */
