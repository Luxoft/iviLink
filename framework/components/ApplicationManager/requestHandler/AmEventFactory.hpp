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


#ifndef AMEVENTFACTORY_HPP_
#define AMEVENTFACTORY_HPP_

#include <list>
#include <UID.hpp>

namespace iviLink
{

namespace Utils
{
class EventHandler;
}

namespace AppMan
{
class IPmpHandler;
}

namespace AMP
{

class AmEventFactory
{
public:
    AmEventFactory();
    virtual ~AmEventFactory();

    void init(Utils::EventHandler * eventHandler, AppMan::IPmpHandler * ipcServer);

    void waitingServicesRequest(const std::list<Service::Uid> & services);

private:
    Utils::EventHandler * mEventHandler;
    AppMan::IPmpHandler * mPmpIpcServer;
};

} /* namespace AMP */
} /* namespace iviLink */
#endif /* AMEVENTFACTORY_HPP_ */
