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






#include "framework/components/SystemController/ssm/states/CApplicationManagerReady.hpp"
#include "framework/components/SystemController/ssm/states/CAuthenticateState.hpp"
#include "framework/components/SystemController/ssm/states/CProfileManagerHandshakeDone.hpp"
#include "framework/components/SystemController/ssm/stateMachine/CSystemStateMachine.hpp"
#include "framework/components/SystemController/ssm/stateMachine/CTriggerQueue.hpp"

namespace SystemController
{

Logger CApplicationManagerReady::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.SystemStateMachine.states.CApplicationManagerReady"));
CApplicationManagerReady* CApplicationManagerReady::sInstance = 0;

CSystemState* CApplicationManagerReady::getInstance()
{
   if(0 == sInstance)
   {
	   sInstance = new CApplicationManagerReady;
   }

   return sInstance;
}

CApplicationManagerReady::CApplicationManagerReady()
{

}

CApplicationManagerReady::~CApplicationManagerReady()
{

}

void CApplicationManagerReady::onApplicationManagerHandshakeDone(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE(sLogger, "onApplicationManagerHandshakeDone()");

   changeState(stateMachine, CAuthenticateState::getInstance());

   CTriggerQueue::sendTrigger(APPLICATION_MANAGER_CONNECTION_ESTABLISHED);
}

} /* namespace SystemController */
