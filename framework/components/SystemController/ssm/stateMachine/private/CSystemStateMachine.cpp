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


#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "CSystemStateMachine.hpp"
#include "SSMTriggerQueue.hpp"
#include "CSystemState.hpp"
#include "InitialState.hpp"
#include "CIdleState.hpp"
#include "TcpSocket.hpp"
#include "reset.hpp"
#include "SystemControllerWatchdogCommon.hpp"
#include "CComponentLauncher.hpp"

#ifndef ANDROID
#else
#include "MakeRequest.hpp"
#endif //ANDROID

void sendStopSignalToProgressBar();

using namespace SystemControllerMsgProtocol;
namespace iviLink
{
namespace SystemController
{

Logger CSystemStateMachine::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.SystemStateMachine.CSystemStateMachine"));
bool CSystemStateMachine::authOn = true;

void CSystemStateMachine::init(bool authOn)
{
   CSystemStateMachine::authOn = authOn;
   LOG4CPLUS_TRACE(sLogger, "init() " + string("Auth is ") + (CSystemStateMachine::authOn ? "ON" : "OFF"));

   // Start listening to incoming triggers
   // Set initial StateMachine State
   mSystemState = InitialState::getInstance();

   SSMTriggerQueue::getInstance()->resetTriggerQueue();
   SSMTriggerQueue::getInstance()->sendTrigger(INITIALIZED);

   CThread::start();
}

void CSystemStateMachine::startTriggerHandler()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(sLogger, "Trigger handler is now running...");

   eSSMTriggers trigger;

   while(true)
   {
      LOG4CPLUS_INFO(sLogger, "read trigger");

      trigger = SSMTriggerQueue::getInstance()->receiveTrigger();

      switch(trigger)
      {
      case EMPTY_TRIGGER_QUEUE:
         LOG4CPLUS_INFO(sLogger, "No incoming triggers");
         break;
      case INITIALIZED:
         LOG4CPLUS_INFO(sLogger, "INITIALIZED trigger");
         mSystemState->launchSystemWatchdog(this);
         break;
      case SYSTEM_WATCHDOG_STARTED:
         LOG4CPLUS_INFO(sLogger, "SYSTEM_WATCHDOG_STARTED trigger");
 		   mSystemState->launchApplicationManager(this);    
         SystemControllerWatchdogProxy::onApplicationLayerStarted();
         break;
     case APPLICATION_MANAGER_STARTED:
         LOG4CPLUS_INFO(sLogger, "APPLICATION_MANAGER_STARTED trigger");
         break;
      case APPLICATION_MANAGER_CONNECTION_ESTABLISHED:
         LOG4CPLUS_INFO(sLogger, "APPLICATION_MANAGER_CONNECTION_ESTABLISHED trigger");
         mSystemState->launchPhysicalLayer(this);
         SystemControllerWatchdogProxy::onPhysicalLayerStarted();
         break;
      case CONNECTIVITY_AGENT_STARTED:
         LOG4CPLUS_INFO(sLogger, "CONNECTIVITY_AGENT_STARTED trigger");
         break;
      case PHISYCAL_CONNECTION_ESTABLISHED:
         mSystemState->launchChannelLayer(this);
         LOG4CPLUS_INFO(sLogger, "PHISYCAL_CONNECTION_ESTABLISHED trigger");
         SystemControllerWatchdogProxy::onChannelLayerStarted();
         break;
      case PHISYCAL_CONNECTION_LOST:
         LOG4CPLUS_WARN(sLogger, "PHISYCAL_CONNECTION_LOST trigger");
         break;
      case CHANNEL_SUPERVISOR_STARTED:
         LOG4CPLUS_INFO(sLogger, "CHANNEL_SUPERVISOR_STARTED trigger");
         break;
      case CHANNEL_CONNECTION_ESTABLISHED:
         LOG4CPLUS_INFO(sLogger, "CHANNEL_CONNECTION_ESTABLISHED trigger");
         mSystemState->launchProfileManager(this);
         SystemControllerWatchdogProxy::onProfileLayerStarted();
         break;
      case CHANNEL_CONNECTION_LOST:
         LOG4CPLUS_WARN(sLogger, "CHANNEL_CONNECTION_LOST trigger");
         break;
      case PROFILE_MANAGER_STARTED:
         LOG4CPLUS_INFO(sLogger, "PROFILE_MANAGER_STARTED trigger");
         break;
      case PROFILE_MANAGER_CONNECTION_ESTABLISHED:
         LOG4CPLUS_INFO(sLogger, "PROFILE_MANAGER_CONNECTION_ESTABLISHED trigger");
         if(authOn)
         {
            unlockAuthenticationProfile();
            mSystemState->startAuthentication(this);
            SystemControllerWatchdogProxy::onAuthenticationStarted();
         }
         else
         {
            mSystemState->finishAuthentication(this);
         }
         break;
      case AUTHENTICATION_ESTABLISHED:
         LOG4CPLUS_INFO(sLogger, "AUTHENTICATION_ESTABLISHED trigger");
         LOG4CPLUS_INFO(sLogger, "Unlock all profiles and switch to idle state");
         unlockProfiles();
         AuthenticationAppMsgProxy::requestShutDown();
         SystemControllerWatchdogProxy::onAuthenticationStopped();
#ifndef ANDROID
         sendStopSignalToProgressBar();
#else
         iviLink::Android::makeRequest(iviLink::Android::eIdle);
#endif //ANDROID
         linkIsUp= true;
         LOG4CPLUS_INFO(sLogger, "====================");
         LOG4CPLUS_INFO(sLogger, "LINK UP NOTIFICATION");
         LOG4CPLUS_INFO(sLogger, "====================");

         linkUpNotification();
         break;
      case AUTHENTICATION_CANCELED:
         AuthenticationAppMsgProxy::requestShutDown();
         doReset();
         break;
      }
   }
}

void CSystemStateMachine::onConnectivityAgentAvailable()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(sLogger, "OK!!! ConnectivityAgent started and ready for communication");

