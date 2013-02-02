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


#ifndef AUTHENTICATIONSTATEMACHINE_HPP_
#define AUTHENTICATIONSTATEMACHINE_HPP_

#include "Logger.hpp"
#include "Application.hpp"
#include "PINCode.hpp"
#include "CAuthenticationProxy.hpp"
#include "SystemControllerMsgProxy.hpp"
#include "IAuthenticationDialog.hpp"

#include "PrivateKeyGenerationThread.hpp"

namespace iviLink
{
namespace Authentication
{

class AuthState;

class AuthenticationStateMachine : private iviLink::Application
                                  , public IAuthenticationProfile_API::Callbacks
                                  , public AuthenticationAppMsgProtocol::SystemControllerMsgProxy
{

public:
    AuthenticationStateMachine(const std::string& pathToTrlist, iviLink::Android::AppInfo appInfo = iviLink::Android::AppInfo());

    virtual ~AuthenticationStateMachine();

    /**
     * Callback is called after initing of app
     * @param launcher gives information who launched app (user of iviLink)
     */
    virtual void onInitDone(iviLink::ELaunchInfo launcher);

    /**
     * Callback is called before loading incoming service
     * @param service is UID of service
    */
    virtual void onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service);

    /**
     * Callback is called after loading of incoming service
     */
    virtual void onIncomingServiceAfterLoading(const iviLink::Service::Uid &service);

    /**
     * Callback is called in case of and service session
     * @param service UID is uid of service
     */
    virtual void onServiceDropped(const iviLink::Service::Uid &service);

    // Calls from GUI
    void startAuthentication(IAuthenticationDialog* gui);

    void sendPin(int first_digit, int second_digit, int third_digit, int fourth_digit);
    void cancelAuthentication();

    void comparePINs();

    //Profile callbacks
    virtual void onReadyToExit();
    virtual void onRequestShutDown();
    virtual void gotPIN(int first_digit, int second_digit, int third_digit, int fourth_digit);
    virtual void onProfileError();
    virtual void onAuthenticationIsNotRequired();
    virtual void onAuthenticationIsRequired();

    virtual std::string getInternalPath()
    {
        return mInternalPath;
    }

    virtual void handleError(BaseError const & error)
    {
        LOG4CPLUS_INFO(sLogger,static_cast<std::string>(error));
    }

private:
    void showRetryPopup();
    void resetNumPad();

    //State Machine Events
    void onEnterInitialState();
    void onEnterPinSentState();
    void onEnterPinReceivedState();
    void onEnterComparingPinsState();
    void onEnterAuthSuccessfullState();
    void onEnterReadyToExitState();

    void changeState(AuthState* s);

private:
    iviLink::Android::AppInfo mAppInfo;
    static Logger sLogger;

    IAuthenticationDialog* mGUI;

    PINCode mLocalPIN;
    PINCode mRemotePIN;

    CAuthenticationProxy * mpAuthenticationProxy;
    PrivateKeyGenerationThread * mKeygenThread;

    AuthState* mpState;
    CMutex mStateMutex;

    std::string mInternalPath;

    friend class AuthState;
};

}
}
#endif /* AUTHENTICATIONSTATEMACHINE_HPP_ */
