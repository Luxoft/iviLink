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
#include "ClimateState.hpp"

#define OFF_TAG "off"
#define AUTO_TAG "auto"
#define RECIRC_TAG "recirculation"
#define AC_TAG "ac"
#define FRONT_TAG "front"
#define REAR_TAG "rear"
#define FAN_HIGH_TAG "fan high"
#define FAN_MID_TAG "fan middle"
#define FAN_LOW_TAG "fan low"
#define SYNC_TAG "sync"
#define PASS_TEMP_TAG "passenger temperature"
#define DRIVER_TEMP_TAG "driver temperature"
#define FAN_SPEED_TAG "fan speed"

namespace iviLink {

ClimateState::ClimateState()
:
    State()
{
    mOff = false;
    mAuto = false;
    mRecirc = false;
    mAC = true;
    mFront = false;
    mRear = false;
    mFanHigh = false;
    mFanLow = false;
    mFanMiddle = false;
    mSync = false;

    mPassTemp = 20;
    mDrTemp = 20;
    mFanSpd = 5;
}


std::string ClimateState::serialize() const
{
    Json::Value root;
    root[OFF_TAG] = mOff;
    root[AUTO_TAG] = mAuto;
    root[RECIRC_TAG] = mRecirc;
    root[AC_TAG] = mAC;
    root[FRONT_TAG] = mFront;
    root[REAR_TAG] = mRear;
    root[FAN_HIGH_TAG] = mFanHigh;
    root[FAN_MID_TAG] = mFanMiddle;
    root[FAN_LOW_TAG] = mFanLow;
    root[SYNC_TAG] = mSync;
    root[PASS_TEMP_TAG] = mPassTemp;
    root[DRIVER_TEMP_TAG] = mDrTemp;
    root[FAN_SPEED_TAG] = mFanSpd;
    appendCookieToJson(root);
    Json::StyledWriter writer;
    return writer.write(root);
}

bool ClimateState::deserialize(const std::string& newState)
{
    Json::Value root; // will contain the root value after parsing.
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(newState.c_str(), root);
    if (parsingSuccessful)
    {
        mOff = root.get(OFF_TAG, false).asBool();
        mAuto = root.get(AUTO_TAG, false).asBool();
        mRecirc = root.get(RECIRC_TAG, false).asBool();
        mAC = root.get(AC_TAG, true).asBool();
        mFront = root.get(FRONT_TAG, false).asBool();
        mRear = root.get(REAR_TAG, false).asBool();
        mFanHigh = root.get(FAN_HIGH_TAG, false).asBool();
        mFanMiddle = root.get(FAN_MID_TAG, false).asBool();
        mFanLow = root.get(FAN_LOW_TAG, false).asBool();
        mSync = root.get(SYNC_TAG, false).asBool();

        mPassTemp = root.get(PASS_TEMP_TAG, 20).asInt();
        mDrTemp = root.get(DRIVER_TEMP_TAG, 20).asInt();
        mFanSpd = root.get(FAN_SPEED_TAG, 5).asInt();
        setCookieFromJson(root);
    }
    return parsingSuccessful;
}

bool ClimateState::incrementPassTemperature()
{
    if (mPassTemp < CLIMATE_MAX_TEMP)
    {
        mPassTemp++;
        return true;
    }
    return false;
}

bool ClimateState::decrementPassTemperature()
{
    if (mPassTemp > CLIMATE_MIN_TEMP)
    {
        mPassTemp--;
        return true;
    }
    return false;
}

bool ClimateState::incrementDriverTemperature()
{
    if (mDrTemp < CLIMATE_MAX_TEMP)
    {
        mDrTemp++;
        return true;
    }
    return false;
}

bool ClimateState::decrementDriverTemperature()
{
    if (mDrTemp > CLIMATE_MIN_TEMP)
    {
        mDrTemp--;
        return true;
    }
    return false;
}

bool ClimateState::incrementFanSpeed()
{
    if (mFanSpd < CLIMATE_MAX_FAN_SPEED)
    {
        mFanSpd++;
        return true;
    }
    return false;
}

bool ClimateState::decrementFanSpeed()
{
    if (mFanSpd > CLIMATE_MIN_FAN_SPEED)
    {
        mFanSpd--;
        return true;
    }
    return false;
}

} // namespace iviLink
