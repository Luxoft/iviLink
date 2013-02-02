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
#include "SeatState.hpp"

#define SEAT_HEATER_TAG "heater"
#define SEAT_X_TAG "x"
#define SEAT_Y_TAG "y"
#define SEAT_BACK_ANGLE_TAG "back angle"
#define SEAT_DRIVER_TAG "driver seat"
#define SEAT_PASSEN_TAG "passen seat"
#define SEAT_SELECTION_TAG "current seat"

namespace iviLink {

std::string SeatState::serialize() const
{
    Json::Value root;
    root[SEAT_DRIVER_TAG] = serializeOneSeat(mDriverSeat);
    root[SEAT_PASSEN_TAG] = serializeOneSeat(mPassengerSeat);
    root[SEAT_SELECTION_TAG] = mCurrentSeatSelected;
    appendCookieToJson(root);
    Json::StyledWriter writer;
    return writer.write(root);
}

bool SeatState::deserialize(const std::string& newState)
{
    Json::Value root; // will contain the root value after parsing.
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(newState.c_str(), root);
    if (parsingSuccessful)
    {
        deserializeOneSeat(root.get(SEAT_DRIVER_TAG, root), mDriverSeat);
        deserializeOneSeat(root.get(SEAT_PASSEN_TAG, root), mPassengerSeat);
        mCurrentSeatSelected = (People)root.get(SEAT_SELECTION_TAG, DRIVER).asInt();
        setCookieFromJson(root);
    }
    return parsingSuccessful;
}

Json::Value SeatState::serializeOneSeat(const Seat toSerialize) const
{
    Json::Value toStore;
    toStore[SEAT_HEATER_TAG] = toSerialize.mHeater;
    toStore[SEAT_X_TAG] = toSerialize.mSeatX;
    toStore[SEAT_Y_TAG] = toSerialize.mSeatY;
    toStore[SEAT_BACK_ANGLE_TAG] = toSerialize.mBackAngle;
    return toStore;
}

void SeatState::deserializeOneSeat(const Json::Value toRetrieve, Seat& seat)
{
    seat.mHeater = toRetrieve.get(SEAT_HEATER_TAG, 0).asInt();
    seat.mBackAngle = toRetrieve.get(SEAT_BACK_ANGLE_TAG, 20).asInt();
    seat.mSeatY = toRetrieve.get(SEAT_Y_TAG, 100).asInt();
    seat.mSeatX = toRetrieve.get(SEAT_X_TAG, 100).asInt();
}

void SeatState::driverHeaterRequest()
{
    if (mDriverSeat.mHeater == SEAT_HEATER_MIN)
    {
        mDriverSeat.mHeater = SEAT_HEATER_MAX;
    } 
    else 
    {
        mDriverSeat.mHeater--;
    }
}   

void SeatState::passengerHeaterRequest()
{
    if (mPassengerSeat.mHeater == SEAT_HEATER_MIN)
    {
        mPassengerSeat.mHeater = SEAT_HEATER_MAX;
    } 
    else 
    {
        mPassengerSeat.mHeater--;
    }
}


void SeatState::driverSeatRequest()
{
    mCurrentSeatSelected = DRIVER;
}

void SeatState::passengerSeatRequest()
{
    mCurrentSeatSelected = PASSENGER;
}

int SeatState::getCurrentSeatHeater() const
{
    if(mCurrentSeatSelected == DRIVER)
    {
        return getDriverHeater();
    }
    return getPassengerHeater();
}

int SeatState::getCurrentSeatBackAngle() const
{
    if(mCurrentSeatSelected == DRIVER)
    {
        return getDriverSeatBackAngle();
    }
    return getPassengerSeatBackAngle();
}

int SeatState::getCurrentX() const
{
    if(mCurrentSeatSelected == DRIVER)
    {
        return getDriverX();
    }
    return getPassengerX();
}

int SeatState::getCurrentY() const
{
    if(mCurrentSeatSelected == DRIVER)
    {
        return getDriverY();
    }
    return getPassengerY();
}


void SeatState::setCurrentHeater(const int newValue)
{
    if(mCurrentSeatSelected == DRIVER)
    {
        setDriverHeater(newValue);
    }
    else 
    {
        setPassengerHeater(newValue);
    }
}

void SeatState::setCurrentX(const int newValue)
{
    if(mCurrentSeatSelected == DRIVER)
    {
        setDriverX(newValue);
    }
    else 
    {
        setPassengerX(newValue);
    }
}    

void SeatState::setCurrentY(const int newValue)
{
    if(mCurrentSeatSelected == DRIVER)
    {
        setDriverY(newValue);
    }
    else 
    {
        setPassengerY(newValue);
    }
}

void SeatState::setCurrentBackAngle(const int newValue)
{
    if(mCurrentSeatSelected == DRIVER)
    {
        setDriverBackAngle(newValue);
    }
    else 
    {
        setPassengerBackAngle(newValue);
    }
}

}
