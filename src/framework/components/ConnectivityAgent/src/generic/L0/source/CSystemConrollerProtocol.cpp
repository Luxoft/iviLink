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

/***************************************************************************
* Project           AXIS
* (c) copyright     2012
* Company           LUXOFT
* @file             CSystemConrollerProtocol.cpp
* @brief            The CSystemConrollerProtocol class implementation file
* @author           KSkidanov
***************************************************************************/
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
 /********************************************************************
 *
 * Forward declaration includes
 *
 ********************************************************************/
#include "framework/components/ConnectivityAgent/src/generic/L0/include/L0InterfaceStub.hpp"
/********************************************************************
 *
 * The class includes
 *
 ********************************************************************/
#include "framework/components/ConnectivityAgent/src/generic/L0/include/CSystemControllerProtocol.hpp"
 /********************************************************************
 *
 * Other includes
 *
 ********************************************************************/


CSystemConrollerProtocol::CSystemConrollerProtocol(L0InterfaceStub* pStub, const std::string connectionName):
	SystemControllerMsgProxy(connectionName),
	mpStub(pStub)
{
}
CSystemConrollerProtocol::~CSystemConrollerProtocol()
{

}

void CSystemConrollerProtocol::sendConnectionEstablished()
{
	SystemControllerMsgProxy::requestConnectionEstablished();
}

void CSystemConrollerProtocol::CSystemConrollerProtocol::sendConnectionLost()
{
	SystemControllerMsgProxy::requestConnectionLost();	
}

CError CSystemConrollerProtocol::onShutDown()
{
	mpStub->onShutDown();
}
CError CSystemConrollerProtocol::connect()
{
	return SystemControllerMsgProxy::connect();
}
