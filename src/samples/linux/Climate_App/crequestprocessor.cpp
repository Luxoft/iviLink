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
#include "condcommon.h"

using namespace std;
using namespace std::tr1;
using namespace limitation_helpers;


CRequestProcessor::CRequestProcessor( shared_ptr<state_app> app  )
    : app(app)
{
    assert(app);
    init( saved_st );
}

void CRequestProcessor::tempDecDrRequest() // driver temperature decrease request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    st.set_driver_temp( dec_temperature( st.driver_temp() ) );
    app->update_state( st );
}

void CRequestProcessor::tempIncDrRequest() // driver temperature increase request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    st.set_driver_temp( inc_temperature( st.driver_temp() ) );
    app->update_state( st );
}

void CRequestProcessor::tempDecPassRequest() // passenger temperature decrease request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    st.set_pass_temp( dec_temperature( st.pass_temp() ) );
    app->update_state( st );
}

void CRequestProcessor::tempIncPassRequest()  // passenger temperature increase request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    st.set_pass_temp( inc_temperature( st.pass_temp() ) );
    app->update_state( st );
}

void CRequestProcessor::syncRequest()  // syncronizing temperature of driver and passenger
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    if( st.dual_climate() )
    {
      st.set_pass_temp( st.driver_temp() );
      st.set_dual_climate( false );
    }
    else
    {
      st.set_dual_climate( true );
      st.set_auto_climate( false );
    }
    app->update_state( st );
}

void CRequestProcessor::fanLowRequest()  // fan low request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    st.set_fan_mode_lows( !st.fan_mode_lows() );
    st.set_auto_climate( false );
    app->update_state( st );
}

void CRequestProcessor::fanMidRequest() // fan middle request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    st.set_fan_mode_middle( !st.fan_mode_middle() );
    st.set_auto_climate( false );
    app->update_state( st );
}

void CRequestProcessor::fanHighRequest()  // fan high request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    st.set_fan_mode_top( !st.fan_mode_top() );
    st.set_auto_climate( false );
    app->update_state( st );
}

void CRequestProcessor::fanIncRequest()  // fan increase requst
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    if( st.is_off() )
    {
        st = saved_st;
        st.set_fan_speed( 1 );
        st.set_is_off( false );
        st.set_auto_climate( false );
    }
    else
    {
      st.set_fan_speed( inc_fan_speed( st.fan_speed() ) );
      st.set_auto_climate( false );
    }
    app->update_state( st );
}

void CRequestProcessor::fanDecRequest()  // fan decrease request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    const int t= dec_fan_speed( st.fan_speed() );
    if( t != st.fan_speed() )
    {
      st.set_auto_climate( false );
      st.set_fan_speed( t );
    }
    app->update_state( st );
}

void CRequestProcessor::offRequest()  // off fan request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    if( !st.is_off() )
    {
        saved_st= st;
        st.set_air_recirc( false );
        st.set_auto_climate( false );
        st.set_fan_speed( 0 );
        st.set_front_window( false );
        st.set_rear_window( false );
        st.set_is_ac( false );
        st.set_is_off( true );
        st.set_fan_mode_lows( false );
        st.set_fan_mode_middle( false );
        st.set_fan_mode_top( false );
    }
    else
      st= saved_st;
    app->update_state( st );
}

void CRequestProcessor::rearRequest() // change rear mode request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    st.set_rear_window( !st.rear_window() );
    app->update_state( st );
}

void CRequestProcessor::frontRequest()  // change front mode request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    st.set_front_window( !st.front_window() );
    app->update_state( st );
}

void CRequestProcessor::acRequest() // change ac mode request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    st.set_is_ac(!st.is_ac());
    st.set_auto_climate( false );
    saved_st = st;
    app->update_state( st );
}

void CRequestProcessor::recircRequest()  // change recirculation mode request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    st.set_air_recirc( !st.air_recirc() );
    st.set_auto_climate( false );
    app->update_state( st );
}

void CRequestProcessor::autoRequest()  // change auto mode request
{
    if( !is_active(app) ) return;
    state_t st;
    app->retrive_state( st );
    if( !st.auto_climate() )
    {
      init( st );
      st.set_auto_climate( true );
      st.set_driver_temp( saved_st.driver_temp() );
      st.set_pass_temp( saved_st.pass_temp() );
    }
    else
      st.set_auto_climate( false );
    app->update_state( st );
}

void CRequestProcessor::initLocalRequest()  // initialization state request
{
    if( !is_active(app) ) return;
}

