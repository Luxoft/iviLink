/* 
 * 
 * iviLINK SDK, version 1.1.2
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










#include "framework/components/SystemController/componentLauncher/CComponentLauncher.hpp"
#include "framework/components/SystemController/ssm/stateMachine/CTriggerQueue.hpp"
#include "framework/components/SystemController/ssm/stateMachine/CSystemStateMachine.hpp"
#include "CInitialState.hpp"
#include "CPhysicalLayerReady.hpp"

namespace SystemController
{

Logger CInitialState::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.SystemStateMachine.states.CInitialState"));
CInitialState* CInitialState::sInstance = 0;

CSystemState* CInitialState::getInstance()
{
   if(0 == sInstance)
   {
	   sInstance = new CInitialState;
   }

   return sInstance;
}

CInitialState::CInitialState()
{}

CInitialState::~CInitialState()
{}

void CInitialState::launchPhysicalLayer(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE(sLogger, "launchPhysicalLayer()");

   #ifndef ANDROID
   CComponentLauncher::getInstance()->launchConnectivityAgent();
   stateMachine->connectConnectivityAgent();
   #else
   stateMachine->connectConnectivityAgent();
   iviLink::Android::makeRequest(iviLink::Android::eLaunchAgent);
   #endif //ANDROID
}

void CInitialState::onConnectivityAgentAvailable(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE(sLogger, "onConnectivityAgentAvailable()");

   changeState(stateMachine, CPhysicalLayerReady::getInstance());

   CTriggerQueue::sendTrigger(CONNECTIVITY_AGENT_STARTED);
}

} /* namespace SystemController */
