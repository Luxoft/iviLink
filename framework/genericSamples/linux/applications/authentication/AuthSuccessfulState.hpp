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


#ifndef AUTHSUCCESSFUL_HPP_
#define AUTHSUCCESSFUL_HPP_

#include "AuthState.hpp"

namespace iviLink
{
namespace Authentication
{

class AuthSuccessfulState : public AuthState
{
private:
    static Logger sLogger;

public:
    static AuthState* getInstance();

    virtual std::string stateName() {return "AuthSuccessfulState";}
    virtual AuthID stateID() {return AUTH_SUCCESSFULL_STATE;}

protected:
    AuthSuccessfulState();
    virtual ~AuthSuccessfulState();

    virtual void pinSent(AuthenticationStateMachine* stateMachine){};
    virtual void pinReceived(AuthenticationStateMachine* stateMachine){};
    virtual void pinsEqual(AuthenticationStateMachine* stateMachine){};
    virtual void pinsNotEqual(AuthenticationStateMachine* stateMachine){};
    virtual void authenticationIsNotRequired(AuthenticationStateMachine* stateMachine)
    {
        forbiddenTrigger(stateMachine, "authenticationIsNotRequired");
    }

    virtual void readyToExit(AuthenticationStateMachine* stateMachine);

private:

    static AuthSuccessfulState* sInstance;

};

}
}
#endif /* AUTHSUCCESSFUL_HPP_ */
