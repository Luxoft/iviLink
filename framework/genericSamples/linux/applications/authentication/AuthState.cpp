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


#include "AuthState.hpp"

namespace iviLink
{
namespace Authentication
{

Logger AuthState::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("applications.AuthenticationApplication"));

AuthState::AuthState()
{
}

AuthState::~AuthState()
{
}

void AuthState::changeState(AuthenticationStateMachine* stateMachine, AuthState*  state)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    stateMachine->changeState(state);

    switch (state->stateID())
    {
    case INITIAL_STATE:
        stateMachine->onEnterInitialState();
        break;
    case PIN_SENT_STATE:
        stateMachine->onEnterPinSentState();
        break;
     case PIN_RECEIVED_STATE:
        stateMachine->onEnterPinReceivedState();
        break;
    case COMPARING_PINS_STATE:
        stateMachine->onEnterComparingPinsState();
        break;
    case AUTH_SUCCESSFULL_STATE:
        stateMachine->onEnterAuthSuccessfullState();
        break;
    case READY_TO_EXIT_STATE:
        stateMachine->onEnterReadyToExitState();
        break;
    }
}

void AuthState::forbiddenTrigger(AuthenticationStateMachine* stateMachine, std::string trigger)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    LOG4CPLUS_ERROR(sLogger, "Trigger " + trigger + " is forbidden for state " +  stateMachine->mpState->stateName());
}

}
}
