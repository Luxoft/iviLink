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


#include "Negotiator.hpp"

#include "CNegotiatorToSysCtrlProxy.hpp" 

using namespace iviLink;
using namespace ChannelSupervisor;

CNegotiatorToSysCtrlProxy::CNegotiatorToSysCtrlProxy(const std::string connectionName,
        Negotiator* pOwner)
        : SystemControllerMsgProxy(connectionName), mpNegotiator(pOwner)
{
}
CNegotiatorToSysCtrlProxy::~CNegotiatorToSysCtrlProxy()
{

}

BaseError CNegotiatorToSysCtrlProxy::sendConnectionEstablished()
{
    return SystemControllerMsgProxy::requestConnectionEstablished();
}

BaseError CNegotiatorToSysCtrlProxy::sendConnectionLost()
{
    return SystemControllerMsgProxy::requestConnectionLost();
}

BaseError CNegotiatorToSysCtrlProxy::onShutDown()
{
    mpNegotiator->onShutDown();
    return BaseError::NoError("CNegotiatorToSysCtrlProxy");
}

BaseError CNegotiatorToSysCtrlProxy::connect()
{
    return SystemControllerMsgProxy::connect();
}
