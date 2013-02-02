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


#include <unistd.h>
#include <sys/param.h>

#include "AuthenticationStateMachine.hpp"
#include "Exit.hpp"
#include "InitialState.hpp"

namespace iviLink
{
namespace Authentication
{

Logger AuthenticationStateMachine::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("applications.AuthenticationApplication"));

AuthenticationStateMachine::AuthenticationStateMachine(const std::string& pathToTrlist, iviLink::Android::AppInfo appInfo)
    : iviLink::Application(iviLink::Service::Uid("AuthenticationService"), appInfo)
    , SystemControllerMsgProxy("SysCtrl_AuthApp")
    , mAppInfo(appInfo)
    , mKeygenThread(NULL)
    , mpState(NULL)
    , mInternalPath(pathToTrlist)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    
    mpAuthenticationProxy = new CAuthenticationProxy(iviLink::Service::Uid("AuthenticationService"), mAppInfo);
    mKeygenThread = new PrivateKeyGenerationThread(mpAuthenticationProxy);

    BaseError err = SystemControllerMsgProxy::connect();

    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(sLogger, "Unable to establish connection to system controller " + (std::string)err);
        killProcess(1); // Watchdog will detect this and restart the stack
    }
}

AuthenticationStateMachine::~AuthenticationStateMachine()
{
    delete mKeygenThread;
    delete mpAuthenticationProxy;
}

void AuthenticationStateMachine::startAuthentication(IAuthenticationDialog* gui)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    mGUI = gui;
    initInIVILink();
    changeState(InitialState::getInstance());
}

void AuthenticationStateMachine::changeState(AuthState* s)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(sLogger, "Change auth state " + (mpState? mpState->stateName() : "NULL") + " => " + s->stateName());

    mStateMutex.lock();
    mpState = s;
    mStateMutex.unlock();
}

void AuthenticationStateMachine::gotPIN(int first_digit, int second_digit, int third_digit, int fourth_digit)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    mRemotePIN.setPIN(first_digit, second_digit, third_digit, fourth_digit);
    mpState->pinReceived(this);
}

void AuthenticationStateMachine::sendPin(int first_digit, int second_digit, int third_digit, int fourth_digit)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    mLocalPIN.setPIN(first_digit, second_digit, third_digit, fourth_digit);
    mpAuthenticationProxy->sendPIN(first_digit, second_digit, third_digit, fourth_digit);

    mpState->pinSent(this);
}

void AuthenticationStateMachine::cancelAuthentication()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    SystemControllerMsgProxy::requestAuthenticationCanceled();
}

void AuthenticationStateMachine::onAuthenticationIsNotRequired()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    mpState->authenticationIsNotRequired(this);
    LOG4CPLUS_TRACE(Logger::getInstance(LOG4CPLUS_TEXT("profiler.ComponentIsOperable")), "AuthenticationApplication");
}

void AuthenticationStateMachine::onAuthenticationIsRequired()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    mGUI->showNumPad();
    LOG4CPLUS_TRACE(Logger::getInstance(LOG4CPLUS_TEXT("profiler.ComponentIsOperable")), "AuthenticationApplication");
}

void AuthenticationStateMachine::onProfileError()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    SystemControllerMsgProxy::requestAuthenticationError();
}

void AuthenticationStateMachine::onReadyToExit()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    mpState->readyToExit(this);
}

void AuthenticationStateMachine::comparePINs()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    if(mRemotePIN == mLocalPIN)
    {
        mpState->pinsEqual(this);
    }
    else
    {
        mLocalPIN.clear();
        mGUI->showRetryPopup();
        mGUI->resetNumPad();
        mGUI->unlockInput();
        mpState->pinsNotEqual(this);
    }
}

void AuthenticationStateMachine::onInitDone(iviLink::ELaunchInfo launcher)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

    if (iviLink::LAUNCHED_BY_USER == launcher)
    {
        LOG4CPLUS_INFO(sLogger, "started by user");
        registerProfileCallbacks(iviLink::Profile::ApiUid("AuthenticationProfile_PAPI_UID"), this);
        if(loadService(iviLink::Service::Uid("AuthenticationService")).isNoError())
        {
            LOG4CPLUS_INFO(sLogger, "started by user - service loaded!");
        }
        else
        {
            LOG4CPLUS_FATAL(sLogger, "could not load the service!");
            onProfileError();
        }
        mKeygenThread->start(); //generates keys and sends public key and uid to the other side
    }
    else
    {
        LOG4CPLUS_INFO(sLogger, "started by iviLink");
    }
}

void AuthenticationStateMachine::onIncomingServiceBeforeLoading(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    registerProfileCallbacks(iviLink::Profile::ApiUid("AuthenticationProfile_PAPI_UID"),this);
}

void AuthenticationStateMachine::onIncomingServiceAfterLoading(const iviLink::Service::Uid &service)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    mKeygenThread->start();
}

void AuthenticationStateMachine::onServiceDropped(iviLink::Service::Uid const& serviceUID)
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_INFO(sLogger, "Counter-service was dropped by counter-side");
    mpState->readyToExit(this);
}

void AuthenticationStateMachine::onRequestShutDown()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(sLogger, "Shutdown request from system controller - close Application");
    killProcess(0);
}

void AuthenticationStateMachine::onEnterInitialState()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
}

void AuthenticationStateMachine::onEnterPinSentState()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    mGUI->lockInput();
}

void AuthenticationStateMachine::onEnterPinReceivedState()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
}

void AuthenticationStateMachine::onEnterComparingPinsState()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
}

void AuthenticationStateMachine::onEnterAuthSuccessfullState()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    mGUI->hideNumPad();
    mpAuthenticationProxy->writeRemoteUIDToTrustList();
    if (getLaunchInfo() == iviLink::LAUNCHED_BY_USER)
    {
        mpAuthenticationProxy->readyToExit();
    }
}

void AuthenticationStateMachine::onEnterReadyToExitState()
{
    LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
    SystemControllerMsgProxy::requestAuthenticationOK();
}

}
}
