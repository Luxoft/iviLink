/* 
 * 
 * iviLINK SDK, version 1.1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 * 
 */

/**
 * @file                main.cpp
 * @ingroup             Application Library Test # 9
 * @author              Mikhail Prosuntsov <MProsuntsov@luxoft.com>
 * @date                25.02.2013
 */

#include <iostream>
#include <fstream>
#include <string>

#include "Logger.hpp"

//app lib test section
#include "Waiter.hpp"
#include "InterAppMessage.hpp"
#include "Communicator.hpp"

#include "AppLibTest9.hpp"

static Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("AppLibTest.Tests.Test9"));

int main(int argc, char* argv[])
{
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
    LOG4CPLUS_INFO(logger, "START APP LIB TEST 9");

    int retVal = 1;

    std::ifstream addressFile("remoteipaddress.txt");
    std::string ipAddress;

    if (addressFile.is_open())
    {
        getline(addressFile, ipAddress);
        addressFile.close();
    }

    LOG4CPLUS_INFO(logger, "APPLIB TEST 9 Remote IPADDRESS: " + ipAddress);

    //some legacy crab
    CSignalSemaphore semaphore;

    //check if started by user
    if (argc > 1)
    {
        LOG4CPLUS_INFO(logger, "Started by user");

        //creating of app lib instance
        AppLibTest* appLibTest = new AppLibTest(semaphore);
        appLibTest->init();

        //communication preparation
        Communicator communicator(ipAddress.c_str());
        Waiter* waiter = new Waiter(SINGLE_MSG);

        appLibTest->setCommunicator(communicator);

        InterAppMessage result;
        communicator.addWaiter(waiter);

        LOG4CPLUS_INFO(logger, "Init done");

        LOG4CPLUS_INFO(logger, "Waiting for second party up");
        result = waiter->waitMessage(APPLICATION_UP_MSG, 20000);

        if (result != APPLICATION_UP_MSG)
        {
            LOG4CPLUS_ERROR(logger, "APP LIB TEST 9 - FAILED");
            return retVal;
        }

        sleep(5);

        LOG4CPLUS_INFO(logger, "Notify remote party everything fine");
        communicator.sendMessage(EVERYTHING_OK);

        LOG4CPLUS_INFO(logger, "APPLIB TEST 9 - STEP 4 UNloadService ");
        appLibTest->unload();

        LOG4CPLUS_INFO(logger, "Waiting remote dropped service");
        result = waiter->waitMessage(UNLOAD_SERVICE_DONE_MSG, 20000);
        if (result != UNLOAD_SERVICE_DONE_MSG)
        {
            LOG4CPLUS_ERROR(logger, "APP LIB TEST 9 - FAILED");
            return retVal;
        }

        LOG4CPLUS_INFO(logger, "Waiting remote dropped service");
        result = waiter->waitMessage(TEST_PASSED_MSG, 20000);
        if (result != TEST_PASSED_MSG)
        {
            LOG4CPLUS_ERROR(logger, "APP LIB TEST 9 - FAILED");
            return retVal;
        }

        LOG4CPLUS_INFO(logger, "Say goodbye");

        //deleting the ivilink app object instance
        if (appLibTest)
        {
            delete appLibTest;
        }

        LOG4CPLUS_INFO(logger, "APP LIB TEST 9 - PASSED");
        retVal = 0;

    }
    else    //second party code here
    {
        //creating of app lib instance
        AppLibTest* appLibTest = new AppLibTest(semaphore);
        appLibTest->init();

        //communication preparation
        Communicator communicator(ipAddress.c_str());
        Waiter* waiter = new Waiter(SINGLE_MSG);

        InterAppMessage result;
        communicator.addWaiter(waiter);

        LOG4CPLUS_INFO(logger, "Started by iviLink");

        appLibTest->setCommunicator(communicator);

        LOG4CPLUS_INFO(logger, "Init done");
        sleep(5);

        //sending msg we are up
        communicator.sendMessage(APPLICATION_UP_MSG);

        LOG4CPLUS_INFO(logger, "Make sure second party is fine");
        result = waiter->waitMessage(EVERYTHING_OK, 20000);
        if (result != EVERYTHING_OK)
        {
            LOG4CPLUS_ERROR(logger, "APP LIB TEST 9 - FAILED");
            return retVal;
        }

        //other party should do unload of the service
        //sending service dropped msg from callback

        //wait a little
        sleep(5);

        //sending we are ok
        communicator.sendMessage(TEST_PASSED_MSG);
        LOG4CPLUS_INFO(logger, "APP LIB TEST 9 - PASSED");

        retVal = 0;
    }

    return retVal;
}
