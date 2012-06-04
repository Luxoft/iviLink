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





#ifndef SYSTEMSTATEMACHINE_HPP
#define SYSTEMSTATEMACHINE_HPP

#include "utils/threads/include/CThread.hpp"
#include "framework/messageProtocol/SystemController_ConnectivityAgent/SystemController/include/ConnectivityAgentMsgProxy.hpp"
#include "framework/messageProtocol/SystemController_ChannelSupervisor/SystemController/include/ChannelSupervisorMsgProxy.hpp"
#include "framework/messageProtocol/SystemController_ProfileManager/SystemController/include/ProfileManagerMsgProxy.hpp"
#include "framework/messageProtocol/SystemController_AppMan/SystemController/include/AppManMsgProxy.hpp"

#include "utils/misc/include/Logger.hpp"

namespace SystemController
{

class CSystemState;

class CSystemStateMachine : public CThread
                          , public SystemControllerMsgProtocol::ConnectivityAgentMsgProxy
                          , public SystemControllerMsgProtocol::ChannelSupervisorMsgProxy
                          , public SystemControllerMsgProtocol::ProfileManagerMsgProxy
                          , public SystemControllerMsgProtocol::AppManMsgProxy
{
private:
   static Logger sLogger;

public:
   int mSystemGender;

   void init(int systemGender);

   CSystemStateMachine();
   virtual ~CSystemStateMachine();

   void launchPhysicalLayer();
   void launchChannelLayer();
   void startAuthentication();
   void launchProfileManager();

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

   void shutDownProfileManager()
   {
      ProfileManagerMsgProxy::requestShutDown();
   }

private:

   virtual CError onCounterCAConnected();
   virtual CError onCounterCADisconnected(){return CError(CError::NO_ERROR, "CSystemStateMachine");}

   virtual CError onCounterCSConnected();
   virtual CError onCounterCSDisconnected(){return CError(CError::NO_ERROR, "CSystemStateMachine");}

   virtual void onConnectivityAgentAvailable();
   virtual void onConnectivityAgentNotAvailable(){LOG4CPLUS_TRACE(sLogger, "CSystemStateMachine::onConnectivityAgentNotAvailable()");};

   virtual void onChannelSupervisorAvailable();
   virtual void onChannelSupervisorNotAvailable(){LOG4CPLUS_TRACE(sLogger, "CSystemStateMachine::onChannelSupervisorNotAvailable()");};

   virtual void onProfileManagerAvailable();
   virtual void onProfileManagerNotAvailable(){LOG4CPLUS_TRACE(sLogger, "CSystemStateMachine::onProfileManagerNotAvailable()");};

   virtual void onAppManAvailable(){LOG4CPLUS_WARN(sLogger, "Implement onProfileManagerNotAvailable() handling");};;
   virtual void onAppManNotAvailable(){LOG4CPLUS_TRACE(sLogger, "CSystemStateMachine::onProfileManagerNotAvailable()");};

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
