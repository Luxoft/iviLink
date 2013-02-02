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


#include <string>
#include <iostream>

#include "AppLibTest9.hpp"

//profile proxy header provides the access to profile API, must be included
#include "AppLibTestProfileProxy.hpp"

#include "Communicator.hpp"
#include "InterAppMessage.hpp"

Logger AppLibTest::mLogger = Logger::getInstance(
        LOG4CPLUS_TEXT("AppLibTest.Tests.Test9.Application"));

AppLibTest::AppLibTest(CSignalSemaphore & semaphore)
        //informing iviLink that this app supports service with this UID
        : Application(iviLink::Service::Uid("AppLibTestService_9")), mAppLibTestCallbacks(
                semaphore), mProfileCallbackSemaphore(&semaphore)
{
    LOG4CPLUS_INFO(mLogger, "APPLIB TEST 9 - STEP 1 CREATE APP ");
}

AppLibTest::~AppLibTest()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
}

void AppLibTest::setCommunicator(Communicator & communicator)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mCommunicator = &communicator;
}

void AppLibTest::init()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    LOG4CPLUS_INFO(mLogger, "APPLIB TEST 9 - STEP 2 initInIVILink ");

    // initialize application in iviLink core
    initInIVILink();

    //registration profile callbacks for given profile API
    registerProfileCallbacks(iviLink::Profile::ApiUid("AppLibTest_PAPI_UID"),
            &mAppLibTestCallbacks);
}

/**
 * Callback that should be invoked by application library thread 
 * after initialization application in application manager 
 */
void AppLibTest::onInitDone(iviLink::ELaunchInfo launcher)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    //if application was launched by user
    if (launcher == iviLink::LAUNCHED_BY_USER)
    {
        LOG4CPLUS_INFO(mLogger, "APPLIB TEST 9 - STEP 3 loadService ");
        //loading service with required UID
        iviLink::Error loadError = loadService(iviLink::Service::Uid("AppLibTestService_9"));
        if (loadError.isNoError())
        {
            LOG4CPLUS_INFO(mLogger, "service started");

            //loading profile proxy which corresponds to given service UID
            AppLibTestProfileProxy appLibTestProfileProxy(
                    iviLink::Service::Uid("AppLibTestService_9"));

            //using method from profile API
            //appLibTestProfileProxy.sendData("SUPERTRUPER");  //keep commented code here for copy-paste purposes
        }
        else
        {
            LOG4CPLUS_ERROR(mLogger, std::string("service NOT started: ") + loadError.toString());
            LOG4CPLUS_INFO(mLogger, "APPLIB TEST 9 FAILED");
            exit(1);
        }
    }

    //if application was launched by other side
    else
    {
        LOG4CPLUS_INFO(mLogger, "started by iviLink");
    }
}

void AppLibTest::onServiceDropped(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mCommunicator->sendMessage(UNLOAD_SERVICE_DONE_MSG);
}

void AppLibTest::unload()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    iviLink::Error unloadError = unloadService(iviLink::Service::Uid("AppLibTestService_9"));
    if (unloadError.isNoError())
    {
        LOG4CPLUS_INFO(mLogger, "service unloaded");
    }
    else
    {
        LOG4CPLUS_ERROR(mLogger, "Unload service error" + unloadError.toString());
        LOG4CPLUS_ERROR(mLogger, "APP LIB TEST 9 - FAILED");
        exit(1);
    }
}
