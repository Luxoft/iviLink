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

#ifndef COMPARE_CL_STATE_HPP
#define COMPARE_CL_STATE_HPP

#include "generated/climate-state.pb.h"

inline bool less_by_time( const iviLink::ClimateState& one, const iviLink::ClimateState& two )
{
    if( one.has_timestamp() && two.has_timestamp() )
        return one.timestamp().utime() < two.timestamp().utime();
    return two.has_timestamp();
}

inline void init(iviLink::Timestamp& ts)
{
    ts.set_utime( ts.utime() );
}

inline void init( iviLink::ClimateState& st )
{
    init( *st.mutable_timestamp() );

    st. set_fan_speed       ( st. fan_speed       () );
    st. set_driver_temp     ( st. driver_temp     () );
    st. set_pass_temp       ( st. pass_temp       () );
    st. set_auto_climate    ( st. auto_climate    () );
    st. set_dual_climate    ( st. dual_climate    () );
    st. set_air_recirc      ( st. air_recirc      () );
    st. set_is_ac           ( st. is_ac           () );
    st. set_front_window    ( st. front_window    () );
    st. set_rear_window     ( st. rear_window     () );
    st. set_is_off          ( st. is_off          () );
    st. set_fan_mode_top    ( st. fan_mode_top    () );
    st. set_fan_mode_middle ( st. fan_mode_middle () );
    st. set_fan_mode_lows   ( st. fan_mode_lows   () );
}


#endif // COMPARE_CL_STATE_HPP
