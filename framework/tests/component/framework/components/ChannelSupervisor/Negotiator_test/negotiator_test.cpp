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


#include <cassert>
#include <ctime>
#include <string>
#include <iostream>
#include <sstream>

#include "ErrorCode.hpp"
#include "AllocateNotification.hpp"
#include "AllocateRequest.hpp"
#include "AllocateResponse.hpp"
#include "DeallocateRequest.hpp"
#include "DeallocateResponse.hpp"
#include "CommandRejectResponse.hpp"
#include "CommandTimeoutResponse.hpp"
#include "pugixml.hpp"

#define EXEC_NUM		10000
#define ID_NOT_EXIST 	(EXEC_NUM+1)


enum TEST_RESULT
{
	PASSED 		= 1,
	FAILED 		= 2,
	BLOCKED 	= 3,
};

//TEST_RESULT TestAddChannelInfo(Map * map, ChannelInfo chInfo[]);
//TEST_RESULT TestGetChannelInfo(Map * map, ChannelInfo chInfo[]);
//TEST_RESULT TestUpdateChannelInfo(Map * map, ChannelInfo chInfo[], ChannelInfo newchInfo[]);

TEST_RESULT TestAllocateNotificationMessage();

TEST_RESULT TestAllocateRequestMessage();
TEST_RESULT TestAllocateResponseMessage();
TEST_RESULT TestDeallocateRequestMessage();
TEST_RESULT TestDeallocateResponseMessage();
TEST_RESULT TestRejectResponseMessage();
TEST_RESULT TestTimeoutResponseMessage();
void ParseResponse( pugi::xml_document* doc, char* responseStr);

int main()
{
	/*ChannelInfo chInfo[EXEC_NUM];
	ChannelInfo chInfoNew[EXEC_NUM];

	Map * map = Map::getInstance();
	if(FAILED == TestAddChannelInfo(map, chInfo))
	{
		std::cout << "fail" << std::endl;
		map->deleteInstance();
		return 0;
	}
	if(FAILED == TestGetChannelInfo(map, chInfo))
	{
		std::cout << "fail" << std::endl;
		map->deleteInstance();
		return 0;
	}
	if(FAILED == TestUpdateChannelInfo(map, chInfo, chInfoNew))
	{
		std::cout << "fail" << std::endl;
		map->deleteInstance();
		return 0;
	}
*/

	TestAllocateNotificationMessage();

	/*TestAllocateRequestMessage();
	TestAllocateResponseMessage();
	TestDeallocateRequestMessage();
	TestDeallocateResponseMessage();
	TestRejectResponseMessage();
	TestTimeoutResponseMessage();*/

	std::cout << "pass" << std::endl;
	//map->deleteInstance();
	return 0;
}

TEST_RESULT TestAllocateNotificationMessage()
{
	std::cout << "create noti with no cid" << std::endl;
	{
		std::stringstream messageToBeSent;
		Message * noti  = new AllocateNotification("gutentag");

		noti->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;
	}

	std::cout << "create noti with cid" << std::endl;
	{
		std::stringstream messageToBeSent;
		Message * noti = new AllocateNotification("gutentag", 15);

		noti->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;
	}

	std::cout << "create noti with error" << std::endl;
	{
		std::stringstream messageToBeSent;
		Notification * noti = new AllocateNotification("gutentag", 15);
		noti->SetError(ERRORCODE_WRONG_CHANNELID, "crab happened");
		noti->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;
	}

	std::cout << "create Allocate noti with error and cid" << std::endl;
	{
		std::stringstream messageToBeSent;
		AllocateNotification * noti = new AllocateNotification("gutentag", 15);
		noti->SetError(ERRORCODE_WRONG_CHANNELID, "crab happened 2");
		noti->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;

		pugi::xml_document* doc = new pugi::xml_document();
		ParseResponse(doc, ( char*)(messageToBeSent.str().c_str()));
		AllocateNotification * res = new AllocateNotification(doc);

		std::cout << "channel id" << res->GetChannelId() << std::endl;
		ErrorCode errc;
		const char * errstr;
		res->GetError(errc, errstr);
		std::string messageName = res->GetMessageName();

		std::cout << "message name" << messageName << std::endl;
		std::cout << "err code" << errc << std::endl;
		std::cout << "err str" << errstr << std::endl;


	}

	std::cout << "test static functionality" << std::endl;
		{
			std::stringstream messageToBeSent;
			AllocateNotification * noti = new AllocateNotification("gutentag", 25);
			noti->SetError(ERRORCODE_WRONG_CHANNELID, "crab happened 3");
			noti->Write(messageToBeSent);
			std::cout << messageToBeSent.str() << std::endl;

			pugi::xml_document* doc = new pugi::xml_document();
			ParseResponse(doc, ( char*)(messageToBeSent.str().c_str()));

			if (Notification::NOTIFICATIONTTYPE_ALLOCATE == Notification::GetNotificationType(doc))
				std::cout << "type obtained properly" << std::endl;

			std::cout << "tag" << Notification::GetNotificationTag(doc) << std::endl;

		}


	return PASSED;

}


