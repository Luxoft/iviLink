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


#include "AmpRequests.hpp"
#include "PmpComponentManager.hpp"
#include "PmpPim.hpp"

namespace iviLink
{
namespace PMP
{

AmpConnectionLost::AmpConnectionLost(PmpComponentManager * manager)
    : mManager(manager)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void AmpConnectionLost::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mManager->onAmpConnectionLost();
}

AmpAppRequestError::AmpAppRequestError(PmpPim * pim, iviLink::Service::SessionUid session)
    : mPim(pim)
    , mSessionUid(session)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void AmpAppRequestError::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mPim->appRequestError(mSessionUid);
}

} /* namespace PMP */
} /* namespace iviLink */
