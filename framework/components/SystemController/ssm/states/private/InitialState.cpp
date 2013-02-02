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


#include "CComponentLauncher.hpp"
#include "CSystemStateMachine.hpp"
#include "InitialState.hpp"
#include "WatchdogReady.hpp"

namespace iviLink
{
namespace SystemController
{

Logger InitialState::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.SystemStateMachine.states.InitialState"));
InitialState* InitialState::sInstance = 0;

CSystemState* InitialState::getInstance()
{
   if(0 == sInstance)
   {
       sInstance = new InitialState;
   }

   return sInstance;
}

InitialState::InitialState()
{}

InitialState::~InitialState()
{}

void InitialState::launchSystemWatchdog(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

#ifndef ANDROID
   CComponentLauncher::getInstance()->launchSystemControllerWatchdog();
   stateMachine->connectSystemWatchdog();
#else
   stateMachine->connectSystemWatchdog();
   iviLink::Android::makeRequest(iviLink::Android::eLaunchWatchdog);
#endif //ANDROID
}

void InitialState::onSystemWatchdogAvaliable(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   changeState(stateMachine, WatchdogReady::getInstance());

   SSMTriggerQueue::getInstance()->sendTrigger(SYSTEM_WATCHDOG_STARTED);
}

} /* namespace SystemController */
}
