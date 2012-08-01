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



#ifndef SEATCOMMON_H
#define SEATCOMMON_H

enum LIMITATIONS
{
    MAX_HEATER = 3,
    MIN_HEATER = 0,
    MAX_ANGLE = 70,
    MIN_ANGLE = (-40),
    ANGLE_STEP = 10,
    LIM_BOTTOM_LEFT = 454,
    LIM_BOTTOM_RIGHT = 494,
    LIM_BOTTOM_UP = 302,
    LIM_BOTTOM_DOWN = 342,
    INIT_BOTTOM_X = 474,
    INIT_BOTTOM_Y = 322,
    INIT_BACK_X = 535,
    INIT_BACK_Y = 197,
    SEAT_STEP = 5,
    INIT_ANGLE = 0,

};

enum PERSONS
{
    DR = 0,
    PASS = 1
};

struct seat
{
    int dr_bottom_x;
    int dr_bottom_y;
    int dr_back_x;
    int dr_back_y;
    int dr_back_angle;
    int pass_bottom_x;
    int pass_bottom_y;
    int pass_back_x;
    int pass_back_y;
    int pass_back_angle;
    int driverHeater;
    int passHeater;
    int currSeat; //0 - driver 1 passenger

    seat() :  dr_bottom_x(INIT_BOTTOM_X), dr_bottom_y(INIT_BOTTOM_Y), dr_back_x(INIT_BACK_X), dr_back_y(INIT_BACK_Y), dr_back_angle(INIT_ANGLE), pass_bottom_x(INIT_BOTTOM_X),
        pass_bottom_y(INIT_BOTTOM_Y), pass_back_x(INIT_BACK_X), pass_back_y(INIT_BACK_Y), pass_back_angle(INIT_ANGLE), driverHeater(MIN_HEATER), passHeater(MIN_HEATER), currSeat(DR){}

};

#endif // SEATCOMMON_H
