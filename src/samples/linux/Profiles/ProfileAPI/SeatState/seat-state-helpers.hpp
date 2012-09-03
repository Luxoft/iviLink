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

#ifndef COMPARE_STATE_HPP
#define COMPARE_STATE_HPP

#include "generated/seat-state.pb.h"

inline bool less_by_time( const iviLink::SeatState& one, const iviLink::SeatState& two )
{
    if( one.has_timestamp() && two.has_timestamp() )
        return one.timestamp().utime() < two.timestamp().utime();
    return two.has_timestamp();
}

inline void init(iviLink::SeatState::Seat& seat)
{
    seat.set_bottom_x(seat.bottom_x());
    seat.set_bottom_y(seat.bottom_y());
    seat.set_back_x(seat.back_x());
    seat.set_back_y(seat.back_y());
    seat.set_back_angle(seat.back_angle());
    seat.set_heater(seat.heater());
}

inline void init(iviLink::Timestamp& ts)
{
    ts.set_utime( ts.utime() );
}

inline void init( iviLink::SeatState& st )
{
    init(*st.mutable_timestamp());
    init(*st.mutable_driver());
    init(*st.mutable_pass());
    st.set_current_seat(st.current_seat());
}


#endif // COMPARE_STATE_HPP
