/* 
 * 
 * iviLINK SDK, version 1.0.1
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









#include "CSystemStateMachine.hpp"
#include "CTriggerQueue.hpp"
#include "framework/components/SystemController/ssm/states/CSystemState.hpp"
#include "framework/components/SystemController/ssm/states/CInitialState.hpp"
#include "framework/components/SystemController/ssm/states/CIdleState.hpp"
#include "framework/components/SystemController/processEntryPoint/reset.hpp"

using namespace SystemControllerMsgProtocol;

namespace SystemController
{

Logger CSystemStateMachine::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.SystemStateMachine.CSystemStateMachine"));
bool CSystemStateMachine::authOn = true;

void CSystemStateMachine::init(bool authOn)
{
   CSystemStateMachine::authOn = authOn;
   LOG4CPLUS_TRACE(sLogger, "init() " + string("Auth is ") + (CSystemStateMachine::authOn ? "ON" : "OFF"));

   // Start listening to incoming triggers
   CThread::start();

   // Set initial StateMachine State
   mSystemState = CInitialState::getInstance();


   CTriggerQueue::resetTriggerQueue();
   CTriggerQueue::sendTrigger(INITIALIZED);
}

void CSystemStateMachine::startTriggerHandler()
{
   LOG4CPLUS_TRACE(sLogger, "startTriggerHandler()");

   LOG4CPLUS_INFO(sLogger, "Trigger handler is now running...");

   eSSMTriggers trigger;

   while(true)
   {
      LOG4CPLUS_INFO(sLogger, "read trigger");

      trigger = CTriggerQueue::receiveTrigger();

      switch(trigger)
      {
      case EMPTY_TRIGGER_QUEUE:
         LOG4CPLUS_INFO(sLogger, "No incoming triggers");
         break;
      case INITIALIZED:
         LOG4CPLUS_INFO(sLogger, "INITIALIZED trigger");
         mSystemState->launchPhysicalLayer(this);
         break;
      case CONNECTIVITY_AGENT_STARTED:
         LOG4CPLUS_INFO(sLogger, "CONNECTIVITY_AGENT_STARTED trigger");
         break;
      case PHISYCAL_CONNECTION_ESTABLISHED:
         LOG4CPLUS_INFO(sLogger, "PHISYCAL_CONNECTION_ESTABLISHED trigger");
         mSystemState->launchChannelLayer(this);
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
         break;
      case CHANNEL_CONNECTION_LOST:
         LOG4CPLUS_WARN(sLogger, "CHANNEL_CONNECTION_LOST trigger");
         break;
      case PROFILE_MANAGER_STARTED:
         LOG4CPLUS_INFO(sLogger, "PROFILE_MANAGER_STARTED trigger");
         break;
      case PROFILE_MANAGER_CONNECTION_ESTABLISHED:
         LOG4CPLUS_INFO(sLogger, "PROFILE_MANAGER_CONNECTION_ESTABLISHED trigger");
         mSystemState->launchApplicationManager(this);
         break;
      case APPLICATION_MANAGER_STARTED:
         LOG4CPLUS_INFO(sLogger, "APPLICATION_MANAGER_STARTED trigger");
         break;
      case APPLICATION_MANAGER_CONNECTION_ESTABLISHED:
         LOG4CPLUS_INFO(sLogger, "APPLICATION_MANAGER_CONNECTION_ESTABLISHED trigger");
         if(authOn)
         {
            unlockAuthenticationProfile();
            mSystemState->startAuthentication(this);
         }
         else
         {
            mSystemState->finishAuthentication(this);
         }
         break;
      case AUTHENTICATION_ESTABLISHED:
         LOG4CPLUS_INFO(sLogger, "AUTHENTICATION_ESTABLISHED trigger");
         LOG4CPLUS_WARN(sLogger, "Unlock all profiles and switch to idle state");
         unlockProfiles();
         AuthenticationAppMsgProxy::requestShutDown();
         break;
      case AUTHENTICATION_CANCELED:
         AuthenticationAppMsgProxy::requestShutDown();
         hardReset(true);
         break;
      }
   }
}

void CSystemStateMachine::onConnectivityAgentAvailable()
{
   LOG4CPLUS_TRACE(sLogger, "onConnectivityAgentAvailable()");
   LOG4CPLUS_INFO(sLogger, "OK!!! ConnectivityAgent started and ready for communication");

   mSystemState->onConnectivityAgentAvailable(this);
}

void CSystemStateMachine::onChannelSupervisorAvailable()
{
   LOG4CPLUS_TRACE(sLogger, "onChannelSulpervisorAvailable()");
   LOG4CPLUS_INFO(sLogger, "OK!!! ChannelSupervisor started and ready for communication");

   mSystemState->onChannelSupervisorAvailable(this);
}

void CSystemStateMachine::onProfileManagerAvailable()
{
   LOG4CPLUS_TRACE(sLogger, "onProfileManagerAvailable()");
   LOG4CPLUS_INFO(sLogger, "OK!!! ProfileManager started and ready for communication");

   mSystemState->onProfileManagerAvailable(this);
}

void CSystemStateMachine::onAppManAvailable()
{
   LOG4CPLUS_TRACE(sLogger, "onAppManAvailable()");
   LOG4CPLUS_INFO(sLogger, "OK!!! ApplicationManager started and ready for communication");

   mSystemState->onApplicationManagerAvailable(this);
}

void CSystemStateMachine::onAuthenticationAppAvailable()
{
   LOG4CPLUS_TRACE(sLogger, "onAuthenticationAppAvailable()");
   LOG4CPLUS_INFO(sLogger, "OK!!! AuthenticationApp  started and ready for communication");
}

CError CSystemStateMachine::onCounterCAConnected(UInt8 role)
{
   LOG4CPLUS_TRACE(sLogger, "onCounterCAConnected(" + convertIntegerToString(role) + ")");

   mSystemGender = role;

   mSystemState->onConnectivityAgentHandshakeDone(this);

   return CError(CError::NO_ERROR, "SystemStateMachine");
}

CError CSystemStateMachine::onCounterCADisconnected()
{
   LOG4CPLUS_TRACE(sLogger, __PRETTY_FUNCTION__);

   hardReset(true);
   
   return CError(CError::NO_ERROR, "SystemStateMachine");
}


CError CSystemStateMachine::onCounterCSConnected()
{
   mSystemState->onChannelSupervisorHandshakeDone(this);

   return CError(CError::NO_ERROR, "SystemStateMachine");
}

CError CSystemStateMachine::onCounterPMConnected()
{
   mSystemState->onProfileManagerHandshakeDone(this);

   return CError(CError::NO_ERROR, "SystemStateMachine");
}

CError CSystemStateMachine::onCounterAMConnected()
{
   mSystemState->onApplicationManagerHandshakeDone(this);

   return CError(CError::NO_ERROR, "SystemStateMachine");
}

CError CSystemStateMachine::onAuthenticationOK()
{
   LOG4CPLUS_TRACE(sLogger, "onAuthenticationOK()");

   mSystemState->finishAuthentication(this);

   return CError(CError::NO_ERROR, "SystemStateMachine");
}

CError CSystemStateMachine::onAuthenticationNOK()
{
   LOG4CPLUS_TRACE(sLogger, "onAuthenticationNOK()");

   return CError(CError::NO_ERROR, "SystemStateMachine");
}
CError CSystemStateMachine::onAuthenticationCanceled()
{
   LOG4CPLUS_TRACE(sLogger, "onAuthenticationCanceled()");

   mSystemState->cancelAuthentication(this);

   return CError(CError::NO_ERROR, "SystemStateMachine");
}

CSystemStateMachine::CSystemStateMachine()
   : CThread("CSystemStateMachine")
   , ConnectivityAgentMsgProxy("SysCtr_ConAgnt")
   , ChannelSupervisorMsgProxy("SysCtr_ChSprvsr")
   , ProfileManagerMsgProxy("SysCtrl_PrflMngr")
   , AppManMsgProxy("SysCtrl_AppMan")
   , AuthenticationAppMsgProxy("SysCtrl_AuthApp")
{}

CSystemStateMachine::~CSystemStateMachine()
{}

void CSystemStateMachine::changeState(CSystemState* state)
{
   LOG4CPLUS_TRACE(sLogger, "ChangeState()");

   LOG4CPLUS_INFO(sLogger, "Change state " + mSystemState->stateName() + " => " + state->stateName());

   mSystemState = state;
}

} /* namespace SystemController */