TEST_RESULT TestAllocateRequestMessage()
{
	std::stringstream messageToBeSent;

	AllocateRequest * req  = new AllocateRequest("super-puper-tag", 10);


	req->Write(messageToBeSent);

	std::cout << messageToBeSent.str() << std::endl;

	return PASSED;
}

TEST_RESULT TestDeallocateRequestMessage()
{
	std::stringstream messageToBeSent;

	DeallocateRequest * req  = new DeallocateRequest("super-puper-tag", 10);


	req->Write(messageToBeSent);

	std::cout << messageToBeSent.str() << std::endl;

	return PASSED;
}

TEST_RESULT TestAllocateResponseMessage()
{
	{
		std::cout << "just a response" << std::endl;
		std::stringstream messageToBeSent;
		AllocateResponse * req  = new AllocateResponse("super-puper-tag", 11);
		req->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;
	}

	{
		std::cout << "response with result" << std::endl;
		std::stringstream messageToBeSent;
		AllocateResponse * req  = new AllocateResponse("super-trouper-tag", 11);
		req->SetError(ERRORCODE_WRONG_CHANNELID, "Channel is wrong");
		req->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;
	}

	{
		std::cout << "response to object and get all the fields" << std::endl;
		std::stringstream messageToBeSent;
		AllocateResponse * req  = new AllocateResponse("super-man-tag", 11);
		req->SetError(ERRORCODE_CHANNELID_ALREADY_OCCUPIED, "Channel is occupied. try another one");
		req->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;

		pugi::xml_document* doc = new pugi::xml_document();
		ParseResponse(doc, ( char*)(messageToBeSent.str().c_str()));
		AllocateResponse * res = new AllocateResponse(doc);

		std::cout << "channel id" << res->GetChannelId() << std::endl;
		ErrorCode errc;
		const char * errstr;
		res->GetError(errc, errstr);
		std::cout << "err code" <<  errc << std::endl;
		std::cout << "err str" <<  errstr << std::endl;

	}

	return PASSED;
}

TEST_RESULT TestDeallocateResponseMessage()
{
	{
		std::cout << "just a response" << std::endl;
		std::stringstream messageToBeSent;
		DeallocateResponse * req  = new DeallocateResponse("super-puper-tag", 11);
		req->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;
	}

	{
		std::cout << "response with result" << std::endl;
		std::stringstream messageToBeSent;
		DeallocateResponse * req  = new DeallocateResponse("super-trouper-tag", 11);
		req->SetError(ERRORCODE_WRONG_CHANNELID, "Channel is wrong");
		req->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;
	}

	{
		std::cout << "response to object and get all the fields" << std::endl;
		std::stringstream messageToBeSent;
		DeallocateResponse * req  = new DeallocateResponse("super-man-tag", 11);
		req->SetError(ERRORCODE_CHANNELID_ALREADY_OCCUPIED, "Channel is occupied. try another one");
		req->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;

		pugi::xml_document* doc = new pugi::xml_document();
		ParseResponse(doc, ( char*)(messageToBeSent.str().c_str()));
		DeallocateResponse * res = new DeallocateResponse(doc);

		std::cout << "channel id" << res->GetChannelId() << std::endl;
		ErrorCode errc;
		const char * errstr;
		res->GetError(errc, errstr);
		std::cout << "err code" <<  errc << std::endl;
		std::cout << "err str" <<  errstr << std::endl;

	}

	return PASSED;
}

TEST_RESULT TestRejectResponseMessage()
{
	{
		std::cout << "just a response" << std::endl;
		std::stringstream messageToBeSent;
		CommandRejectResponse * req  = new CommandRejectResponse("allocate-channel");
		req->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;
	}

	{
		std::cout << "response with result" << std::endl;
		std::stringstream messageToBeSent;
		CommandRejectResponse * req  = new CommandRejectResponse("deallocate-channel");
		req->SetError(ERRORCODE_WRONG_CHANNELID, "Channel is wrong");
		req->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;
	}

	{
		std::cout << "response to object and get all the fields" << std::endl;
		std::stringstream messageToBeSent;
		CommandRejectResponse * req  = new CommandRejectResponse("super-man-tag");
		req->SetError(ERRORCODE_CHANNELID_ALREADY_OCCUPIED, "Channel is occupied. try another one");
		req->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;

		pugi::xml_document* doc = new pugi::xml_document();
		ParseResponse(doc, ( char*)(messageToBeSent.str().c_str()));
		CommandRejectResponse * res = new CommandRejectResponse(doc);

		std::cout << "channel id" << res->GetMessageName() << std::endl;
		ErrorCode errc;
		const char * errstr;
		res->GetError(errc, errstr);
		std::cout << "err code" <<  errc << std::endl;
		std::cout << "err str" <<  errstr << std::endl;

	}

	return PASSED;
}

