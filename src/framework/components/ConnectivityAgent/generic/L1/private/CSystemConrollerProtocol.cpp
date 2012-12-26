/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <cassert>
 /********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "L1InterfaceStub.hpp"
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "CSystemControllerProtocol.hpp"
 /********************************************************************
 *
 * Other includes
 *
 ********************************************************************/

using namespace iviLink::ConnectivityAgent::HAL;
using namespace iviLink::ConnectivityAgent::L1;
CSystemConrollerProtocol::CSystemConrollerProtocol(L1InterfaceStub* pStub, const std::string connectionName):
   SystemControllerMsgProxy(connectionName),
   mpStub(pStub)
{
}
CSystemConrollerProtocol::~CSystemConrollerProtocol()
{

}

void CSystemConrollerProtocol::sendConnectionEstablished(HAL::EGenderType gender)
{
   int g = 1;
   switch (gender)
   {
   case eServerGender: g = 1; break;
   case eClientGender: g = 2; break;
   case eAnyGender: assert(false); break;
   }

   SystemControllerMsgProxy::requestConnectionEstablished(g);
}

void CSystemConrollerProtocol::CSystemConrollerProtocol::sendConnectionLost()
{
   SystemControllerMsgProxy::requestConnectionLost();
}

CError CSystemConrollerProtocol::onShutDown()
{
   mpStub->onShutDown();
   return CError::NoError("CSystemConrollerProtocol");
}

CError CSystemConrollerProtocol::connect()
{
   return SystemControllerMsgProxy::connect();
}
