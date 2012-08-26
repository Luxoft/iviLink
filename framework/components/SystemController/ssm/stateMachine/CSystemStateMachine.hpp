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









#ifndef SYSTEMSTATEMACHINE_HPP
#define SYSTEMSTATEMACHINE_HPP

#include "utils/threads/CThread.hpp"
#include "framework/messageProtocol/SystemController_ConnectivityAgent/SystemController/ConnectivityAgentMsgProxy.hpp"
#include "framework/messageProtocol/SystemController_ChannelSupervisor/SystemController/ChannelSupervisorMsgProxy.hpp"
#include "framework/messageProtocol/SystemController_ProfileManager/SystemController/ProfileManagerMsgProxy.hpp"
#include "framework/messageProtocol/SystemController_AppMan/SystemController/AppManMsgProxy.hpp"
#include "framework/messageProtocol/SystemController_Authentication/SystemController/AuthenticationAppMsgProxy.hpp"

#include "utils/misc/Logger.hpp"

namespace SystemController
{

class CSystemState;

class CSystemStateMachine : public CThread
                          , public SystemControllerMsgProtocol::ConnectivityAgentMsgProxy
                          , public SystemControllerMsgProtocol::ChannelSupervisorMsgProxy
                          , public SystemControllerMsgProtocol::ProfileManagerMsgProxy
                          , public SystemControllerMsgProtocol::AppManMsgProxy
                          , public SystemControllerMsgProtocol::AuthenticationAppMsgProxy
{
private:
   static Logger sLogger;
public:
   static bool authOn;

public:
   int mSystemGender;

   void init(bool authOn);

   CSystemStateMachine();
   virtual ~CSystemStateMachine();

   void launchPhysicalLayer();
   void launchChannelLayer();
   void startAuthentication();
   void launchProfileManager();

   void unlockAuthenticationProfile()
   {
      ProfileManagerMsgProxy::requestUnlockAuthenticationProfile();
   }

   void unlockProfiles()
   {
      ProfileManagerMsgProxy::requestUnlockProfiles();
   }

   void connectConnectivityAgent()
   {
      ConnectivityAgentMsgProxy::connectConnectivityAgent();
   }

   void connectChannelSupervisor()
   {
      ChannelSupervisorMsgProxy::connectChannelSupervisor();
   }

   void connectProfileManager()
   {
	   ProfileManagerMsgProxy::connectProfileManager();
   }

   void connectApplicationManager()
   {
	   AppManMsgProxy::connectAppMan();
   }

   void connectAuthenticationApp()
   {
	   AuthenticationAppMsgProxy::connectAuthenticationApp();
   }


   void shutDownProfileManager()
   {
      ProfileManagerMsgProxy::requestShutDown();
   }

private:

   virtual CError onCounterCAConnected(UInt8 role);
   virtual CError onCounterCADisconnected();

   virtual CError onCounterCSConnected();
   virtual CError onCounterCSDisconnected(){return CError(CError::NO_ERROR, "CSystemStateMachine");}

   virtual CError onCounterPMConnected();
   virtual CError onCounterPMDisconnected(){return CError(CError::NO_ERROR, "CSystemStateMachine");}

   virtual CError onCounterAMConnected();
   virtual CError onCounterAMDisconnected(){return CError(CError::NO_ERROR, "CSystemStateMachine");}

   virtual CError onAuthenticationOK();
   virtual CError onAuthenticationNOK();
   virtual CError onAuthenticationCanceled();

   virtual void onConnectivityAgentAvailable();
   virtual void onConnectivityAgentNotAvailable(){LOG4CPLUS_TRACE(sLogger, "CSystemStateMachine::onConnectivityAgentNotAvailable()");};

   virtual void onChannelSupervisorAvailable();
   virtual void onChannelSupervisorNotAvailable(){LOG4CPLUS_TRACE(sLogger, "CSystemStateMachine::onChannelSupervisorNotAvailable()");};

   virtual void onProfileManagerAvailable();
   virtual void onProfileManagerNotAvailable(){LOG4CPLUS_TRACE(sLogger, "CSystemStateMachine::onProfileManagerNotAvailable()");};

   virtual void onAppManAvailable();
   virtual void onAppManNotAvailable(){LOG4CPLUS_TRACE(sLogger, "CSystemStateMachine::onProfileManagerNotAvailable()");};

   virtual void onAuthenticationAppAvailable();
   virtual void onAuthenticationAppNotAvailable(){LOG4CPLUS_TRACE(sLogger, "CSystemStateMachine::onProfileManagerNotAvailable()");};

private:

   friend class CSystemState;

   void changeState(CSystemState* state);

   CSystemState* mSystemState;

   virtual void threadFunc()
   {
      startTriggerHandler();
   }

   void startTriggerHandler();
};

} /* namespace SystemController */

#endif /* SYSTEMSTATEMACHINE_HPP */
