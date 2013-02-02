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


#include <cassert>
#include "ClimateRequestProcessor.hpp"

#include "Exit.hpp"

using namespace std;
using namespace std::tr1;

ClimateRequestProcessor::ClimateRequestProcessor(climateAppPtr app)
    : app(app)
{
    assert(app);
}

void ClimateRequestProcessor::tempDecDrRequest() // driver temperature decrease request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    st.decrementDriverTemperature();
    if (st.isSync())
    {
        st.setPassTemperature(st.getDriverTemperature());
    }
    app->updateState(st);
}

void ClimateRequestProcessor::tempIncDrRequest() // driver temperature increase request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    st.incrementDriverTemperature();
    if (st.isSync())
    {
        st.setPassTemperature(st.getDriverTemperature());
    }
    app->updateState(st);
}

void ClimateRequestProcessor::tempDecPassRequest() // passenger temperature decrease request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    st.decrementPassTemperature();
    if (st.isSync())
    {
        st.setSync(false);
    }
    app->updateState(st);
}

void ClimateRequestProcessor::tempIncPassRequest()  // passenger temperature increase request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    st.incrementPassTemperature();
    if (st.isSync())
    {
        st.setSync(false);
    }
    app->updateState(st);
}

void ClimateRequestProcessor::syncRequest()  // syncronizing temperature of driver and passenger
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    if( !st.isSync() )
    {
      st.setPassTemperature(st.getDriverTemperature());
      st.setSync( true );
    }
    else
    {
      st.setSync(false);
      st.setAuto(false);
    }
    app->updateState(st);
}

void ClimateRequestProcessor::fanLowRequest()  // fan low request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    st.setFanLow(!st.isFanLow());
    st.setAuto(false);
    app->updateState(st);
}

void ClimateRequestProcessor::fanMidRequest() // fan middle request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    st.setFanMiddle( !st.isFanMiddle() );
    st.setAuto( false );
    app->updateState(st);
}

void ClimateRequestProcessor::fanHighRequest()  // fan high request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    st.setFanHigh( !st.isFanHigh() );
    st.setAuto( false );
    app->updateState(st);
}

void ClimateRequestProcessor::fanIncRequest()  // fan increase requst
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    if( st.isOff() )
    {
        saved_st.setCookie( st.getCookie() );
        st = saved_st;
        st.setFanSpeed( 1 );
        st.setOff( false );
        st.setAuto( false );
    }
    else
    {
      st.incrementFanSpeed();
      st.setAuto( false );
    }
    app->updateState(st);
}

void ClimateRequestProcessor::fanDecRequest()  // fan decrease request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    if( st.decrementFanSpeed())
    {
      st.setAuto( false );
    }
    app->updateState(st);
}

void ClimateRequestProcessor::offRequest()  // off fan request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    if( !st.isOff() )
    {
        saved_st= st;
        st.setRecirculation( false );
        st.setAuto( false );
        st.setFanSpeed( 0 );
        st.setFront( false );
        st.setRear( false );
        st.setAC( false );
        st.setOff( true );
        st.setFanLow( false );
        st.setFanMiddle( false );
        st.setFanHigh( false );
    }
    else
    {
      saved_st.setCookie( st.getCookie() );
      st= saved_st;
    }
    app->updateState(st);
}

void ClimateRequestProcessor::rearRequest() // change rear mode request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    st.setRear( !st.isRear() );
    app->updateState(st);
}

void ClimateRequestProcessor::frontRequest()  // change front mode request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    st.setFront( !st.isFront() );
    app->updateState(st);
}

void ClimateRequestProcessor::acRequest() // change ac mode request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    st.setAC(!st.isAC());
    st.setAuto( false );
    saved_st = st;
    app->updateState(st);
}

void ClimateRequestProcessor::recircRequest()  // change recirculation mode request
{
    if(!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    st.setRecirculation( !st.isRecirculation() );
    st.setAuto( false );
    app->updateState(st);
}

void ClimateRequestProcessor::autoRequest()  // change auto mode request
{
    if (!app->isActive()) return;
    state_t st;
    app->retrieveState(st);
    if (!st.isAuto())
    {
      state_t new_st = state_t();
      new_st.setCookie(st.getCookie());
      new_st.setAuto(true);
      new_st.setDriverTemperature (saved_st.getDriverTemperature());
      new_st.setPassTemperature(saved_st.getPassTemperature ());
      st= new_st;
    }
    else
    {
        st.setAuto(false);
    }
    app->updateState(st);
}

void ClimateRequestProcessor::initLocalRequest()  // initialization state request
{
    if(!app->isActive()) return;
}


void ClimateRequestProcessor::exitRequest()
{
    killProcess();
}

