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


#ifndef CCONNECTIONFINDERTEST_HPP_
#define CCONNECTIONFINDERTEST_HPP_


#define IVILINK_UNITTESTBUILD

#include <map>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "CConnectionFinder.hpp"
#include "CTcpAutoConnectionFinder.hpp"
#include "CConnectivityAgent.hpp"
#include "CCarrierAdapter.hpp"
#include "CConnectivityAgentMock.hpp"

using namespace iviLink;
using namespace iviLink::ConnectivityAgent;
using namespace iviLink::ConnectivityAgent::HAL;

using ::testing::AtLeast;


std::map<std::string, int> counters;
int callIsThisThread = 0;

/*
 * for counting current class methods calls
 */
#define COUNT \
		std::cout<<__FUNCTION__<<std::endl; \
	if (counters.find(__FUNCTION__) == counters.end()) \
		{ \
			counters[__FUNCTION__] = 0; \
		} \
		++counters[__FUNCTION__];


 //Setters section

void CConnectionFinder::setState(EState currState)
{
	mState = currState;
}

/*
 * replaces the original method start()
 * extra contains a method calls counter
 */
bool CConnectionFinder::start()
{
	COUNT

    return CThread::start();
}

/*
 * replaces the original method stop()
 * contains a method calls counter
 */
bool CConnectionFinder::stop()
{
	COUNT

    return CThread::stop();
}

void CConnectionFinder::setReadyTest(CCarrierAdapter * pAdapter)
{
	setReady(pAdapter);
}

/*
 * replaces the original method isThisThread() in CThread class
 * depending on the callIsThisThread value returns:
 * 	 true/false in test case
 * 	 original value in default
 */
bool CThread::isThisThread() const
{
	switch (callIsThisThread)
	{
		case 1: cout<<"callIsThisThread = "<<callIsThisThread<<" must return false\n"; return false;
		case 2: cout<<"callIsThisThread = "<<callIsThisThread<<" must return true\n"; return true;
		default: cout<<"callIsThisThread = "<<callIsThisThread<<" must return default\n"; return pthread_self() == mThread;
	}
}

/*
 * replaces the original method signal() in CCondVar class
 * contains a method calls counter
 */
void CCondVar::signal()
{
	COUNT

	pthread_cond_signal(&mCond);
}

/*
 * CConnectionFinder testing class
 */
class CConnectionFinderTest : public ::testing::Test
{
protected:

	CConnectionFinderTest()
	{
	}

	~CConnectionFinderTest();

	void SetUp()
	{
        CConnectivityAgent theConnectivityAgent(true);

        IFoundConnectionHandler& rConnectivityAgent = theConnectivityAgent;

        EGenderType theGenderType(eServerGender);
        pConnectionFinder = new CTcpAutoConnectionFinder(rConnectivityAgent, theGenderType);


	}
	CConnectionFinder * pConnectionFinder;
	CConnectionFinder * pConnectionFinderM;
};

#undef IVILINK_UNITTESTBUILD
#endif /* CCONNECTIONFINDERTEST_HPP_ */
