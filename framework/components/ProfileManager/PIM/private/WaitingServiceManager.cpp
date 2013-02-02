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


#include <climits>

#include "WaitingServiceManager.hpp"
#include "EventFactory.hpp"
#include "PmpSessionProtocol.hpp"
#include "ServiceRepository.hpp"
#include "IAmpRequest.hpp"

namespace iviLink
{
namespace PMP
{

WaitingServiceManager::WaitingServiceManager()
    : mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.SessionManager")))
    , mEventFactory(NULL)
    , mSetLocalServices(false)
    , mSetRemoteServices(false)
    , mSessionProtocol(NULL)
    , mServiceRepository(NULL)
    , mStartRequest(false)
    , mAuthenticationDone(false)
    , mAmpRequest(NULL)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
}

WaitingServiceManager::~WaitingServiceManager()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
}

void WaitingServiceManager::setLocalWaitingServices(const std::list<Service::Uid> & services)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

    for (std::list<Service::Uid>::const_iterator it = services.begin();
            services.end() != it; ++it)
    {
        LOG4CPLUS_INFO(mLogger, "service: " + it->value());
    }
    mLocalWaitingServices = services;
    mSetLocalServices = true;
}

void WaitingServiceManager::setRemoteWaitingServices(const std::list<Service::Uid> & services)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

    for (std::list<Service::Uid>::const_iterator it = services.begin();
            services.end() != it; ++it)
    {
        LOG4CPLUS_INFO(mLogger, "service: " + it->value());
    }
    mRemoteWaitingServices = services;
    mSetRemoteServices = true;
}

bool WaitingServiceManager::isLocalWaitingServicesSet() const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    return mSetLocalServices;
}

bool WaitingServiceManager::isRemoteWaitingServicesSet() const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    return mSetRemoteServices;
}

void WaitingServiceManager::init(EventFactory * eventFactory, PmpSessionProtocol * sessionProtocol,
        ServiceRepository * serviceRepository, AppMan::IAmpRequest * ampRequest)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mEventFactory = eventFactory;
    mSessionProtocol = sessionProtocol;
    mServiceRepository = serviceRepository;
    mAmpRequest = ampRequest;
}

void WaitingServiceManager::startWaitingServiceRequest(bool master)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    if (!master)
    {
        LOG4CPLUS_INFO(mLogger, "startWaitingServiceRequest: !master");
        std::list<Service::Uid> pairServices;
        for (std::list<Service::Uid>::iterator it = mLocalWaitingServices.begin();
                mLocalWaitingServices.end() != it; ++it)
        {
            LOG4CPLUS_INFO(mLogger, "local service: " + it->value());
            pairServices.push_back(mServiceRepository->getServiceInfo(*it)->pairUid());
        }
        mSessionProtocol->sendWaitingServicesList(pairServices);
    }
    else // master
    {
        LOG4CPLUS_INFO(mLogger, "startWaitingServiceRequest: master");
        UInt32 minPrio = UINT_MAX;
        Service::Uid service;
        for (std::list<Service::Uid>::iterator it = mLocalWaitingServices.begin();
                mLocalWaitingServices.end() != it; ++it)
        {
            LOG4CPLUS_INFO(mLogger, "local service: " + it->value());
            UInt32 itPrio = mServiceRepository->getServiceInfo(*it)->priority();
            LOG4CPLUS_INFO(mLogger, "local service " + it->value() + " prio " + convertIntegerToString(itPrio));
            if (itPrio < minPrio && 0 != itPrio)
            {
                LOG4CPLUS_INFO(mLogger, "Min local service: " + it->value());
                service = *it;
                minPrio = itPrio;
            }
        }

        for (std::list<Service::Uid>::iterator it = mRemoteWaitingServices.begin();
                mRemoteWaitingServices.end() != it; ++it)
        {
            LOG4CPLUS_INFO(mLogger, "remote service " + it->value());
            UInt32 itPrio = mServiceRepository->getServiceInfo(*it)->priority();
            LOG4CPLUS_INFO(mLogger, "remote service " + it->value() + " prio " + convertIntegerToString(itPrio));
            if (itPrio < minPrio && 0 != itPrio)
            {
                LOG4CPLUS_INFO(mLogger, "Min remote service: " + it->value());
                service = *it;
                minPrio = itPrio;
            }
        }

        mStartRequest = true;
        mServiceToStart = service;
        startWaitingServiceInAppLib();
    }
}

void WaitingServiceManager::authenticationDone()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mAuthenticationDone = true;
    startWaitingServiceInAppLib();
}

void WaitingServiceManager::startWaitingServiceInAppLib()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

    if (mAuthenticationDone && mStartRequest)
    {
        if (mServiceToStart.value() == "")
        {
            LOG4CPLUS_INFO(mLogger, "No waiting services");
        }
        else
        {
            LOG4CPLUS_INFO(mLogger, "START_WAITING_SERVICE " + mServiceToStart.value());
            assert(mAmpRequest);
            mAmpRequest->waitingServiceLoadRequest(mServiceToStart);
        }
    }
    else
    {
        LOG4CPLUS_INFO(mLogger, "Wait for start conditions");
        if (!mAuthenticationDone)
        {
            LOG4CPLUS_INFO(mLogger, "Wait for start conditions: auth not done yet");
        }
        if (!mStartRequest)
        {
            LOG4CPLUS_INFO(mLogger, "Wait for start conditions: no start request yet");
        }
    }
}


} // namespace AMP
} // namespace iviLink
