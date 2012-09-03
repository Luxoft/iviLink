/* 
 * 
 * iviLINK SDK, version 1.1.2
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
#include "../Profiles/ProfileAPI/ClimateState/climate-state-helpers.hpp"

namespace limitation_helpers {

    static const int
        FAN_MAX_SPEED = iviLink::ClimateState::FAN_MAX_SPEED,
        FAN_MIN_SPEED = iviLink::ClimateState::FAN_MIN_SPEED,
        TEMP_MIN      = iviLink::ClimateState::TEMP_MIN,
        TEMP_MAX      = iviLink::ClimateState::TEMP_MAX;

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
    inline int change_value( int v, F f, L l )
    {
      return l( f( v ) );
    }

    inline int inc( int v ) { return v+1; }
    inline int dec( int v ) { return v-1; }

    inline int inc_temperature( int temp ) { return change_value( temp, inc, limit_temperature ); }
    inline int dec_temperature( int temp ) { return change_value( temp, dec, limit_temperature ); }
    inline int inc_fan_speed( int speed )  { return change_value( speed, inc, limit_fan_speed ); }
    inline int dec_fan_speed( int speed )  { return change_value( speed, dec, limit_fan_speed ); }

} // namespace limitation_helpers

#endif // CONDCOMMON_H
