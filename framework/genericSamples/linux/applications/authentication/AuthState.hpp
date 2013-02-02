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


#ifndef AUTHSTATE_HPP_
#define AUTHSTATE_HPP_

#include <string>
#include "Logger.hpp"
#include "AuthenticationStateMachine.hpp"

namespace iviLink
{

namespace Authentication
{

enum AuthID
{
    INITIAL_STATE,
    PIN_SENT_STATE,
    PIN_RECEIVED_STATE,
    COMPARING_PINS_STATE,
    AUTH_SUCCESSFULL_STATE,
    READY_TO_EXIT_STATE
};

class AuthState
{
public:
    AuthState();
    virtual ~AuthState();

    /**
     * Incoming trigger which is sent when local PIN was sent.
     * Should be implemented by specific state class
     * @param Pointer to client
     * @return void
     */
    virtual void pinSent(AuthenticationStateMachine* stateMachine) = 0;

    /**
     * Incoming trigger which is sent when remote PIN was received.
     * Should be implemented by specific state class
     * @param Pointer to client
     * @return void
     */
    virtual void pinReceived(AuthenticationStateMachine* stateMachine) = 0;

    /**
     * Incoming trigger which is sent when after remote and local triggers
     * had been compared and are equal.
     * Should be implemented by specific state class
     * @param Pointer to client
     * @return void
     */
    virtual void pinsEqual(AuthenticationStateMachine* stateMachine) = 0;

    /**
      * Incoming trigger which is sent when after remote and local triggers
      * had been compared and aren't equal.
      * Should be implemented by specific state class
      * @param Pointer to client
      * @return void
      */
    virtual void pinsNotEqual(AuthenticationStateMachine* stateMachine) = 0;

    /**
      * Incoming trigger which is sent when it is clear that authentication
      * is not needed for any reason
      * Should be implemented by specific state class
      * @param Pointer to client
      * @return void
      */
    virtual void authenticationIsNotRequired(AuthenticationStateMachine* stateMachine) = 0;

    /**
      * Incoming trigger which is sent when authentication was complete and
      * all the components are ready to be shut down
      * Should be implemented by specific state class
      * @param Pointer to client
      * @return void
      */
    virtual void readyToExit(AuthenticationStateMachine* stateMachine) = 0;

    /**
      * Returns the string name of the specific state
      * Should be implemented by specific state class
      * @return string
      */
    virtual std::string stateName() = 0;

    /**
      * Returns the ID of the specific state. It is used in @changeState method
      * to sent onEnter event to the client when state was changed.
      * Should be implemented by specific state class
      * @return AuthID (the enum which enumerates the IDs af all states)
      */
    virtual AuthID stateID() = 0;

protected:

    /** Normally should be called only by the specific state as a reaction on
      * incoming trigger and in order to change current state
      * @param Pointer to client
      * @param State the system should be switched to
      * @return void
      */
    void changeState(AuthenticationStateMachine* stateMachine, AuthState*  state);

    /** Normally should be called only by the specific state as a reaction on
      * incoming trigger which never should be called upon this state.
      * @param Pointer to client
      * @param string name of incoming trigger
      * @return void
      */
    void forbiddenTrigger(AuthenticationStateMachine* stateMachine, std::string trigger);

private:
    static Logger sLogger;
};

}
}
#endif /* AUTHSTATE_HPP_ */
