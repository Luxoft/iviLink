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



#ifndef CONDCOMMON_H
#define CONDCOMMON_H

#include <algorithm>

struct condState
{
    int  fanSpeed;
    int  driverTemp;
    int  passTemp;
    bool autoClimate;
    bool dualClimate;  // ! Sync on/off
    bool airRecirc;
    bool isAC;
    bool frontWindow;
    bool rearWindow;
    bool isOFF;
    bool fanModeTop;
    bool fanModeMiddle;
    bool fanModeLow;

    condState() :  fanSpeed(5), driverTemp(20), passTemp(20), autoClimate(true), dualClimate(false), airRecirc(false),
        isAC(true), frontWindow(false), rearWindow(false), isOFF(false), fanModeTop(false), fanModeMiddle(false), fanModeLow(false){}

};

inline bool operator==( const condState& a, const condState& b)
{
    return
               a.fanSpeed      == b.fanSpeed
            && a.driverTemp    == b.driverTemp
            && a.passTemp      == b.passTemp
            && a.autoClimate   == b.autoClimate
            && a.dualClimate   == b.dualClimate
            && a.airRecirc     == b.airRecirc
            && a.frontWindow   == b.frontWindow
            && a.isAC          == b.isAC
            && a.isOFF         == b.isOFF
            && a.rearWindow    == b.rearWindow
            && a.fanModeLow    == b.fanModeLow
            && a.fanModeMiddle == b.fanModeMiddle
            && a.fanModeTop    == b.fanModeTop
            ;
}

inline bool operator!=( const condState& a, const condState& b)
{
    return !( a == b );
}


namespace limitation_helpers {

    static const int
      FAN_MAX_SPEED = 7,
      FAN_MIN_SPEED = 0,
      TEMP_MIN      = 16,
      TEMP_MAX      = 27;

    template<typename T, typename U>
    inline T limit_value( T v, U min_v, U max_v )
    {
        return std::max( min_v, std::min( max_v, v ) );
    }

    inline int limit_temperature( int value )
    {
        return limit_value( value, TEMP_MIN, TEMP_MAX );
    }

    inline int limit_fan_speed( int value )
    {
        return limit_value( value, FAN_MIN_SPEED, FAN_MAX_SPEED );
    }

    template<typename F, typename L>
    inline bool change_value( int& v, F f, L l )
    {
        const int new_v = l( f( v ) );
        if( new_v != v )
        {
            v = new_v;
            return true;
        }
        return false;
    }

    inline int inc( int v ) { return v+1; }
    inline int dec( int v ) { return v-1; }

    inline bool inc_temperature( int& temp ) { return change_value( temp, inc, limit_temperature ); }
    inline bool dec_temperature( int& temp ) { return change_value( temp, dec, limit_temperature ); }
    inline bool inc_fan_speed( int& speed )  { return change_value( speed, inc, limit_fan_speed ); }
    inline bool dec_fan_speed( int& speed )  { return change_value( speed, dec, limit_fan_speed ); }

} // namespace limitation_helpers

#endif // CONDCOMMON_H
