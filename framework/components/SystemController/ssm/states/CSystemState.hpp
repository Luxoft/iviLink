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


#ifndef SYSTEMSTATE_HPP_
#define SYSTEMSTATE_HPP_

#include <string>

#include "ConnectivityAgentMsgProxy.hpp"
#include "ChannelSupervisorMsgProxy.hpp"
#include "ProfileManagerMsgProxy.hpp"
#include "SSMTriggerQueue.hpp"

#ifndef ANDROID
#else
#include "MakeRequest.hpp"
#endif //ANDROID

namespace iviLink
{
namespace SystemController
{

class CSystemStateMachine;

class CSystemState
{
public:

   CSystemState();
   virtual ~CSystemState();

   virtual std::string stateName() = 0;

   virtual void launchSystemWatchdog(CSystemStateMachine* stateMachine){};
   virtual void launchPhysicalLayer(CSystemStateMachine* stateMachine){};
   virtual void launchChannelLayer(CSystemStateMachine* stateMachine){};
   virtual void launchProfileManager(CSystemStateMachine* stateMachine){};
   virtual void launchApplicationManager(CSystemStateMachine* stateMachine){};
   virtual void startAuthentication(CSystemStateMachine* stateMachine){};
   virtual void finishAuthentication(CSystemStateMachine* stateMachine){};
   virtual void cancelAuthentication(CSystemStateMachine* stateMachine){};

   virtual void onConnectivityAgentAvailable(CSystemStateMachine* stateMachine){};
   virtual void onConnectivityAgentHandshakeDone(CSystemStateMachine* stateMachine){};

   virtual void onChannelSupervisorAvailable(CSystemStateMachine* stateMachine){};
   virtual void onChannelSupervisorHandshakeDone(CSystemStateMachine* stateMachine){};

   virtual void onProfileManagerAvailable(CSystemStateMachine* stateMachine){};
   virtual void onProfileManagerHandshakeDone(CSystemStateMachine* stateMachine){};

   virtual void onApplicationManagerAvailable(CSystemStateMachine* stateMachine){};
   virtual void onApplicationManagerHandshakeDone(CSystemStateMachine* stateMachine){};

   virtual void onSystemWatchdogAvaliable(CSystemStateMachine* stateMachine){};

protected:

   void changeState(CSystemStateMachine* stateMachine, CSystemState*  state);
};

} /* namespace SystemController */
} // namespace iviLink
#endif /* SYSTEMSTATE_HPP_ */
