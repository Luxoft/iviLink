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
#include <string>
#include <sstream>
#include <string.h>
#include <pthread.h>
#include "../common/Common.hpp"
#include "utils/misc/include/Types.hpp"
#include "framework/components/ConnectivityAgent/src/generic/common/include/API.hpp"
#include "../Tube/include/ChannelSupervisorTube.hpp"
#include "TubeTest.hpp"

using namespace iviLink;
using namespace ChannelSupervisor;

void TubeTest()
{
	UInt32 cid;
	ChannelObserver cObserver;

	//allocateTubeTest_many(cid, &cObserver);

	while (1)
	{
		std::cout << "RECEIVE" << std::endl;
		receiveDataTubeTest(cid, &cObserver);
		sleep(1);
		std::cout << "ALLOCATE" << std::endl;
		allocateTubeTest(cid, &cObserver);
		sleep(1);
		std::cout << "DEALLOCATE" << std::endl;
		deallocateTubeTest(cid, &cObserver);
		sleep(1);
	}
	int i = 0;
	while (i < 1)
	{
		//sendDataTubeTest(cid, &cObserver);
		//wdKickTubeTest(cid, &cObserver);
		sleep(2);
		i++;
	}

	deallocateTubeTest(cid, &cObserver);
}

//normal allocation
void allocateTubeTest(UInt32 & cid, ChannelObserver* cObserver)
{
	std::string tag = "gutentag";
	//do allocation
	CError err = allocateChannel((IChannelSupervisorTubeObserver*) cObserver,
			tag, cid);
	if (err.isNoError())
		std::cout << "test passed" << std::endl;
	else
		std::cout << "test failed" << std::endl;
	std::cout << cid << std::endl;
}
//already allocated
void allocateTubeTest_already(UInt32 & cid, ChannelObserver* cObserver)
{
	std::string tag = "gutentag";
	//do allocation
	CError err = allocateChannel((IChannelSupervisorTubeObserver*) cObserver,
			tag, cid);
	if (err.isNoError())
	{
		std::cout << "allocation failed" << std::endl;
		return;
	}
	err = allocateChannel((IChannelSupervisorTubeObserver*) cObserver,
			tag, cid);
	if (err.isNoError())
	{
		std::cout << "test failed" << std::endl;
	}

	std::cout << cid << std::endl;
}
//many channels allocation
void allocateTubeTest_many(UInt32 & cid, ChannelObserver* cObserver)
{
	std::string tag = "gutentag";
	std::stringstream iteration;

	for (unsigned int i = 0; i < 10000; i++)
	{
		iteration << i;

		CError err = allocateChannel(
				(IChannelSupervisorTubeObserver*) cObserver,
				tag + iteration.str(), cid);
		if (err.isNoError())
		{
			std::cout << "test failed" << std::endl;
			return;
		}
		std::cout << cid << std::endl;

	}
	return;

}

//allocation no observer
void allocateTubeTest_noobserver(UInt32 & cid, ChannelObserver* cObserver)
{
	cObserver = NULL;
	std::string tag = "gutentag";

	CError err = allocateChannel((IChannelSupervisorTubeObserver*) cObserver,
			tag, cid);
	if (err.isNoError())
	{
		std::cout << "test failed" << std::endl;
		return;
	}
	std::cout << cid << std::endl;

	return;
}

//allocation no observer
void allocateTubeTest_emptytag(UInt32 & cid, ChannelObserver* cObserver)
{
	std::string tag;

	CError err = allocateChannel((IChannelSupervisorTubeObserver*) cObserver,
			tag, cid);
	if (err.isNoError())
	{
		std::cout << "test failed" << std::endl;
		return;
	}
	std::cout << cid << std::endl;

	return;
}

//normal deallocation
void deallocateTubeTest(UInt32 & cid, ChannelObserver* cObserver)
{
	std::string tag = "gutentag";

	//do allocation
	CError err = allocateChannel((IChannelSupervisorTubeObserver*) cObserver,
			tag, cid);
	if (err.isNoError())
	{
		std::cout << "test failed (on allocation)" << std::endl;
		return;
	}

	//do deallocation
	err = deallocateChannel(cid, tag);
	if (err.isNoError())
	{
		std::cout << "test failed" << std::endl;
		return;
	}

	std::cout << cid << std::endl;
}

//deallocate already deallocated
void deallocateTubeTest_deallocated(UInt32 & cid, ChannelObserver* cObserver)
{
	std::string tag = "gutentag";

	//do allocation
	CError err = allocateChannel((IChannelSupervisorTubeObserver*) cObserver,
			tag, cid);
	if (err.isNoError())
	{
		std::cout << "test failed (on allocation)" << std::endl;
		return;
	}

	//do deallocation
	err = deallocateChannel(cid, tag);
	if (err.isNoError())
	{
		std::cout << "test failed" << std::endl;
		return;
	}

	//do deallocation of deallocated channel
	err = deallocateChannel(cid, tag);
	if (err.isNoError())
	{
		std::cout << "test failed" << std::endl;
		return;
	}

	std::cout << cid << std::endl;
}

