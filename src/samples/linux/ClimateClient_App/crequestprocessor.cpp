/* 
 * 
 * iviLINK SDK, version 1.0
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
#include <cassert>

CRequestProcessor::CRequestProcessor()
   : mClimateClient(iviLink::Service::Uid("ClimateClientService"))
{
}

//change driver's fan temperature request 
void CRequestProcessor::tempDecDrRequest()
{  
    mClimateClient.tempRequest(TEMP_DOWN_DRIVER);
}

//change driver's fan temperature request 
void CRequestProcessor::tempIncDrRequest()
{
    mClimateClient.tempRequest(TEMP_UP_DRIVER);
}

//change passenger's fan temperature request 
void CRequestProcessor::tempDecPassRequest()
{ 
    mClimateClient.tempRequest(TEMP_DOWN_PASS);
}

//change passenger's fan temperature request 
void CRequestProcessor::tempIncPassRequest()
{  
    mClimateClient.tempRequest(TEMP_UP_PASS);
}

//change sync mode request
void CRequestProcessor::syncRequest()
{ 
    mClimateClient.tempRequest(TEMP_SYNC);
}

//change fan direction request
void CRequestProcessor::fanLowRequest()
{ 
    mClimateClient.heaterRequest(HEAT_DOWN);
}

//change fan direction request
void CRequestProcessor::fanMidRequest()
{ 
    mClimateClient.heaterRequest(HEAT_ALL);
}

//change fan direction request
void CRequestProcessor::fanHighRequest()
{   
    mClimateClient.heaterRequest(HEAT_UP);
}

//change fan intensity request
void CRequestProcessor::fanIncRequest()
{  
    mClimateClient.fanRequest(FAN_FAST);
}

//change fan intensity request
void CRequestProcessor::fanDecRequest()
{ 
    mClimateClient.fanRequest(FAN_LOW);
}

//fan off request
void CRequestProcessor::offRequest()
{ 
    mClimateClient.fanRequest(FAN_OFF);
}

//change rear mode request
void CRequestProcessor::rearRequest()
{
    mClimateClient.generalRequest(GEN_REQ_REAR);
}

//change front mode request
void CRequestProcessor::frontRequest()
{
    mClimateClient.generalRequest(GEN_REQ_FRONT);
}

//change ac mode request
void CRequestProcessor::acRequest()
{
    mClimateClient.generalRequest(GEN_REQ_AC);
}

//change recirculation mode request
void CRequestProcessor::recircRequest()
{
    mClimateClient.generalRequest(GEN_REQ_RECIR);
}

//change auto mode request
void CRequestProcessor::autoRequest()
{
   mClimateClient.generalRequest(GEN_REQ_AUTO);
}

// initializing state request
void CRequestProcessor::initRequest()
{
   mClimateClient.initRequest();
}