   LOG4CPLUS_TRACE(Logger::getInstance(LOG4CPLUS_TEXT("profiler.ComponentIsOperable")), "IVILinkConnectivityAgent");

   mSystemState->onConnectivityAgentAvailable(this);
}

void CSystemStateMachine::onChannelSupervisorAvailable()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(sLogger, "OK!!! ChannelSupervisor started and ready for communication");

   LOG4CPLUS_TRACE(Logger::getInstance(LOG4CPLUS_TEXT("profiler.ComponentIsOperable")), "IVILinkNegotiator");

   mSystemState->onChannelSupervisorAvailable(this);
}

void CSystemStateMachine::onProfileManagerAvailable()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(sLogger, "OK!!! ProfileManager started and ready for communication");

   LOG4CPLUS_TRACE(Logger::getInstance(LOG4CPLUS_TEXT("profiler.ComponentIsOperable")), "IVILinkProfileManager");

   mSystemState->onProfileManagerAvailable(this);
}

void CSystemStateMachine::onAppManAvailable()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(sLogger, "OK!!! ApplicationManager started and ready for communication");

   LOG4CPLUS_TRACE(Logger::getInstance(LOG4CPLUS_TEXT("profiler.ComponentIsOperable")), "IVILinkApplicationManager");

   mSystemState->onApplicationManagerAvailable(this);
}

void CSystemStateMachine::onAuthenticationAppAvailable()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(sLogger, "OK!!! AuthenticationApp  started and ready for communication");
}

BaseError CSystemStateMachine::onCounterCAConnected(UInt8 role)
{
   LOG4CPLUS_TRACE(sLogger, "onCounterCAConnected(" + convertIntegerToString(role) + ")");

   mSystemGender = role;

   mSystemState->onConnectivityAgentHandshakeDone(this);

   return BaseError(BaseError::IVILINK_NO_ERROR, "SystemStateMachine");
}

