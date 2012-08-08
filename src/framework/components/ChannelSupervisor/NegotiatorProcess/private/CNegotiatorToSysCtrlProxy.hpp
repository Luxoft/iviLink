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








#ifndef NEGOTIATOR_SYSTEM_CONTROLLER_PROTOCOL_HPP
#define NEGOTIATOR_SYSTEM_CONTROLLER_PROTOCOL_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
 #include <string>
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "utils/misc/Types.hpp"
#include "framework/messageProtocol/SystemController_ChannelSupervisor/ChannelSupervisor/SystemControllerMsgProxy.hpp"
#include "framework/messageProtocol/SystemController_ChannelSupervisor/messages.hpp"

namespace iviLink {
namespace ChannelSupervisor {

class Negotiator;

class CNegotiatorToSysCtrlProxy: public ChannelSupervisorMsgProtocol::SystemControllerMsgProxy
{
   public:

   CNegotiatorToSysCtrlProxy( const std::string connectionName, Negotiator* pOwner);

   ~CNegotiatorToSysCtrlProxy();

   CError sendConnectionEstablished();

   CError sendConnectionLost();

   virtual CError onShutDown();

   CError connect();

   private:
   	Negotiator * mpNegotiator;
};

}  // namespace ChannelSupervisor
}  // namespace AXIS

#endif
