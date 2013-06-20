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

#include <iostream>
#include <sstream>
#include <ctime>

#include "Negotiator.hpp"

#include "CMutex.hpp"
#include "Types.hpp"
#include "Logger.hpp"
#include "NegotiatorTube.hpp"
#include "ProcessorsHolder.hpp"
#include "NegotiatorConstants.hpp"

using namespace iviLink::ChannelSupervisor;
using namespace std;

Logger Negotiator::mLogger = Logger::getInstance(
                LOG4CPLUS_TEXT("ChannelSupervisor.NegotiatorProcess.Negotiator"));
static const char* systemControllerSocket = "SysCtr_ChSprvsr";

namespace
{
std::string getAddress(std::string const& sock_addr)
{
    if (!sock_addr.empty())
    {
        return sock_addr;
    }
    return IPC_NEGOTIATOR_ADDRESS;
}
}

Negotiator::Negotiator(std::string const& sock_addr)
: mSysCtrlProxy(systemControllerSocket, this)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    BaseError err = mSysCtrlProxy.connect();
    LOG4CPLUS_INFO(mLogger, static_cast<std::string>(err));
    mWatchdogThread = new ChannelSupervisorWatchdogThread();
    mTube = new NegotiatorTube();
    mIpcServer = new NegotiatorIPCHandler(getAddress(sock_addr), mTube);
}

Negotiator::~Negotiator()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    delete mIpcServer;
    delete mTube;
    delete mWatchdogThread;
    ProcessorsHolder::deleteInstance();
    Map::getInstance()->deleteInstance();
}

void Negotiator::waitShutdown()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mShutdownSema.wait();
}

void Negotiator::onShutDown()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mShutdownSema.signal();
}

void Negotiator::onReceiveRole(bool isMaster)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mIsMaster = isMaster;
    mRoleSema.signal();
}

void Negotiator::init()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    mRoleSema.wait();
    ProcessorsHolder::getInstance()->setRole(mIsMaster);
    mTube->init();
#ifndef  __APPLE__
    /**
     * iOS implementation sends automated responses to
     * other side's watchdog, because otherwise
     * start of the KickWatchdogThread on both sides will be not
     * synchronized (here everything is run in the same process,
     * and on other platforms each process has its own kickwatchdogthread).
     */
    mWatchdogThread->start();
#endif
    mSysCtrlProxy.sendConnectionEstablished();
}