void CSystemStateMachine::onSystemControllerWatchdogAvaliable()
{
   LOG4CPLUS_TRACE(sLogger, __PRETTY_FUNCTION__);
   mSystemState->onSystemWatchdogAvaliable(this);
}

void CSystemStateMachine::onSystemControllerWatchdogNotAvaliable()
{
   LOG4CPLUS_TRACE(sLogger, __PRETTY_FUNCTION__);
#ifndef ANDROID
   // if we make reset here, and the cause of the crash was an external reset, 
   // stacks may enter an endless loop of restarting and killing each other
   // TODO fixit
#else
   // probably it has crashed/ been shut down by Android OS, trying to reset
   // if it has shut down as a part of an already ongoing reset, nothing bad will happen
   doReset();
#endif //ANDROID
}

BaseError CSystemStateMachine::onCounterCADisconnected()
{
   LOG4CPLUS_TRACE(sLogger, __PRETTY_FUNCTION__);

   linkIsUp= false;
   linkDownNotification();

   doReset();

   return BaseError(BaseError::IVILINK_NO_ERROR, "SystemStateMachine");
}


BaseError CSystemStateMachine::onCounterCSConnected()
{
   mSystemState->onChannelSupervisorHandshakeDone(this);

   return BaseError(BaseError::IVILINK_NO_ERROR, "SystemStateMachine");
}

BaseError CSystemStateMachine::onCounterPMConnected()
{
   mSystemState->onProfileManagerHandshakeDone(this);

   return BaseError(BaseError::IVILINK_NO_ERROR, "SystemStateMachine");
}

BaseError CSystemStateMachine::onCounterAMConnected()
{
   mSystemState->onApplicationManagerHandshakeDone(this);

   return BaseError(BaseError::IVILINK_NO_ERROR, "SystemStateMachine");
}

void CSystemStateMachine::onAuthenticationOK()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   mSystemState->finishAuthentication(this);
}

void CSystemStateMachine::onAuthenticationNOK()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(sLogger, "stub");
}

void CSystemStateMachine::onAuthenticationCanceled()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   mSystemState->cancelAuthentication(this);
}

void CSystemStateMachine::onAuthenticationError()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   doReset();
}

CSystemStateMachine::CSystemStateMachine()
   : CThread("CSystemStateMachine")
   , ConnectivityAgentMsgProxy("SysCtr_ConAgnt")
   , ChannelSupervisorMsgProxy("SysCtr_ChSprvsr")
   , ProfileManagerMsgProxy("SysCtrl_PrflMngr")
   , AppManMsgProxy("SysCtrl_AppMan")
   , AuthenticationAppMsgProxy("SysCtrl_AuthApp")
   , SystemControllerWatchdogProxy("SysCtrl_Watchdog")
   , linkIsUp( false )
{}

CSystemStateMachine::~CSystemStateMachine()
{}

void CSystemStateMachine::changeState(CSystemState* state)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   LOG4CPLUS_INFO(sLogger, "Change state " + mSystemState->stateName() + " => " + state->stateName());

   mSystemState = state;
}

bool CSystemStateMachine::getLinkState()
{
   return linkIsUp;
}

void CSystemStateMachine::doReset()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   static CMutex mutex;
   MutexLocker lock(mutex);
   static bool isResetInProgress = false;
   if (!isResetInProgress)
   {
      isResetInProgress = true;
#ifndef ANDROID
      hardReset(true);
#else
      iviLink::Android::makeRequest(iviLink::Android::eReset);
#endif //ANDROID
   }

}
 
} /* namespace SystemController */
} // namespace iviLink

void sendStopSignalToProgressBar()
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.StateMachine.socket"));
   TcpSocket tcpSocketAuth(SPLASH_SCREEN_PORT);
   tcpSocketAuth.initClient();
   tcpSocketAuth.sendMessage((UInt8)stopMessage);
   tcpSocketAuth.closeSocket();
}


