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


#ifndef SESSIONMANAGER_HPP_
#define SESSIONMANAGER_HPP_

#include <list>

#include "UID.hpp"
#include "Logger.hpp"

namespace iviLink
{

namespace AppMan
{
class IAmpRequest;
}
namespace PMP
{

class EventFactory;
class PmpSessionProtocol;
class ServiceRepository;

class WaitingServiceManager
{
public:
    WaitingServiceManager();
    virtual ~WaitingServiceManager();

    void setLocalWaitingServices(const std::list<Service::Uid> & services);
    void setRemoteWaitingServices(const std::list<Service::Uid> & services);

    bool isLocalWaitingServicesSet() const;
    bool isRemoteWaitingServicesSet() const;

    void init(EventFactory * eventFactory, PmpSessionProtocol * sessionProtocol,
            ServiceRepository * serviceRepository, AppMan::IAmpRequest * ampRequest);

    void startWaitingServiceRequest(bool master);
    void authenticationDone();
private:
    void startWaitingServiceInAppLib();

    Logger mLogger;
    EventFactory * mEventFactory;
    std::list<Service::Uid> mLocalWaitingServices;
    std::list<Service::Uid> mRemoteWaitingServices;
    bool mSetLocalServices;
    bool mSetRemoteServices;
    PmpSessionProtocol * mSessionProtocol;
    ServiceRepository * mServiceRepository;
    Service::Uid mServiceToStart;
    bool mStartRequest;
    bool mAuthenticationDone;
    AppMan::IAmpRequest * mAmpRequest;
};

}
}

#endif /* SESSIONMANAGER_HPP_ */
