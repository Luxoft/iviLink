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


#include <iostream>
#include <sstream>
#include <ctime>

#include "Negotiator.hpp"

#include "CMutex.hpp"
#include "Types.hpp"
#include "Logger.hpp"
#include "NegotiatorIPCHandler.hpp"
#include "NegotiatorTube.hpp"
#include "NegotiatorStates.hpp"

using namespace iviLink::ChannelSupervisor;
using namespace std;

Logger Negotiator::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("NegotiatorProcess.Negotiator"));
static const char* systemControllerSocket = "SysCtr_ChSprvsr";

Negotiator::Negotiator(std::string const& sock_addr)
        : mSysCtrlProxy(systemControllerSocket, this)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    NegotiatorStates * states = new NegotiatorStates();

    BaseError err = mSysCtrlProxy.connect();
    LOG4CPLUS_INFO(msLogger, static_cast<std::string>(err));

    NegotiatorTube * tube = new NegotiatorTube(states, &mSysCtrlProxy);

    m_ipcHandler = new NegotiatorIPCHandler(tube, states, sock_addr);
    mWatchdogThread = new ChannelSupervisorWatchdogThread();
    mWatchdogThread->start();
}

Negotiator::~Negotiator()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    delete m_ipcHandler;
    delete mWatchdogThread;
}

NegotiatorIPCHandler* Negotiator::getHandler()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    return m_ipcHandler;
}
