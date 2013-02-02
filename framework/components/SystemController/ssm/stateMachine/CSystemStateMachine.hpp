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


#ifndef SYSTEMSTATEMACHINE_HPP
#define SYSTEMSTATEMACHINE_HPP

#include "CThread.hpp"
#include "ConnectivityAgentMsgProxy.hpp"
#include "ChannelSupervisorMsgProxy.hpp"
#include "ProfileManagerMsgProxy.hpp"
#include "AppManMsgProxy.hpp"
#include "AuthenticationAppMsgProxy.hpp"
#include "SystemControllerWatchdogProxy.hpp"
#include "Logger.hpp"

#define SPLASH_SCREEN_PORT 3425
#define stopMessage 1
 
namespace iviLink
{
namespace SystemController
{

class CSystemState;

class CSystemStateMachine : public CThread
                          , public SystemControllerMsgProtocol::ConnectivityAgentMsgProxy
                          , public SystemControllerMsgProtocol::ChannelSupervisorMsgProxy
                          , public SystemControllerMsgProtocol::ProfileManagerMsgProxy
                          , public SystemControllerMsgProtocol::AppManMsgProxy
                          , public SystemControllerMsgProtocol::AuthenticationAppMsgProxy
                          , public SystemControllerMsgProtocol::SystemControllerWatchdogProxy
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
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
      ProfileManagerMsgProxy::requestUnlockAuthenticationProfile();
   }

   void unlockProfiles()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
      ProfileManagerMsgProxy::requestUnlockProfiles();
      ProfileManagerMsgProxy::requestAuthenticationDone();
   }

   void connectSystemWatchdog()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
      SystemControllerWatchdogProxy::connectSystemControllerWatchdog();
   }

   void connectConnectivityAgent()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
      ConnectivityAgentMsgProxy::connectConnectivityAgent();
   }

   void connectChannelSupervisor()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
      ChannelSupervisorMsgProxy::connectChannelSupervisor();
   }

   void connectProfileManager()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
	   ProfileManagerMsgProxy::connectProfileManager();
   }

   void connectApplicationManager()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
	   AppManMsgProxy::connectAppMan();
   }

   void connectAuthenticationApp()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
	   AuthenticationAppMsgProxy::connectAuthenticationApp();
   }

   void shutDownProfileManager()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
      ProfileManagerMsgProxy::requestShutDown();
   }

private:

   virtual BaseError onCounterCAConnected(UInt8 role);
   virtual BaseError onCounterCADisconnected();

   virtual BaseError onCounterCSConnected();
   virtual BaseError onCounterCSDisconnected(){return BaseError(BaseError::IVILINK_NO_ERROR, "CSystemStateMachine");}

   virtual BaseError onCounterPMConnected();
   virtual BaseError onCounterPMDisconnected(){return BaseError(BaseError::IVILINK_NO_ERROR, "CSystemStateMachine");}

   virtual BaseError onCounterAMConnected();
   virtual BaseError onCounterAMDisconnected(){return BaseError(BaseError::IVILINK_NO_ERROR, "CSystemStateMachine");}


   virtual void onSystemControllerWatchdogAvaliable();
   virtual void onSystemControllerWatchdogNotAvaliable();

   virtual void onAuthenticationOK();
   virtual void onAuthenticationNOK();
   virtual void onAuthenticationCanceled();
   virtual void onAuthenticationError();

   virtual void onConnectivityAgentAvailable();
   virtual void onConnectivityAgentNotAvailable()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
      LOG4CPLUS_INFO(sLogger, __PRETTY_FUNCTION__);
   };

   virtual void onChannelSupervisorAvailable();
   virtual void onChannelSupervisorNotAvailable()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
      LOG4CPLUS_INFO(sLogger, __PRETTY_FUNCTION__);
   };

   virtual void onProfileManagerAvailable();
   virtual void onProfileManagerNotAvailable()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
      LOG4CPLUS_INFO(sLogger, __PRETTY_FUNCTION__);
   };

   virtual void onAppManAvailable();
   virtual void onAppManNotAvailable()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
      LOG4CPLUS_INFO(sLogger, __PRETTY_FUNCTION__);
   };

   virtual void onAuthenticationAppAvailable();
   virtual void onAuthenticationAppNotAvailable()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
      LOG4CPLUS_INFO(sLogger, __PRETTY_FUNCTION__);
   };

   virtual bool getLinkState();
private:

   friend class CSystemState;

   void changeState(CSystemState* state);

   CSystemState* mSystemState;

   virtual void threadFunc()
   {
      LOG4CPLUS_TRACE_METHOD(sLogger, __PRETTY_FUNCTION__);
      startTriggerHandler();
   }

   void doReset();

   void startTriggerHandler();

   bool linkIsUp;
};

} /* namespace SystemController */
} // namespace iviLink

#endif /* SYSTEMSTATEMACHINE_HPP */
