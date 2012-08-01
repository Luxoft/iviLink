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



#include "cseatstate.h"
#include "seatcommon.h"

CSeatState::CSeatState(CRequestProcessor * pCRequestProcessor)
{
    mpCRequestProcessor = pCRequestProcessor;
}


//send notification about current state
void CSeatState::notifying()
{
    Json::Value state;

    state["currSeat"] = currState.currSeat;
    state["heaterDriver"] = currState.driverHeater;
    state["heaterPass"] = currState.passHeater;
    if (currState.currSeat == DR)
    {
        state["bottom_x"] = currState.dr_bottom_x;
        state["bottom_y"] = currState.dr_bottom_y;
        state["back_x"] = currState.dr_back_x;
        state["back_y"] = currState.dr_back_y;
        state["back_angle"] = currState.dr_back_angle;
    }
    else if (currState.currSeat == PASS)
    {
        state["bottom_x"] = currState.pass_bottom_x;
        state["bottom_y"] = currState.pass_bottom_y;
        state["back_x"] = currState.pass_back_x;
        state["back_y"] = currState.pass_back_y;
        state["back_angle"] = currState.pass_back_angle;
    }

    mpCRequestProcessor->sendNotification(state);

}

void CSeatState::setProxy(CSeatHUProxy * proxy)
{
   mpCRequestProcessor->setProxy(proxy);
}

void CSeatState::initialization()
{
    QMutexLocker locker(&mutex);
    notifying();
}

// functions processing state change requests

void CSeatState::driverHeater()
{
    QMutexLocker locker(&mutex);

    if (currState.driverHeater > MIN_HEATER) {
        --currState.driverHeater;
    } else if (currState.driverHeater <= MIN_HEATER) {
        currState.driverHeater = MAX_HEATER;
    }

    notifying();
}

void CSeatState::passHeater()
{
    QMutexLocker locker(&mutex);

    if (currState.passHeater > MIN_HEATER) {
        --currState.passHeater;
    } else if (currState.passHeater <= MIN_HEATER) {
        currState.passHeater = MAX_HEATER;
    }

    notifying();
}

void CSeatState::driverSeat()
{
    QMutexLocker locker(&mutex);
    currState.currSeat = DR;

    notifying();
}

void CSeatState::passSeat()
{
    QMutexLocker locker(&mutex);
    currState.currSeat = PASS;

    notifying();
}

void CSeatState::backLeft()
{
    QMutexLocker locker(&mutex);

    if (currState.currSeat == DR) {
        //driver
        if(currState.dr_back_angle - ANGLE_STEP > MIN_ANGLE)
            currState.dr_back_angle -= ANGLE_STEP;
    } else if (currState.currSeat == PASS) {
        //passen
        if(currState.pass_back_angle - ANGLE_STEP > MIN_ANGLE)
            currState.pass_back_angle -= ANGLE_STEP;
    }

    notifying();
}

void CSeatState::backRight()
{
    QMutexLocker locker(&mutex);

    if (currState.currSeat == DR) {
        //driver
        if(currState.dr_back_angle + ANGLE_STEP < MAX_ANGLE)
            currState.dr_back_angle += ANGLE_STEP;
    } else if (currState.currSeat == PASS) {
        //passen
        if(currState.pass_back_angle + ANGLE_STEP < MAX_ANGLE)
            currState.pass_back_angle += ANGLE_STEP;
    }

    notifying();
}

void CSeatState::bottomUp()
{
    QMutexLocker locker(&mutex);

    if (currState.currSeat == DR)
    {
        //driver
        if(currState.dr_bottom_y - SEAT_STEP > LIM_BOTTOM_UP)
        {
            currState.dr_bottom_y -= SEAT_STEP;
            currState.dr_back_y -= SEAT_STEP;
        }
    } else if (currState.currSeat == PASS) {
        //passen
        if(currState.pass_bottom_y - SEAT_STEP > LIM_BOTTOM_UP)
        {
            currState.pass_bottom_y -= SEAT_STEP;
            currState.pass_back_y -= SEAT_STEP;
        }
    }

    notifying();
}

void CSeatState::bottomDown()
{
    QMutexLocker locker(&mutex);

    if (currState.currSeat == DR)
    {
        //driver
        if(currState.dr_bottom_y + SEAT_STEP < LIM_BOTTOM_DOWN)
        {
            currState.dr_bottom_y += SEAT_STEP;
            currState.dr_back_y += SEAT_STEP;
        }
    } else if (currState.currSeat == PASS) {
        //passen
        if(currState.pass_bottom_y + SEAT_STEP < LIM_BOTTOM_DOWN)
        {
            currState.pass_bottom_y += SEAT_STEP;
            currState.pass_back_y += SEAT_STEP;
        }
    }

    notifying();
}

void CSeatState::bottomLeft()
{
    QMutexLocker locker(&mutex);

    if (currState.currSeat == DR)
    {
        //driver
        if(currState.dr_bottom_x - SEAT_STEP > LIM_BOTTOM_LEFT)
        {
            currState.dr_bottom_x -= SEAT_STEP;
            currState.dr_back_x -= SEAT_STEP;
        }
    } else if (currState.currSeat == PASS) {
        //passen
        if(currState.pass_bottom_x - SEAT_STEP > LIM_BOTTOM_LEFT)
        {
            currState.pass_bottom_x -= SEAT_STEP;
            currState.pass_back_x -= SEAT_STEP;
        }
    }

    notifying();
}

void CSeatState::bottomRight()
{
    QMutexLocker locker(&mutex);

    if (currState.currSeat == DR)
    {
        //driver
        if(currState.dr_bottom_x + SEAT_STEP < LIM_BOTTOM_RIGHT)
        {
            currState.dr_bottom_x += SEAT_STEP;
            currState.dr_back_x += SEAT_STEP;
        }
    } else if (currState.currSeat == PASS) {
        //passen
        if(currState.pass_bottom_x + SEAT_STEP < LIM_BOTTOM_RIGHT)
        {
            currState.pass_bottom_x += SEAT_STEP;
            currState.pass_back_x += SEAT_STEP;
        }
    }

    notifying();
}

