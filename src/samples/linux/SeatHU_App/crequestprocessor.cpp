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
CRequestProcessor::CRequestProcessor(): mProxy(NULL)
{
}

//loading profile
void CRequestProcessor::setProxy(CSeatHUProxy * proxy)
{    
    mProxy = proxy;
    assert(mProxy);
}

//Send the state of seat to the stateless side
void CRequestProcessor::sendNotification(Json::Value seatState)
{

     //updating GUI according to the new state
     emit showSeat();

     int status = seatState.get("currSeat", 0).asInt();
     if (status == 0)
         emit current_seat_viewDriver();
     else
         emit current_seat_viewPass();
     status = seatState.get("heaterDriver", 0).asInt();
     emit heaterDriver(status);
     status = seatState.get("heaterPass", 0).asInt();
     emit heaterPass(status);
     status = seatState.get("bottom_x", 0).asInt();
     emit bottom_x(status);
     status = seatState.get("bottom_y", 0).asInt();
     emit bottom_y(status);
     status = seatState.get("back_x", 0).asInt();
     emit back_x(status);
     status = seatState.get("back_y", 0).asInt();
     emit back_y(status);
     status = seatState.get("back_angle", 0).asInt();
     emit back_angle(status);

     //send the new state to another side
     if(mProxy)  mProxy->sendNotification(seatState);
}

