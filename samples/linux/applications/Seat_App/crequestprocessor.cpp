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
#include "crequestprocessor.h"
#include "seat_coords.hpp"

using namespace std;
using namespace std::tr1;

typedef ISeatSenderProfile::state_t state_t;

CRequestProcessor::CRequestProcessor(seatAppPtr app)
    : app(app)
{
    assert(app);
}

// change driver heater state request
void CRequestProcessor::driverHeaterRequest()
{
    if(!app->isActive()) 
    {
        return;
    }

    state_t st;
    app->retrieveState(st);
    st.driverHeaterRequest();
    app->updateState(st);
}

// change passenger heater state request
void CRequestProcessor::passHeaterRequest()
{
    if(!app->isActive()) 
    {
        return;
    }

    state_t st;
    app->retrieveState(st);
    st.passengerHeaterRequest();
    app->updateState(st);
}

// change visible seat state to driver request
void CRequestProcessor::driverSeatRequest()
{
    if(!app->isActive()) 
    {
        return;
    }

    state_t st;
    app->retrieveState(st);
    st.driverSeatRequest();
    app->updateState(st);
}

// change visible seat state to passenger request
void CRequestProcessor::passSeatRequest()
{
    if(!app->isActive()) 
    {
        return;
    }

    state_t st;
    app->retrieveState(st);
    st.passengerSeatRequest();
    app->updateState(st);
}

// move seatback request
void CRequestProcessor::backLeftRequest()
{
    if(!app->isActive()) 
    {
        return;
    }

    state_t st;
    app->retrieveState(st);
    const int newAngle= st.getCurrentSeatBackAngle() - SEAT_BACK_ANGLE_STEP;
    if(newAngle > SEAT_BACK_MIN_ANGLE)
    {
        st.setCurrentBackAngle(newAngle);
        app->updateState(st);
    }
}

// move seatback request
void CRequestProcessor::backRightRequest()
{
    if(!app->isActive()) 
    {
        return;
    }

    state_t st;
    app->retrieveState(st);
    const int newAngle= st.getCurrentSeatBackAngle() + SEAT_BACK_ANGLE_STEP;
    if(newAngle < SEAT_BACK_MAX_ANGLE)
    {
        st.setCurrentBackAngle(newAngle);
        app->updateState(st);
    }
}

// move seat request
void CRequestProcessor::bottomUpRequest()
{
    if(!app->isActive()) 
    {
        return;
    }

    state_t st;
    app->retrieveState(st);
    st.setCurrentY(normalizeRy(st.getCurrentY() - SEAT_STEP));
    app->updateState(st);
}

// move seat request
void CRequestProcessor::bottomDownRequest()
{
    if(!app->isActive()) 
    {
        return;
    }

    state_t st;
    app->retrieveState(st);
    st.setCurrentY(normalizeRy(st.getCurrentY() + SEAT_STEP));
    app->updateState(st);
}

// move seat request
void CRequestProcessor::bottomLeftRequest()
{
    if(!app->isActive()) 
    {
        return;
    }
    state_t st;
    app->retrieveState(st);
    st.setCurrentX(normalizeRx(st.getCurrentX() - SEAT_STEP));
    app->updateState(st);
}

// move seat request
void CRequestProcessor::bottomRightRequest()
{
    if(!app->isActive()) 
    {
        return;
    }

    state_t st;
    app->retrieveState(st);
    st.setCurrentX(normalizeRx(st.getCurrentX() + SEAT_STEP));
    app->updateState(st);
}

// initializing state request
void CRequestProcessor::initRequest()
{       
    LOG4CPLUS_TRACE_METHOD(log4StateApp(), __PRETTY_FUNCTION__);   
    state_t st;
    app->retrieveState(st);
    app->updateState(st);
    return;
}

void CRequestProcessor::exitRequest()
{
    killProcess();
}

