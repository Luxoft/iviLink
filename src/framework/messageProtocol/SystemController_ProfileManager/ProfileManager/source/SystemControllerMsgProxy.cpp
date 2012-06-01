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
 * MessageProtocolBase.cpp
 *
 *  Created on: Mar 19, 2012
 *      Author: Kramchaninov Pavel
 */

#include <cassert>

#include "framework/messageProtocol/SystemController_ProfileManager/ProfileManager/include/SystemControllerMsgProxy.hpp"
#include "framework/messageProtocol/SystemController_ProfileManager/messages.hpp"

using namespace AXIS::Ipc;

namespace ProfileManagerMsgProtocol
{

SystemControllerMsgProxy::SystemControllerMsgProxy(const string connectionName):
   mpIpc(NULL)
{
   mpIpc = new CIpc(connectionName, *this);
}

SystemControllerMsgProxy::~SystemControllerMsgProxy()
{
   delete mpIpc;
}

CError SystemControllerMsgProxy::connect()
{
   return mpIpc->connect();
}

bool SystemControllerMsgProxy::isConnected() const
{
   return mpIpc->isConnected();
}

void SystemControllerMsgProxy::OnRequest(AXIS::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, AXIS::Ipc::DirectionID)
{
   Message const* req = reinterpret_cast<Message const*>(pPayload);

   assert(req->header.size + sizeof(Message) == payloadSize);
   assert(bufferSize >= sizeof(Message));

   //Message* resp = reinterpret_cast<Message*>(pResponseBuffer);

   switch(req->header.type)
   {
   case SC_PM_SHUTDOWN:
      onShutDown();
      break;
   default:
      break;
   }
}

void SystemControllerMsgProxy::OnConnection(AXIS::Ipc::DirectionID)
{

}

void SystemControllerMsgProxy::OnConnectionLost(AXIS::Ipc::DirectionID)
{

}

SystemControllerMsgProxy::CMsgIdGen::CMsgIdGen() :
      mId(-1)
{
}

SystemControllerMsgProxy::CMsgIdGen::~CMsgIdGen()
{

}

AXIS::Ipc::MsgID SystemControllerMsgProxy::CMsgIdGen::getNext()
{
   mId += 2;
   return mId;
}

}