TEST_RESULT TestTimeoutResponseMessage()
{
	{
		std::cout << "just a response" << std::endl;
		std::stringstream messageToBeSent;
		CommandTimeoutResponse * req  = new CommandTimeoutResponse("allocate-channel");
		req->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;
	}

	{
		std::cout << "response with result" << std::endl;
		std::stringstream messageToBeSent;
		CommandTimeoutResponse * req  = new CommandTimeoutResponse("deallocate-channel");
		req->SetError(ERRORCODE_WRONG_CHANNELID, "Channel is wrong");
		req->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;
	}

	{
		std::cout << "response to object and get all the fields" << std::endl;
		std::stringstream messageToBeSent;
		CommandTimeoutResponse * req  = new CommandTimeoutResponse("super-man-tag");
		req->SetError(ERRORCODE_CHANNELID_ALREADY_OCCUPIED, "Channel is occupied. try another one");
		req->Write(messageToBeSent);
		std::cout << messageToBeSent.str() << std::endl;

		pugi::xml_document* doc = new pugi::xml_document();
		ParseResponse(doc, ( char*)(messageToBeSent.str().c_str()));
		CommandTimeoutResponse * res = new CommandTimeoutResponse(doc);

		std::cout << "channel id" << res->GetMessageName() << std::endl;
		ErrorCode errc;
		const char * errstr;
		res->GetError(errc, errstr);
		std::cout << "err code" <<  errc << std::endl;
		std::cout << "err str" <<  errstr << std::endl;

	}

	return PASSED;
}

void ParseResponse( pugi::xml_document* doc, char* responseStr)
{
	pugi::xml_parse_result res;
	res = doc->load((const char *)responseStr);
}

/*TEST_RESULT TestAddChannelInfo(Map * map, ChannelInfo chInfo[])
{
	std::string tagBeginning = "tag_";

	//add new channel info. should be ok
	for (int i = 0; i < EXEC_NUM; i++)
	{
		std::stringstream ss;
		ss << tagBeginning << i;
		chInfo[i].tag = ss.str();
		chInfo[i].timeStamp = time(NULL);
		if(ERR_OK != map->addChannelInfo(i, &(chInfo[i])))
		{
			return FAILED;
		}
	}

	//add existing channel info. should be notok
	for (int i = 0; i < EXEC_NUM; i++)
	{
		std::stringstream ss;
		ss << tagBeginning << i;
		chInfo[i].tag = ss.str();
		chInfo[i].timeStamp = time(NULL);
		if(ERR_OK == map->addChannelInfo(i, &(chInfo[i])))
		{
			return FAILED;
		}
	}

	//test for wrong param
	if(ERR_OK == map->addChannelInfo(ID_NOT_EXIST, NULL))
	{
		return FAILED;
	}

	return PASSED;
}

TEST_RESULT TestGetChannelInfo(Map * map, ChannelInfo chInfo[])
{
	for (int i = 0; i < EXEC_NUM; i++)
	{
		ChannelInfo * test = map->getChannelInfo(i);
		if(test->tag != chInfo[i].tag)
		{
			return FAILED;
		}
		else if(test->timeStamp != chInfo[i].timeStamp)
		{
			return FAILED;
		}
	}

	//test for wrong param
	if(NULL != map->getChannelInfo(ID_NOT_EXIST))
	{
		return FAILED;
	}

	return PASSED;
}

TEST_RESULT TestUpdateChannelInfo(Map * map, ChannelInfo chInfo[], ChannelInfo newchInfo[])
{
	std::string newtagBeginning = "newtag_";

	for (int i = 0; i < EXEC_NUM; i++)
	{
		std::stringstream ss;
		ss << newtagBeginning << i;
		chInfo[i].tag = ss.str();
		newchInfo[i].timeStamp = time(NULL);
		if( ERR_OK != map->updateChannelInfo(i, &(newchInfo[i])))
		{
			return FAILED;
		}
	}

	//new channel info should work
	if(FAILED == TestGetChannelInfo(map, newchInfo))
	{
		return FAILED;
	}

	//old channel info should not work
	if(FAILED != TestGetChannelInfo(map, chInfo))
	{
		return FAILED;
	}

	//test for wrong param
	if(ERR_OK == map->updateChannelInfo(ID_NOT_EXIST, NULL))
	{
		return FAILED;
	}

	return PASSED;

}*/
