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
#include "Waiter.hpp"
#include "InterAppMessage.hpp"
#include "Communicator.hpp"

using namespace std;

/*
 *  doing some testing of the Communicator
 */

int main(int argc, char** argv)
{

    Communicator communicator(argv[1]);
    Waiter* waiter = new Waiter(SINGLE_MSG);

    InterAppMessage message3 = INCOMING_BEFORE_MSG;
    InterAppMessage message2 = APPLICATION_UP_MSG;

    InterAppMessage result;

    communicator.addWaiter(waiter);

    std::cout << "!!!!!!!!!!!!!!!!!!!! created" << std::endl;

//TEST1  
    result = waiter->waitMessage(message2, 20000);
    std::cout << "!!!!!!!!!!!!!!!!!!!! wait complete, result: " << result << std::endl;
    communicator.sendMessage(message3);
    std::cout << "!!!!!!!!!!!!!!!!!!!! sent" << std::endl;

//TEST2
    result = waiter->waitMessage(message2, 10000);
    std::cout << "!!!!!!!!!!!!!!!!!!!! wait complete, result: " << result << std::endl;
    result = waiter->waitMessage(message3, 30000);
    std::cout << "!!!!!!!!!!!!!!!!!!!! wait complete, result: " << result << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!! end: " << std::endl;
    sleep(10);
    communicator.sendMessage(message2);

    communicator.removeWaiter(waiter);

    return 0;
}

