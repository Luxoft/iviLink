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


#include <iostream>
#include "utils/misc/include/Types.hpp"
#include "../common/CSError.hpp"
#include "../Tube/include/ChannelSupervisorTube.hpp"
#include "LocalDevice.hpp"


Int8 testTag[] = "cs_test_tag";


using namespace iviLink;
using namespace ChannelSupervisor;

//here is the Local device.

int main ()
{
	//error
	BaseError err = CSError(CSError::ERROR_OTHER);

	//observer
	IChannelSupervisorTubeObserver * observer = new LocalChannelObserver();

	//channel id
	UInt32 channelId = 0;

	//allocate channel
	std::cout << "TEST: allocate channel" << std::endl;
	err = allocateChannel(observer, testTag, channelId);

	std::cout << "TEST: allocated channel: " << channelId << std::endl;


	//send the data via allocated channel
	//std::cout << "TEST: send the data via allocated channel" << std::endl;

	//receive the data
	//std::cout << "TEST: receive the data" << std::endl;

	//deallocate channel
	//std::cout << "TEST: deallocate channel" << std::endl;


	if (observer)
		delete observer;

	observer = 0;
	return 0;
}
