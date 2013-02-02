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


#include <cstddef>

#include "AmEventFactory.hpp"
#include "Event.hpp"
#include "IPmpHandler.hpp"
#include "EventHandler.hpp"

namespace iviLink
{
namespace AMP
{

AmEventFactory::AmEventFactory()
    : mEventHandler(NULL)
    , mPmpIpcServer(NULL)
{
}

AmEventFactory::~AmEventFactory()
{
}

void AmEventFactory::init(Utils::EventHandler * eventHandler, AppMan::IPmpHandler * ipcServer)
{
    mEventHandler = eventHandler;
    mPmpIpcServer = ipcServer;
}

void AmEventFactory::waitingServicesRequest(const std::list<Service::Uid> & services)
{
    class WaitingServicesRequest : public Utils::Event
    {
        std::list<Service::Uid> mServices;
        AppMan::IPmpHandler * mPmpHandler;

    public:
        WaitingServicesRequest(const std::list<Service::Uid> & services,
                AppMan::IPmpHandler * pmpHandler)
            : mServices(services)
            , mPmpHandler(pmpHandler)
        {
        }

        virtual void handleEvent()
        {
            mPmpHandler->waitingServicesRequest(mServices);
        }
    };

    mEventHandler->pushEvent(new WaitingServicesRequest(services, mPmpIpcServer));
}

} /* namespace AMP */
} /* namespace iviLink */
