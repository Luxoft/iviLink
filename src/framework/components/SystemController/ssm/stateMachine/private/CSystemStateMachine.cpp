/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "CSystemStateMachine.hpp"
#include "CTriggerQueue.hpp"
#include "CSystemState.hpp"
#include "CInitialState.hpp"
#include "CIdleState.hpp"
#include "reset.hpp"
#include "CComponentLauncher.hpp"

#ifndef ANDROID
#else
#include "MakeRequest.hpp"
#endif //ANDROID

void sendStopSignalToProgressBar();

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
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

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
         mSystemState->launchChannelLayer(this);
         LOG4CPLUS_INFO(sLogger, "PHISYCAL_CONNECTION_ESTABLISHED trigger");
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
         LOG4CPLUS_INFO(sLogger, "Unlock all profiles and switch to idle state");
         unlockProfiles();
         AuthenticationAppMsgProxy::requestShutDown();
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
#ifndef ANDROID
         hardReset(true);         
#else
         iviLink::Android::makeRequest(iviLink::Android::eReset);        
#endif //ANDROID
         break;
      }
   }
}

void CSystemStateMachine::onConnectivityAgentAvailable()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(sLogger, "OK!!! ConnectivityAgent started and ready for communication");

   mSystemState->onConnectivityAgentAvailable(this);
}

void CSystemStateMachine::onChannelSupervisorAvailable()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(sLogger, "OK!!! ChannelSupervisor started and ready for communication");

   mSystemState->onChannelSupervisorAvailable(this);
}

void CSystemStateMachine::onProfileManagerAvailable()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(sLogger, "OK!!! ProfileManager started and ready for communication");

   mSystemState->onProfileManagerAvailable(this);
}

void CSystemStateMachine::onAppManAvailable()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(sLogger, "OK!!! ApplicationManager started and ready for communication");

   mSystemState->onApplicationManagerAvailable(this);
}

void CSystemStateMachine::onAuthenticationAppAvailable()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
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

   linkIsUp= false;
   linkDownNotification();

#ifndef ANDROID
   hardReset(true);
#else
   iviLink::Android::makeRequest(iviLink::Android::eReset);
#endif //ANDROID

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
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   mSystemState->finishAuthentication(this);

   return CError(CError::NO_ERROR, "SystemStateMachine");
}

CError CSystemStateMachine::onAuthenticationNOK()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   return CError(CError::NO_ERROR, "SystemStateMachine");
}
CError CSystemStateMachine::onAuthenticationCanceled()
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

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
 
}
/* namespace SystemController */

void sendStopSignalToProgressBar()
{
   Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.StateMachine.socket"));
   int clientSocket;  
   struct sockaddr_in addr;
   UInt8 stopMessage[1] = {1};
        
   clientSocket = socket(AF_INET, SOCK_STREAM, 0);
   if(clientSocket < 0)
   {
      LOG4CPLUS_ERROR(logger, "socket create error");
   }
   addr.sin_family = AF_INET;
   addr.sin_port = htons(3425);
   addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
   
   if(connect(clientSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
   {
      LOG4CPLUS_ERROR(logger, "socket connection error");/*log4cplus Msg*/
   }

   send(clientSocket, stopMessage, sizeof(stopMessage), 0);
   close(clientSocket);
}


