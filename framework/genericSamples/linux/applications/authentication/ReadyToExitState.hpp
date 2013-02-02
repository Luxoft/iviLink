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


#ifndef READYTOEXIT_HPP_
#define READYTOEXIT_HPP_

#include "AuthState.hpp"

namespace iviLink
{
namespace Authentication
{

class ReadyToExitState : public AuthState
{
public:
    static AuthState* getInstance();

    virtual std::string stateName() {return "ReadyToExitState";}
    virtual AuthID stateID() {return READY_TO_EXIT_STATE;}

    ReadyToExitState();
    virtual ~ReadyToExitState();

    virtual void pinSent(AuthenticationStateMachine* stateMachine)
    {
        forbiddenTrigger(stateMachine, "pinSent");
    }

    virtual void pinReceived(AuthenticationStateMachine* stateMachine)
    {
        forbiddenTrigger(stateMachine, "pinReceived");
    }

    virtual void pinsEqual(AuthenticationStateMachine* stateMachine)
    {
        forbiddenTrigger(stateMachine, "pinsEqual");
    }

    virtual void pinsNotEqual(AuthenticationStateMachine* stateMachine)
    {
        forbiddenTrigger(stateMachine, "pinsNotEqual");
    }

    virtual void authenticationIsNotRequired(AuthenticationStateMachine* stateMachine)
    {
        forbiddenTrigger(stateMachine, "authenticationIsNotRequired");
    }

    virtual void readyToExit(AuthenticationStateMachine* stateMachine){forbiddenTrigger(stateMachine, "readyToExit");}

private:

    static ReadyToExitState* sInstance;

};
}
}
#endif /* READYTOEXIT_HPP_ */
