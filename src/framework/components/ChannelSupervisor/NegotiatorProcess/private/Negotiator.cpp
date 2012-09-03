/* 
 * 
 * iviLINK SDK, version 1.1.2
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











/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <iostream>
#include <sstream>
#include <ctime>
 /********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "Negotiator.hpp"
 /********************************************************************
 *
 * Other includes
 *
 ********************************************************************/

#include "utils/threads/CMutex.hpp"
#include "utils/misc/Types.hpp"
#include "utils/misc/Logger.hpp"
#include "NegotiatorIPCHandler.hpp"
#include "NegotiatorTube.hpp"
#include "NegotiatorStates.hpp"

using namespace iviLink::ChannelSupervisor;
using namespace std;

const time_t semaTomeoutmiliSec = 1000;

Logger Negotiator::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("NegotiatorProcess.Negotiator"));
static const char* systemControllerSocket = "SysCtr_ChSprvsr";

Negotiator::Negotiator(std::string const& sock_addr):mSysCtrlProxy(systemControllerSocket, this)
{

   NegotiaterStates * states = new NegotiaterStates();

   CError err = mSysCtrlProxy.connect();
   LOG4CPLUS_INFO(msLogger, static_cast<std::string>(err));

   NegotiatorTube * tube = new NegotiatorTube(states, &mSysCtrlProxy);

   m_ipcHandler = new NegotiatorIPCHandler(tube, states, sock_addr);

}

Negotiator::~Negotiator()
{
   if(m_ipcHandler)
   {
      delete m_ipcHandler;
   }
   m_ipcHandler = NULL;
}

NegotiatorIPCHandler* Negotiator::getHandler()
{
   return m_ipcHandler;
}
