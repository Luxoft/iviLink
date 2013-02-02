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


#ifndef APPLIB_TEST_1_HPP
#define APPLIB_TEST_1_HPP

#include <string>
#include <iostream>

//CApp header, must be included for interaction with apllication manager
#include "Application.hpp"

//api profile header, must be included for implementing callbacks
#include "IAppLibTestProfileAPI.hpp"

#include "CSignalSemaphore.hpp"

// AppLibTest class inherits Application class
class AppLibTest: public iviLink::Application
{
    // callbacks
    class AppLibTestProfileCallbacks: public IAppLibTestProfile_API::Callbacks
    {

    public:

        std::string getData()
        {
            return mReceivedData;
        }

        explicit AppLibTestProfileCallbacks(CSignalSemaphore & semaphore)
                : mProfileCallbackSemaphore(&semaphore)
        {
            ;
        }

        virtual void handleError(BaseError const & error)
        {
            ;
        }

        virtual void dataReceived(std::string data)
        {
            mReceivedData = data;
            mProfileCallbackSemaphore->signal();
        }

    private:
        std::string mReceivedData;
        CSignalSemaphore *mProfileCallbackSemaphore;
    };

public:
    AppLibTest(CSignalSemaphore & semaphore);
    virtual ~AppLibTest();

    /**
     * Initializes application in iviLink core
     */
    void init();

    /**
     * Callback that should be invoked by application library thread 
     * after initialization application in application manager 
     * @param launcher shows the initiator of application start
     * @return none
     */
    virtual void onInitDone(iviLink::ELaunchInfo launcher);

private:

    //instance of class implementing callbacks
    AppLibTestProfileCallbacks mAppLibTestCallbacks;
    IAppLibTestProfile_API *mAppLibTestAPI;
    CSignalSemaphore *mProfileCallbackSemaphore;
    std::string mReceivedData;

};

#endif // APPLIB_TEST_1_HPP
