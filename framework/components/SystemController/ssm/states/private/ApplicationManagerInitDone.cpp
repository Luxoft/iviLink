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


#include "ApplicationManagerInitDone.hpp"
#include "CSystemStateMachine.hpp"
#include "CComponentLauncher.hpp"
#include "CPhysicalLayerReady.hpp"


namespace iviLink
{
namespace SystemController
{

ApplicationManagerInitDone* ApplicationManagerInitDone::sInstance = 0;

ApplicationManagerInitDone::ApplicationManagerInitDone()
    : mLogger(Logger::getInstance(LOG4CPLUS_TEXT("systemController.SystemStateMachine.states.ApplicationManagerInitDone")))
{
}

ApplicationManagerInitDone::~ApplicationManagerInitDone()
{
}

CSystemState* ApplicationManagerInitDone::getInstance()
{
    if(0 == sInstance)
    {
        sInstance = new ApplicationManagerInitDone();
    }

    return sInstance;
}
void ApplicationManagerInitDone::launchPhysicalLayer(CSystemStateMachine* stateMachine)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    #ifndef ANDROID
    CComponentLauncher::getInstance()->launchConnectivityAgent();
    stateMachine->connectConnectivityAgent();
    #else
    stateMachine->connectConnectivityAgent();
    iviLink::Android::makeRequest(iviLink::Android::eLaunchAgent);
    #endif //ANDROID
}

void ApplicationManagerInitDone::onConnectivityAgentAvailable(CSystemStateMachine* stateMachine)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    changeState(stateMachine, CPhysicalLayerReady::getInstance());

    SSMTriggerQueue::getInstance()->sendTrigger(CONNECTIVITY_AGENT_STARTED);
}

} /* namespace SystemController */
} /* namespace iviLink */
