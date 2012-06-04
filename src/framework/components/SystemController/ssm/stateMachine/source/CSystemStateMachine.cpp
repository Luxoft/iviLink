/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */





#include "framework/components/SystemController/ssm/stateMachine/include/CSystemStateMachine.hpp"
#include "framework/components/SystemController/ssm/stateMachine/include/CTriggerQueue.hpp"
#include "framework/components/SystemController/ssm/states/include/CSystemState.hpp"
#include "framework/components/SystemController/ssm/states/include/CInitialState.hpp"

using namespace SystemControllerMsgProtocol;

namespace SystemController
{

Logger CSystemStateMachine::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.SystemStateMachine.CSystemStateMachine"));

void CSystemStateMachine::init(int systemGender)
{
   mSystemGender = systemGender;

   // Start listening to incoming triggers
   CThread::start();

   // Set initial StateMachine State
   mSystemState = CInitialState::getInstance();

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
         mSystemState->startAuthentication(this);
         break;
      case CHANNEL_CONNECTION_LOST:
         LOG4CPLUS_WARN(sLogger, "CHANNEL_CONNECTION_LOST trigger");
         break;
      case AUTHENTICATION_ESTABLISHED:
         LOG4CPLUS_INFO(sLogger, "AUTHENTICATION_ESTABLISHED trigger");
         mSystemState->launchProfileManager(this);
         break;
      case PROFILE_MANAGER_STARTED:
         LOG4CPLUS_INFO(sLogger, "PROFILE_MANAGER_STARTED trigger");
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

CError CSystemStateMachine::onCounterCAConnected()
{
   mSystemState->onConnectivityAgentHandshakeDone(this);

   return CError(CError::NO_ERROR, "SystemStateMachine");
}

CError CSystemStateMachine::onCounterCSConnected()
{
   mSystemState->onChannelSupervisorHandshakeDone(this);

   return CError(CError::NO_ERROR, "SystemStateMachine");
}

CSystemStateMachine::CSystemStateMachine()
   : CThread("CSystemStateMachine")
   , ConnectivityAgentMsgProxy("SysCtr_ConAgnt")
   , ChannelSupervisorMsgProxy("SysCtr_ChSprvsr")
   , ProfileManagerMsgProxy("SysCtrl_PrflMngr")
   , AppManMsgProxy("SysCtrl_AppMan")
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
