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
#ifndef SEAT_STATE_HPP
#define SEAT_STATE_HPP

#include "State.hpp"

#define SEAT_HEATER_MIN 0
#define SEAT_HEATER_MAX 3
#define SEAT_BACK_MIN_ANGLE (-40)
#define SEAT_BACK_MAX_ANGLE 70
#define SEAT_BACK_ANGLE_STEP 10
#define SEAT_STEP 25
#define SEAT_MIN_X 0
#define SEAT_MAX_X 200
#define SEAT_MIN_Y 0
#define SEAT_MAX_Y 200

namespace iviLink 
{

struct Seat 
{
    int mHeater;
    int mSeatX;
    int mSeatY;
    int mBackAngle;

    Seat() 
    {
        mHeater = 0;
        mSeatY = 100;
        mSeatX = 100;
        mBackAngle = 20;
    }
};

enum People
{
    DRIVER,
    PASSENGER,
};

class SeatState : public State
{
public:
    SeatState() : State()
    {
        mCurrentSeatSelected = DRIVER;
    }

    virtual ~SeatState() {}
    
    virtual std::string serialize() const;
    virtual bool deserialize(const std::string& newState);

    void driverHeaterRequest();

    void passengerHeaterRequest();

    void driverSeatRequest();

    void passengerSeatRequest();

    // getters section

    People getCurrentSelection() const 
    {
        return mCurrentSeatSelected;
    }

    int getDriverHeater() const
    {
        return mDriverSeat.mHeater;
    }

    int getPassengerHeater() const 
    {
        return mPassengerSeat.mHeater;
    }

    int getDriverSeatBackAngle() const
    {
        return mDriverSeat.mBackAngle;
    }

    int getPassengerSeatBackAngle() const 
    {
        return mPassengerSeat.mBackAngle;
    }

    int getDriverX() const
    {
        return mDriverSeat.mSeatX;
    }

    int getPassengerX() const 
    {
        return mPassengerSeat.mSeatX;
    }

    int getDriverY() const
    {
        return mDriverSeat.mSeatY;
    }

    int getPassengerY() const 
    {
        return mPassengerSeat.mSeatY;
    }
    
    // setters

    void setDriverHeater(const int newValue)
    {
        mDriverSeat.mHeater = newValue;
    }

    void setPassengerHeater(const int newValue)
    {
        mPassengerSeat.mHeater = newValue;
    }

    void setDriverX(const int newValue)
    {
        mDriverSeat.mSeatX = newValue;
    }

    void setPassengerX(const int newValue)
    {
        mPassengerSeat.mSeatX = newValue;
    }

    void setDriverY(const int newValue)
    {
        mDriverSeat.mSeatY = newValue;
    }

    void setPassengerY(const int newValue)
    {
        mPassengerSeat.mSeatY = newValue;
    }

    void setDriverBackAngle(const int newValue)
    {
        mDriverSeat.mBackAngle = newValue;
    }

    void setPassengerBackAngle(const int newValue)
    {
        mPassengerSeat.mBackAngle = newValue;
    }


    // getters with logic
    int getCurrentSeatHeater() const;
    int getCurrentSeatBackAngle() const;
    int getCurrentX() const;
    int getCurrentY() const;

    // setters with logic
    void setCurrentHeater(const int newValue);
    void setCurrentX(const int newValue);
    void setCurrentY(const int newValue);
    void setCurrentBackAngle(const int newValue);

private:
    // helpers for serialization/ deserialization
    Json::Value serializeOneSeat(const Seat seat) const;
    void deserializeOneSeat(const Json::Value toRetrieve, Seat& seat);

private:
    Seat mDriverSeat;
    Seat mPassengerSeat;
    People mCurrentSeatSelected;
};
}

#endif //SEAT_STATE_HPP