//deallocate already deallocated
void deallocateTubeTest_wrongtag(UInt32 & cid, ChannelObserver* cObserver)
{
	std::string goodtag = "gutentag";
	std::string badtag = "schlechtentag";

	//do allocation
	CError err = allocateChannel((IChannelSupervisorTubeObserver*) cObserver,
			goodtag, cid);
	if (err.isNoError())
	{
		std::cout << "test failed (on allocation)" << std::endl;
		return;
	}

	//do deallocation
	err = deallocateChannel(cid, badtag);
	if (err.isNoError())
	{
		std::cout << "test failed" << std::endl;
		return;
	}

	//do deallocation of deallocated channel
	err = deallocateChannel(cid, goodtag);
	if (err.isNoError())
	{
		std::cout << "test failed" << std::endl;
		return;
	}

	std::cout << cid << std::endl;
}

//deallocate already deallocated
void deallocateTubeTest_wrongcid(UInt32 & cid, ChannelObserver* cObserver)
{
	std::string tag = "gutentag";
	unsigned int wrong_cid = 5;

	//do allocation
	CError err = allocateChannel((IChannelSupervisorTubeObserver*) cObserver,
			tag, cid);
	if (err.isNoError())
	{
		std::cout << "test failed (on allocation)" << std::endl;
		return;
	}

	//do deallocation
	err = deallocateChannel(wrong_cid, tag);
	if (err.isNoError())
	{
		std::cout << "test failed" << std::endl;
		return;
	}

	//do deallocation of allocated channel
	err = deallocateChannel(cid, tag);
	if (err.isNoError())
	{
		std::cout << "test failed" << std::endl;
		return;
	}

	std::cout << cid << std::endl;
}

void sendDataTubeTest(UInt32 & cid, ChannelObserver* cObserver)
{
	std::string tag = "gutentag";
	unsigned char data[] = "Some testing string";
	const unsigned int size = strlen((const char *) data);

	//do allocation
	CError err = allocateChannel((IChannelSupervisorTubeObserver*) cObserver,
			tag, cid);
	{
		std::cout << "test failed (on allocation)" << std::endl;
		return;
	}

	//send the data	
	err = sendData(cid, tag, data, size);
	if (err.isNoError())
	{
		std::cout << "test failed" << std::endl;
		return;
	}

	//do deallocation of allocated channel
	err = deallocateChannel(cid, tag);
	if (err.isNoError())
	{
		std::cout << "test failed (on deallocation)" << std::endl;
		return;
	}

}

#define NUM_THREADS     1

void *RequestDataCB(void *threadid)
{
	//another magic number
	unsigned int cid = 15;
	sleep(30); //wait until everything is up in another thread

	int i = 0;
	while (i < 1000)
	{
		reqCallback(cid);
		sleep(1);
		i++;
	}

	pthread_exit(NULL);
}

void receiveDataTubeTest(UInt32 & cid, ChannelObserver* cObserver)
{
	std::string tag = "gutentag";

	//do allocation
	CError err = allocateChannel((IChannelSupervisorTubeObserver*) cObserver,
			tag, cid);
	if (err.isNoError())
	{
		std::cout << "test failed (on allocation)" << std::endl;
		return;
	}

	pthread_t threads[NUM_THREADS];

	long t;
	int rc;

	for (t = 0; t < NUM_THREADS; t++)
	{
		std::cout << "In main: creating thread" << t << std::endl;
		rc = pthread_create(&threads[t], NULL, RequestDataCB, (void *) t);
		if (rc)
		{
			std::cout << "ERROR; return code from pthread_create() is " << rc
					<< std::endl;
			return;
		}
	}

	//do deallocation of allocated channel
	err = deallocateChannel(cid, tag);
	if (err.isNoError())
	{
		std::cout << "test failed (on deallocation)" << std::endl;
	}

	pthread_exit(NULL);
}

void receiveDataFromCB(const UInt32 cid)
{
	unsigned char * data;
	unsigned int receivedSize;
	unsigned int maxSize = 10;

	std::string tag = "gutentag";

	receiveData(cid, tag, data, receivedSize, maxSize);

}

void wdKickTubeTest(UInt32 cid)
{
	std::string tag = "gutentag";
	kickWatchdog(cid, tag);
}

void reqCallback(UInt32 cid)
{
	//trequestCallback(DATA_RECEIVED, (const UInt32) cid);
}

int main(int argc, char** argv)
{

	TubeTest();

	return 0;
}

//void 
