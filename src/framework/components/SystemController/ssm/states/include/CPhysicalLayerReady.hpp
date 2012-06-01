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

/*
 * PhysicalLayerReady.hpp
 *
 *  Created on: Mar 27, 2012
 *      Author: Kramchaninov Pavel
 */

#ifndef PHYSICAL_LAYER_READY_HPP_
#define PHYSICAL_LAYER_READY_HPP_

#include "framework/components/SystemController/ssm/states/include/CSystemState.hpp"

#include "utils/misc/include/Logger.hpp"

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
#endif /* PHYSICAL_LAYER_READY_HPP_ */
