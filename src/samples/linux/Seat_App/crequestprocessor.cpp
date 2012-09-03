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





//#include <ctime>
#include <cassert>
#include "crequestprocessor.h"

using namespace std;
using namespace std::tr1;

typedef ISeatSenderProfile::state_t state_t;

namespace {
    state_t::Seat& cur_seat( state_t& st )
    {
        return st.current_seat() == state_t::DRIVER ?
            *st.mutable_driver() :
            *st.mutable_pass();
    }
}


CRequestProcessor::CRequestProcessor( shared_ptr<state_app> app  )
    : app(app)
{
    assert(app);
}

// change driver heater state request
void CRequestProcessor::driverHeaterRequest()
{
    if( !is_active(app) ) return;

    state_t st;
    app->retrive_state( st );
    int speed = st.driver().heater();
    --speed;
    if( speed < state_t::HEATER_MIN )
        speed = state_t::HEATER_MAX;
    st.mutable_driver()->set_heater(speed);
    app->update_state( st );
}

// change passenger heater state request
void CRequestProcessor::passHeaterRequest()
{
    if( !is_active(app) ) return;

    state_t st;
    app->retrive_state( st );
    int speed = st.pass().heater();
    --speed;
    if( speed < state_t::HEATER_MIN )
        speed = state_t::HEATER_MAX;
    st.mutable_pass()->set_heater(speed);
    app->update_state( st );
}

// change visible seat state to driver request
void CRequestProcessor::driverSeatRequest()
{
    if( !is_active(app) ) return;

    state_t st;
    app->retrive_state( st );
    st.set_current_seat(state_t::DRIVER);
    app->update_state( st );
}

// change visible seat state to passenger request
void CRequestProcessor::passSeatRequest()
{
    if( !is_active(app) ) return;

    state_t st;
    app->retrive_state( st );
    st.set_current_seat(state_t::PASS);
    app->update_state( st );
}

// move seatback request
void CRequestProcessor::backLeftRequest()
{
    if( !is_active(app) ) return;

    state_t st;
    app->retrive_state( st );
    state_t::Seat& seat = cur_seat( st );
    const int new_angle= seat.back_angle() - state_t::ANGLE_STEP;
    if( new_angle > state_t::MIN_ANGLE )
    {
        seat.set_back_angle( new_angle );
        app->update_state( st );
    }
}

// move seatback request
void CRequestProcessor::backRightRequest()
{
    if( !is_active(app) ) return;

    state_t st;
    app->retrive_state( st );
    state_t::Seat& seat = cur_seat( st );
    const int new_angle= seat.back_angle() + state_t::ANGLE_STEP;
    if( new_angle < state_t::MAX_ANGLE )
    {
        seat.set_back_angle( new_angle );
        app->update_state( st );
    }
}

// move seat request
void CRequestProcessor::bottomUpRequest()
{
    if( !is_active(app) ) return;

    state_t st;
    app->retrive_state( st );
    state_t::Seat& seat = cur_seat( st );

    const int new_bottom_y = seat.bottom_y() - state_t::SEAT_STEP;
    if( new_bottom_y > state_t::LIM_BOTTOM_UP )
    {
        seat.set_bottom_y( new_bottom_y );
        seat.set_back_y( seat.back_y() - state_t::SEAT_STEP );
        app->update_state( st );
    }
}

// move seat request
void CRequestProcessor::bottomDownRequest()
{
    if( !is_active(app) ) return;

    state_t st;
    app->retrive_state( st );
    state_t::Seat& seat = cur_seat( st );

    const int new_bottom_y = seat.bottom_y() + state_t::SEAT_STEP;
    if( new_bottom_y < state_t::LIM_BOTTOM_DOWN )
    {
        seat.set_bottom_y( new_bottom_y );
        seat.set_back_y( seat.back_y() + state_t::SEAT_STEP );
        app->update_state( st );
    }
}

// move seat request
void CRequestProcessor::bottomLeftRequest()
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    state_t::Seat& seat = cur_seat( st );

    const int new_bottom_x = seat.bottom_x() - state_t::SEAT_STEP;
    if( new_bottom_x > state_t::LIM_BOTTOM_LEFT )
    {
        seat.set_bottom_x( new_bottom_x );
        seat.set_back_x( seat.back_x() - state_t::SEAT_STEP );
        app->update_state( st );
    }
}

// move seat request
void CRequestProcessor::bottomRightRequest()
{
    if( !is_active(app) ) return;

    state_t st;
    app->retrive_state( st );
    state_t::Seat& seat = cur_seat( st );

    const int new_bottom_x = seat.bottom_x() + state_t::SEAT_STEP;
    if( new_bottom_x < state_t::LIM_BOTTOM_RIGHT )
    {
        seat.set_bottom_x( new_bottom_x );
        seat.set_back_x( seat.back_x() + state_t::SEAT_STEP );
        app->update_state( st );
    }
}

// initializing state request
void CRequestProcessor::initRequest()
{
    return;
    //if( !is_active(app) ) return;
}


