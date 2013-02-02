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


#include "CConnectionFinderTest.hpp"

CConnectionFinderTest::~CConnectionFinderTest()
{

}

TEST_F(CConnectionFinderTest, Method_start)
{
	//CCF1
	EXPECT_EQ(true, pConnectionFinder->start());
	pConnectionFinder->stop();
}

TEST_F(CConnectionFinderTest, Method_stop)
{
	//CCF2
	pConnectionFinder->start();
	EXPECT_EQ(true, pConnectionFinder->stop());

}

TEST_F(CConnectionFinderTest, Method_startSearch)
{
	//CCF3
	std::map<std::string, int>::iterator start;
	start = counters.find("start");
	start->second = 0;
	pConnectionFinder->setState(CConnectionFinder::eStopped);

	pConnectionFinder->startSearch();

	EXPECT_EQ(CConnectionFinder::eSearching,pConnectionFinder->getState());
	EXPECT_EQ(1, start->second);

	pConnectionFinder->stopSearch();


	//CCF4
	pConnectionFinder->setState(CConnectionFinder::eWaitAck);

	pConnectionFinder->startSearch();

	EXPECT_EQ(CConnectionFinder::eWaitAck,pConnectionFinder->getState());

	pConnectionFinder->setState(CConnectionFinder::eSearching);
	pConnectionFinder->stopSearch();
}

TEST_F(CConnectionFinderTest, Method_stopSearch)
{
	std::map<std::string, int>::iterator stop;
	std::map<std::string, int>::iterator signal;

	//CCF5
	pConnectionFinder->startSearch();

	stop = counters.find("stop");
	stop->second = 0;
	signal = counters.find("signal");
	signal->second = 0;
	callIsThisThread ++;
	pConnectionFinder->stopSearch();

	EXPECT_EQ(CConnectionFinder::eStopped,pConnectionFinder->getState());
	EXPECT_EQ(1, stop->second);
	EXPECT_EQ(1, signal->second);

	//CCF6
	pConnectionFinder->startSearch();

	stop->second = 0;
	signal->second = 0;
	callIsThisThread ++;

	pConnectionFinder->stopSearch();

	EXPECT_EQ(CConnectionFinder::eStopped,pConnectionFinder->getState());
	EXPECT_EQ(0, stop->second);
	EXPECT_EQ(1, signal->second);

	callIsThisThread = 0;

	//CCF7
	pConnectionFinder->startSearch();
	pConnectionFinder->setState(CConnectionFinder::eStopped);
	signal->second = 0;

	pConnectionFinder->stopSearch();

	EXPECT_EQ(CConnectionFinder::eStopped, pConnectionFinder->getState());
	EXPECT_EQ(0, signal->second);
}

TEST_F(CConnectionFinderTest, Method_setReady)
{

    CConnectivityAgentMock theConnectivityAgentMock(true);
    EGenderType theGenderType(eServerGender);
    pConnectionFinderM = new CTcpAutoConnectionFinder(theConnectivityAgentMock, theGenderType);
	//CCF8
	pConnectionFinderM->startSearch();

	CCarrierAdapter* pCarrierAdapter = pConnectionFinderM->getConnectedAdapter();

	//pConnectionFinderM->setReadyTest(pCarrierAdapter);

	//EXPECT_EQ(1, signal->second);
	EXPECT_CALL(theConnectivityAgentMock, connectionFound(pConnectionFinderM)).Times(AtLeast(1));
	pConnectionFinderM->setReadyTest(pCarrierAdapter);

}


int main(int argc, char **argv)
{
	::testing::InitGoogleMock(&argc, argv);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
