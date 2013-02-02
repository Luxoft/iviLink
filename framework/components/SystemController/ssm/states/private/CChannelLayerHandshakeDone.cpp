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


#include "CSystemStateMachine.hpp"
#include "CComponentLauncher.hpp"
#include "SSMTriggerQueue.hpp"
#include "CChannelLayerHandshakeDone.hpp"
#include "CProfileManagerReady.hpp"

namespace iviLink
{
namespace SystemController
{

Logger CChannelLayerHandshakeDone::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.SystemStateMachine.states.CChannelLayerHandshakeDone"));
CChannelLayerHandshakeDone* CChannelLayerHandshakeDone::sInstance = 0;

CSystemState* CChannelLayerHandshakeDone::getInstance()
{
   if(0 == sInstance)
   {
	   sInstance = new CChannelLayerHandshakeDone;
   }

   return sInstance;
}


CChannelLayerHandshakeDone::CChannelLayerHandshakeDone()
{}

CChannelLayerHandshakeDone::~CChannelLayerHandshakeDone()
{}

void CChannelLayerHandshakeDone::launchProfileManager(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   //LOG4CPLUS_WARN(sLogger, "Application manager is launched here. Create specific method and probably state for this");
   //CComponentLauncher::getInstance()->launchApplicationManager();

#ifndef ANDROID
   CComponentLauncher::getInstance()->launchProfileManager();
   stateMachine->connectProfileManager();
#else
   stateMachine->connectProfileManager();
   iviLink::Android::makeRequest(iviLink::Android::eLaunchProfileManager);
#endif //ANDROID
}

void CChannelLayerHandshakeDone::onProfileManagerAvailable(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   changeState(stateMachine, CProfileManagerReady::getInstance());

   SSMTriggerQueue::getInstance()->sendTrigger(PROFILE_MANAGER_STARTED);
}

}
}

