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
#include "WatchdogReady.hpp"
#include "CPhysicalLayerReady.hpp"
#include "CApplicationManagerReady.hpp"

namespace iviLink
{
namespace SystemController
{

Logger WatchdogReady::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.SystemStateMachine.states.WatchdogReady"));
WatchdogReady* WatchdogReady::sInstance = 0;

CSystemState* WatchdogReady::getInstance()
{
   if(0 == sInstance)
   {
	   sInstance = new WatchdogReady;
   }

   return sInstance;
}

WatchdogReady::WatchdogReady()
{}

WatchdogReady::~WatchdogReady()
{}

void WatchdogReady::launchApplicationManager(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

#ifndef ANDROID
   CComponentLauncher::getInstance()->launchApplicationManager();
   stateMachine->connectApplicationManager();
#else
   stateMachine->connectApplicationManager();
   iviLink::Android::makeRequest(iviLink::Android::eLaunchApplicationManager);
#endif //ANDROID
}

void WatchdogReady::onApplicationManagerAvailable(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   changeState(stateMachine, CApplicationManagerReady::getInstance());

   SSMTriggerQueue::getInstance()->sendTrigger(APPLICATION_MANAGER_STARTED);
}

} /* namespace SystemController */
}
