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


CRequestProcessor::CRequestProcessor()
   : mClimateHU(iviLink::Service::Uid("ClimateHUService"))
{
}

//Send the state of conditioner to the stateless side
void CRequestProcessor::sendNotification(Json::Value condState)
{
    //updating GUI according to the new state
    emit showClimate();

    bool status = condState.get("auto", false).asBool();
    emit autoState(status);
    status = condState.get("recirc", false).asBool();
    emit recircState(status);
    status = condState.get("ac", false).asBool();
    emit acState(status);
    status = condState.get("front", false).asBool();
    emit frontState(status);
    status = condState.get("rear", false).asBool();
    emit rearState(status);
    status = condState.get("fanLow", false).asBool();
    emit fanLowState(status);
    status = condState.get("fanMid", false).asBool();
    emit fanMidState(status);
    status = condState.get("fanHigh", false).asBool();
    emit fanHighState(status);
    int temp = condState.get("passTemp", 20).asInt();
    emit passTemp(temp);
    temp = condState.get("driverTemp", 20).asInt();
    emit driverTemp(temp);
    status = condState.get("sync", false).asBool();
    emit syncState(status);
    int speed = condState.get("fanSpeed", 0).asInt();
    emit fanSpeed(speed);
    status = condState.get("fanOff", false).asBool();
    emit fanOffState(status);
    
    //send the new state to another side
    mClimateHU.sendNotification(condState);
}

