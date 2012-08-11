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






#include <iostream>
#include "cclimatecond.h"

using namespace limitation_helpers;

CClimateCond::CClimateCond(CRequestProcessor * pCRequestProcessor)
{
    mpCRequestProcessor = pCRequestProcessor;

}

// functions processing state change requests

bool CClimateCond::update_state( bool force_update )
{
    if( force_update || oldState != currState )
    {
        oldState = currState;
        notifying();
        return true;
    }
    return false;
}


void CClimateCond::tempDriverDown()
{
    QMutexLocker locker(&mutex);
    //check if dual or sync. climate
    dec_temperature( currState.driverTemp );
    if (!currState.dualClimate)
        currState.passTemp = currState.driverTemp;
    update_state();
}

void CClimateCond::tempDriverUp()
{
    QMutexLocker locker(&mutex);
    //check if dual or sync. climate
    inc_temperature( currState.driverTemp );
    if (!currState.dualClimate)
        currState.passTemp = currState.driverTemp;
    update_state();
}

void CClimateCond::tempPassDown()
{
    QMutexLocker locker(&mutex);
    if( dec_temperature( currState.passTemp) )
    {
        //in any case switch to dual
        currState.dualClimate = true;
        //auto goes off
        currState.autoClimate = false;
    }
    update_state();
}

void CClimateCond::tempPassUp()
{
    QMutexLocker locker(&mutex);  
    if( inc_temperature( currState.passTemp) )
    {
        //in any case switch to dual
        currState.dualClimate = true;
        //auto goes off
        currState.autoClimate = false;
    }
    update_state();
}

void CClimateCond::sync()
{
    QMutexLocker locker(&mutex);
    if (currState.dualClimate) {
        //turn dual off
        currState.passTemp = currState.driverTemp;
        currState.dualClimate = false;
        //auto goes off
        currState.autoClimate = false;
    } else {
        currState.dualClimate = true;
        //auto goes off
        currState.autoClimate = false;
    }
    update_state();
}

void CClimateCond::fanLow()
{
    QMutexLocker locker(&mutex);
    currState.fanModeLow = !(currState.fanModeLow);
    currState.autoClimate = false;
    notifying();
}

void CClimateCond::fanMid()
{
    QMutexLocker locker(&mutex);  
    currState.fanModeMiddle = !(currState.fanModeMiddle);
    currState.autoClimate = false;
    update_state( true );
}

void CClimateCond::fanHigh()
{
    QMutexLocker locker(&mutex);
    currState.fanModeTop = !(currState.fanModeTop);
    currState.autoClimate = false;
    update_state( true );
}

void CClimateCond::fanInc()
{
    QMutexLocker locker(&mutex);
    if (currState.isOFF)
    {
        currState = oldState;
        currState.fanSpeed = 1; //set fan to 0 + 1 speed
        currState.isOFF = false;
        currState.autoClimate = false;
    }
    else
    {
        if( inc_fan_speed( currState.fanSpeed ) )
            // turn off auto climate if speed changed
            currState.autoClimate = false;
    }
    update_state();
}

void CClimateCond::fanDec()
{
    QMutexLocker locker(&mutex);
    if( dec_fan_speed(currState.fanSpeed) )
    {
        currState.autoClimate = false;
	// Should we turn the system off ?
	//	if( currState.fanSpeed == 0 )
	//currState.isOFF = true;
    }
    update_state();
}

void CClimateCond::fanOff()
{
    QMutexLocker locker(&mutex);
    if (!currState.isOFF) {
        oldState = currState;
        currState.airRecirc = false;
        currState.autoClimate = false;
        //currState.dualClimate = false;
        currState.fanSpeed = 0; // 0 -- 10
        currState.frontWindow = false;
        currState.isAC = false;
        currState.isOFF = true;
        currState.rearWindow = false;
        currState.fanModeLow = false;
        currState.fanModeMiddle = false;
        currState.fanModeTop = false;
    } else {
        currState = oldState;
    }
    update_state( true );
}

void CClimateCond::rear()
{
    QMutexLocker locker(&mutex);
    //change rear window warming
    currState.rearWindow = !(currState.rearWindow);
    oldState = currState;
    notifying();
}

void CClimateCond::front()
{
    QMutexLocker locker(&mutex);
    //change wind shield warming
    currState.frontWindow = !(currState.frontWindow);
    oldState = currState;
    update_state( true );
}

void CClimateCond::ac()
{
    QMutexLocker locker(&mutex);
    //change AC
    currState.isAC = !(currState.isAC);
    //turn auto off
    currState.autoClimate = false;
    //auto goes off
    oldState = currState;
    update_state( true );
}

void CClimateCond::recirc()
{
    QMutexLocker locker(&mutex);
    currState.airRecirc = !(currState.airRecirc);
    currState.autoClimate = false;
    update_state( true );
}

void CClimateCond::autoMode()
{
    QMutexLocker locker(&mutex);
    if (!currState.autoClimate) //if it was off
    {
        setDefaultState();
        currState.driverTemp = oldState.driverTemp;
        currState.passTemp = currState.driverTemp;
    } else {
        currState.autoClimate = false;
    }
    update_state( true );
}

void CClimateCond::initialization()
{
    QMutexLocker locker(&mutex);
    notifying();
}

void CClimateCond::setDefaultState()
{
    currState.airRecirc = false;
    currState.autoClimate = true;
    currState.driverTemp = 20; // 16 -- 27
    currState.dualClimate = false;
    currState.fanSpeed = 5; // 0 -- 10
    currState.frontWindow = false;
    currState.isAC = true;
    currState.isOFF = false;
    currState.passTemp = 20; // 16 -- 27
    currState.rearWindow = false;
    currState.fanModeLow = false;
    currState.fanModeMiddle = false;
    currState.fanModeTop = false;
}

//send notification about current state
void CClimateCond::notifying()
{
    Json::Value state;

    state["fanOff"] = currState.isOFF;
    state["driverTemp"] = currState.driverTemp;
    state["passTemp"] = currState.passTemp;
    state["fanSpeed"] = currState.fanSpeed;
    state["auto"] = currState.autoClimate;
    state["sync"] = currState.dualClimate;
    state["front"] = currState.frontWindow;
    state["rear"] = currState.rearWindow;
    state["fanLow"] = currState.fanModeLow;
    state["fanMid"] = currState.fanModeMiddle;
    state["fanHigh"] = currState.fanModeTop;
    state["ac"] = currState.isAC;
    state["recirc"] = currState.airRecirc;

    mpCRequestProcessor->sendNotification(state);

}
