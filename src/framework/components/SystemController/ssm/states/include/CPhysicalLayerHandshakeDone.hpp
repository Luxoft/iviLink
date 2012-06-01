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
 * PhysicalLayerHandshakeDone.hpp
 *
 *  Created on: Apr 6, 2012
 *      Author: Kramchaninov Pavel
 */

#ifndef PHYSICALLAYERHANDSHAKEDONE_HPP
#define PHYSICALLAYERHANDSHAKEDONE_HPP

#include "framework/components/SystemController/ssm/states/include/CSystemState.hpp"

#include "utils/misc/include/Logger.hpp"

namespace SystemController
{

class CPhysicalLayerHandshakeDone : public CSystemState
{
private:
   static Logger sLogger;

public:

   static CSystemState* getInstance();

   virtual string stateName() {return "CPhysicalLayerHandshakeDone";}

   virtual void launchChannelLayer(CSystemStateMachine* stateMachine);
   virtual void onChannelSupervisorAvailable(CSystemStateMachine* stateMachine);

protected:
	CPhysicalLayerHandshakeDone();
	virtual ~CPhysicalLayerHandshakeDone();

private:

   static CPhysicalLayerHandshakeDone* sInstance;
};

} /* namespace SystemController */
#endif /* PHYSICALLAYERHANDSHAKEDONE_HPP */
