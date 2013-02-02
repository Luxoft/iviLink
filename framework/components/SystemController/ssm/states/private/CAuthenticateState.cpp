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


#include "CAuthenticateState.hpp"
#include "CIdleState.hpp"
#include "CSystemStateMachine.hpp"
#include "CComponentLauncher.hpp"

namespace iviLink
{
namespace SystemController
{

Logger CAuthenticateState::sLogger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.SystemStateMachine.states.CAuthenticateState"));
CAuthenticateState* CAuthenticateState::sInstance = 0;

CSystemState* CAuthenticateState::getInstance()
{
   if(0 == sInstance)
   {
	   sInstance = new CAuthenticateState;
   }

   return sInstance;
}

CAuthenticateState::CAuthenticateState()
{

}

CAuthenticateState::~CAuthenticateState()
{

}

void CAuthenticateState::startAuthentication(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   if(stateMachine->authOn)
   {
      stateMachine->connectAuthenticationApp();

      if (stateMachine->mSystemGender == 1)
      {
#ifndef ANDROID
         CComponentLauncher::getInstance()->launchAuthenticationApp();
#else
         iviLink::Android::makeRequest(iviLink::Android::eStartAuth);
#endif //ANDROID
      }
   }
   else
   {
      LOG4CPLUS_INFO(sLogger, "noauth option requested --> Skip authentication");
      changeState(stateMachine, CIdleState::getInstance());
   }
}

void CAuthenticateState::finishAuthentication(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   changeState(stateMachine, CIdleState::getInstance());

   SSMTriggerQueue::getInstance()->sendTrigger(AUTHENTICATION_ESTABLISHED);
}

void CAuthenticateState::cancelAuthentication(CSystemStateMachine* stateMachine)
{
   LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);

   changeState(stateMachine, CIdleState::getInstance());

   SSMTriggerQueue::getInstance()->sendTrigger(AUTHENTICATION_CANCELED);
}

} /* namespace SystemController */
}
