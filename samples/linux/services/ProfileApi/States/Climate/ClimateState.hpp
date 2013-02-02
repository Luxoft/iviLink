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
#ifndef CLIMATE_STATE_HPP
#define CLIMATE_STATE_HPP

#include "State.hpp"

#define CLIMATE_MAX_TEMP 27
#define CLIMATE_MIN_TEMP 16
#define CLIMATE_MAX_FAN_SPEED 7
#define CLIMATE_MIN_FAN_SPEED 0

namespace iviLink 
{


class ClimateState : public State
{
public:
    ClimateState();
    virtual ~ClimateState() {} // stub for now
    
    virtual std::string serialize() const;
    virtual bool deserialize(const std::string& newState);

    bool isOff() const
    {
        return mOff; 
    }

    void setOff(const bool newOffState) 
    { 
        mOff = newOffState;
    }

    bool isAuto() const
    {
        return mAuto;
    }

    void setAuto(const bool newAutoState)
    {
        mAuto = newAutoState;
    }

    bool isRecirculation() const
    {
        return mRecirc;
    }

    void setRecirculation(const bool newRecirculationState)
    {
        mRecirc = newRecirculationState;
    }

    bool isAC() const
    {
        return mAC;
    }

    void setAC(const bool newACState)
    {
        mAC = newACState;
    }

    bool isFront() const
    {
        return mFront;
    }

    void setFront(const bool newFrontState)
    {
        mFront = newFrontState;
    }

    bool isRear() const
    {
        return mRear;
    }

    void setRear(const bool newRearState)
    {
        mRear = newRearState;
    }

    bool isFanHigh() const
    {
        return mFanHigh;
    }

    void setFanHigh(const bool newFanTopState)
    {
        mFanHigh = newFanTopState;
    }

    bool isFanMiddle() const
    {
        return mFanMiddle;
    }

    void setFanMiddle(const bool newFanMiddleState)
    {
        mFanMiddle = newFanMiddleState;
    }

    bool isFanLow() const
    {
        return mFanLow;
    }

    void setFanLow(const bool newFanLowState)
    {
        mFanLow = newFanLowState;
    }

    bool isSync() const
    {
        return mSync;
    }

    void setSync(const bool newSyncState)
    {
        mSync = newSyncState;
    }

    int getPassTemperature() const
    {
        return mPassTemp;
    }

    bool incrementPassTemperature();
    bool decrementPassTemperature();

    void setPassTemperature(const int newValue)
    {
        mPassTemp = newValue;
    }


    int getDriverTemperature() const
    {
        return mDrTemp;
    }

    bool incrementDriverTemperature();
    bool decrementDriverTemperature();

    void setDriverTemperature(const int newValue)
    {
        mDrTemp = newValue;
    }

    int getFanSpeed() const
    {
        return mFanSpd;
    }

    bool incrementFanSpeed();
    bool decrementFanSpeed();

    void setFanSpeed(const int newValue)
    {
        mFanSpd = newValue;
    }

private:
    bool mOff;
    bool mAuto;
    bool mRecirc;
    bool mAC;
    bool mFront;
    bool mRear;
    bool mFanHigh;
    bool mFanLow;
    bool mFanMiddle;
    bool mSync;

    int mPassTemp;
    int mDrTemp;
    int mFanSpd;
};
} // iviLink

#endif // CLIMATE_STATE_HPP
