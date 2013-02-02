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


#ifndef PHYSICAL_LAYER_READY_HPP_
#define PHYSICAL_LAYER_READY_HPP_

#include "CSystemState.hpp"

#include "Logger.hpp"

namespace iviLink
{
namespace SystemController
{

class CPhysicalLayerReady : public CSystemState
                           // @TODO: Implement MsgProtocol to ConnectivityAgent
                           // and sent PHISYCAL_CONNECTION_ESTABLISHED or PHISYCAL_CONNECTION_LOST
                           // when corresponding messages are received from CA
{
private:
   static Logger sLogger;

public:

   static CSystemState* getInstance();

   virtual string stateName() {return "CPhysicalLayerReady";}

protected:

   CPhysicalLayerReady();
   virtual ~CPhysicalLayerReady();

   virtual void onConnectivityAgentHandshakeDone(CSystemStateMachine* stateMachine);

private:

   static CPhysicalLayerReady* sInstance;

};

} /* namespace SystemController */
}
#endif /* PHYSICAL_LAYER_READY_HPP_ */
