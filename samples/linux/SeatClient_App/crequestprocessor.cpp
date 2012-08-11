/* 
 * 
 * iviLINK SDK, version 1.0.1
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





#include "crequestprocessor.h"

CRequestProcessor::CRequestProcessor() : mProxy(NULL)
{
}

//loading profile
void CRequestProcessor::setProxy(CSeatClientProxy* proxy)
{
    mProxy = proxy;
    assert(mProxy);
    mLogger = Logger::getInstance("SeatClientSample.CRequestProcessor");
    LOG4CPLUS_INFO(mLogger, "CSeatClientProxy is set");
}

// change driver heater state request
void CRequestProcessor::driverHeaterRequest()
{
   mProxy->heaterRequest(DRIVER);
}

// change passenger heater state request
void CRequestProcessor::passHeaterRequest()
{
    mProxy->heaterRequest(PASSENGER);
}

// change visible seat state to driver request
void CRequestProcessor::driverSeatRequest()
{
    mProxy->tabRequest(DRIVER);
}

// change visible seat state to passenger request
void CRequestProcessor::passSeatRequest()
{
    mProxy->tabRequest(PASSENGER);
}

// move seatback request
void CRequestProcessor::backLeftRequest()
{
    mProxy->moveRequest(BACK_LEFT);
}

// move seatback request
void CRequestProcessor::backRightRequest()
{
    mProxy->moveRequest(BACK_RIGHT);
}

// move seat request 
void CRequestProcessor::bottomUpRequest()
{
    mProxy->moveRequest(BOTTOM_UP);
}

// move seat request 
void CRequestProcessor::bottomDownRequest()
{
    mProxy->moveRequest(BOTTOM_DOWN);
}

// move seat request 
void CRequestProcessor::bottomLeftRequest()
{
    mProxy->moveRequest(BOTTOM_LEFT);
}

// move seat request 
void CRequestProcessor::bottomRightRequest()
{
    mProxy->moveRequest(BOTTOM_RIGHT);
}

// initializing state request
void CRequestProcessor::initRequest()
{
   mProxy->initRequest();
}


