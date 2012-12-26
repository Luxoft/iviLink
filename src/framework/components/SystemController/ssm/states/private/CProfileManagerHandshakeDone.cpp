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
 

#include "CProfileManagerHandshakeDone.hpp"
#include "CApplicationManagerReady.hpp"
#include "CChannelLayerReady.hpp"
#include "CComponentLauncher.hpp"
#include "CSystemStateMachine.hpp"
#include "CTriggerQueue.hpp"

namespace SystemController
{

Logger CProfileManagerHandshakeDone::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.SystemStateMachine.states.CProfileManagerHandshakeDone"));
CProfileManagerHandshakeDone* CProfileManagerHandshakeDone::sInstance = 0;

CSystemState* CProfileManagerHandshakeDone::getInstance()
{
   if(0 == sInstance)
   {
	   sInstance = new CProfileManagerHandshakeDone;
   }

   return sInstance;
}

CProfileManagerHandshakeDone::CProfileManagerHandshakeDone()
{}

CProfileManagerHandshakeDone::~CProfileManagerHandshakeDone()
{}

void CProfileManagerHandshakeDone::launchApplicationManager(CSystemStateMachine* stateMachine)
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

void CProfileManagerHandshakeDone::onApplicationManagerAvailable(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   changeState(stateMachine, CApplicationManagerReady::getInstance());

   CTriggerQueue::sendTrigger(APPLICATION_MANAGER_STARTED);
}

} /* namespace SystemController */
