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

/*
 * ChannelLayerHandshakeDone.cpp
 *
 *  Created on: May 14, 2012
 *      Author: Kramchaninov Pavel
 */

#include "framework/components/SystemController/ssm/stateMachine/include/CTriggerQueue.hpp"
#include "framework/components/SystemController/ssm/states/include/CChannelLayerHandshakeDone.hpp"
#include "framework/components/SystemController/ssm/states/include/CSuccessfulAuth.hpp"

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

void CChannelLayerHandshakeDone::startAuthentication(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE(sLogger, "startAuthentication()");
   //TODO: perform authentication scenario using AuthenticationManager

   //TODO: As authentication scenario will become more complex
   // provide specific States and Events such as
   // onPinRequired()
   // onAuthenticationFailed()
   // and so on.

   changeState(stateMachine, CSuccessfulAuth::getInstance());

   CTriggerQueue::sendTrigger(AUTHENTICATION_ESTABLISHED);
}

}

