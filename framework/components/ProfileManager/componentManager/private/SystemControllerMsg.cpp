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


#include <cstdlib>
#include <cassert>
#include <unistd.h>

#include "SystemControllerMsg.hpp"
#include "PmpComponentManager.hpp"
#include "EventFactory.hpp"
#include "Exit.hpp"

static const char moduleName[] = "SystemControllerMsg";

namespace iviLink
{

namespace PMP
{

SystemControllerMsg::SystemControllerMsg()
    : SystemControllerMsgProxy("SysCtrl_PrflMngr")
    , mEventFactory(NULL)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.SystemControllerMsg")))
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

    bool noError = false;
    for (int i = 0; i < 5; ++i)
    {
        LOG4CPLUS_INFO(mLogger, "Try to connect to System Controller number " + convertIntegerToString(i));
        if (connect().isNoError())
        {
            noError = true;
            break;
        }
        usleep(250000);
    }

    if (!noError)
    {
        LOG4CPLUS_FATAL(mLogger, "Can't connect to system controller");
        killProcess(1);
    }
}

SystemControllerMsg::~SystemControllerMsg()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
}

BaseError SystemControllerMsg::onShutDown()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);
    mEventFactory->scShutDown();
    return BaseError::NoError("","");
}

BaseError SystemControllerMsg::onUnlockAuthenticationProfile()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);
    mEventFactory->scUnlockAuthenticationProfile();
    return BaseError::NoError(moduleName);
}

BaseError SystemControllerMsg::onUnlockProfiles()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    assert(mEventFactory);
    mEventFactory->scUnlockProfiles();
    return BaseError::NoError(moduleName);
}

BaseError SystemControllerMsg::onAuthenticationDone()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mEventFactory->authenticationDone();
    return BaseError::NoError(moduleName);
}

void SystemControllerMsg::init(EventFactory * eventFactory)
{
    mEventFactory = eventFactory;
}

}
}
