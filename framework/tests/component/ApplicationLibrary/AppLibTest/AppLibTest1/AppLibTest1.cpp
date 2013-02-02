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

#include "AppLibTest1.hpp"

//profile proxy header provides the access to profile API, must be included
#include "AppLibTestProfileProxy.hpp"

AppLibTest::AppLibTest(CSignalSemaphore & semaphore)
        //informing iviLink that this app supports service with this UID
        : Application(iviLink::Service::Uid("AppLibTestService_1")), mAppLibTestCallbacks(
                semaphore), mProfileCallbackSemaphore(&semaphore)
{
}

AppLibTest::~AppLibTest()
{

}

void AppLibTest::init()
{
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

    //if application was launched by user
    if (iviLink::LAUNCHED_BY_USER == launcher)
    {
        //loading service with required UID
        iviLink::Error loadError = loadService(iviLink::Service::Uid("AppLibTestService_1"));
        if (loadError.isNoError())
        {
            //loading profile proxy which corresponds to given service UID
            AppLibTestProfileProxy appLibTestProfileProxy(
                    iviLink::Service::Uid("AppLibTestService_1"));

            //using method from profile API
            appLibTestProfileProxy.sendData("SUPERTRUPER");
        }
        else
        {
            mProfileCallbackSemaphore->signal();
        }
    }

    //if application was launched by other side
    else
    {
        ;
    }
}
