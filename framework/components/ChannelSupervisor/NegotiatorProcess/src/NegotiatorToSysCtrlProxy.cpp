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

#include "Negotiator.hpp"

#include "NegotiatorToSysCtrlProxy.hpp" 

using namespace iviLink;
using namespace ChannelSupervisor;

NegotiatorToSysCtrlProxy::NegotiatorToSysCtrlProxy(const std::string& connectionName,
                Negotiator* pOwner)
: SystemControllerMsgProxy(connectionName)
, mNegotiator(pOwner)
{
}

NegotiatorToSysCtrlProxy::~NegotiatorToSysCtrlProxy()
{

}

BaseError NegotiatorToSysCtrlProxy::sendConnectionEstablished()
{
    return SystemControllerMsgProxy::requestConnectionEstablished();
}

BaseError NegotiatorToSysCtrlProxy::sendConnectionLost()
{
    return SystemControllerMsgProxy::requestConnectionLost();
}

BaseError NegotiatorToSysCtrlProxy::onShutDown()
{
    mNegotiator->onShutDown();
    return BaseError::NoError();
}

void NegotiatorToSysCtrlProxy::onReceiveRole(bool isMaster)
{
    mNegotiator->onReceiveRole(isMaster);
}

BaseError NegotiatorToSysCtrlProxy::connect()
{
    return SystemControllerMsgProxy::connect();
}
