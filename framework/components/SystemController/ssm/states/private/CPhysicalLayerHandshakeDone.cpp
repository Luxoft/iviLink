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


#include "CPhysicalLayerHandshakeDone.hpp"
#include "CChannelLayerReady.hpp"
#include "CComponentLauncher.hpp"
#include "CSystemStateMachine.hpp"

namespace iviLink
{
namespace SystemController
{

Logger CPhysicalLayerHandshakeDone::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.SystemStateMachine.states.CPhysicalLayerHandshakeDone"));
CPhysicalLayerHandshakeDone* CPhysicalLayerHandshakeDone::sInstance = 0;

CSystemState* CPhysicalLayerHandshakeDone::getInstance()
{
   if(0 == sInstance)
   {
	   sInstance = new CPhysicalLayerHandshakeDone;
   }

   return sInstance;
}

CPhysicalLayerHandshakeDone::CPhysicalLayerHandshakeDone()
{}

CPhysicalLayerHandshakeDone::~CPhysicalLayerHandshakeDone()
{}

void CPhysicalLayerHandshakeDone::launchChannelLayer(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

#ifndef ANDROID
   CComponentLauncher::getInstance()->launchChannelSupervisor();
   stateMachine->connectChannelSupervisor();
#else 
   stateMachine->connectChannelSupervisor();
   iviLink::Android::makeRequest(iviLink::Android::eLaunchNegotiator);
#endif //ANDROID
}

void CPhysicalLayerHandshakeDone::onChannelSupervisorAvailable(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   changeState(stateMachine, CChannelLayerReady::getInstance());

   SSMTriggerQueue::getInstance()->sendTrigger(CHANNEL_SUPERVISOR_STARTED);
}

} /* namespace SystemController */
}
