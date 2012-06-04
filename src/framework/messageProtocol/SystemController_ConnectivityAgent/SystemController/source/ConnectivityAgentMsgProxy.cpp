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





#include <cassert>

#include "framework/messageProtocol/SystemController_ConnectivityAgent/SystemController/include/ConnectivityAgentMsgProxy.hpp"
#include "framework/messageProtocol/SystemController_ConnectivityAgent/messages.hpp"

using namespace AXIS::Ipc;

namespace SystemControllerMsgProtocol
{

Logger ConnectivityAgentMsgProxy::logger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.msgProtocol.ConnectivityAgentMsgProxy"));

ConnectivityAgentMsgProxy::ConnectivityAgentMsgProxy(const string connectionName):
   mpIpc(NULL)
{
   LOG4CPLUS_TRACE(logger, "ConnectivityAgentMsgProxy(" + connectionName + ")");

   mpIpc = new CIpc(connectionName, *this);
}

ConnectivityAgentMsgProxy::~ConnectivityAgentMsgProxy()
{
   LOG4CPLUS_TRACE(logger, "~ConnectivityAgentMsgProxy()");

   delete mpIpc;
}

CError ConnectivityAgentMsgProxy::connectConnectivityAgent()
{
   LOG4CPLUS_TRACE(logger, "beginWaitForConnection()");

   return mpIpc->beginWaitForConnection();
}

bool ConnectivityAgentMsgProxy::isConnected() const
{
   return mpIpc->isConnected();
}

CError ConnectivityAgentMsgProxy::requestShutDown()
{
   LOG4CPLUS_TRACE(logger, "requestShutDown()");

   if (!mpIpc)
      return CError(1, getName(), CError::FATAL, "no ipc");

   Message* req = reinterpret_cast<Message*>(mWriteBuffer);
   req->header.type = SC_CA_SHUTDOWN;
   req->header.size = 0;

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(Message) + req->header.size;
   //UInt32 respSize = BUFFER_SIZE;
   UInt32 respSize = 0;

   CError err = mpIpc->request(id, mWriteBuffer, reqSize, mReadBuffer, respSize);

   if (!err.isNoError())
      return err;

   return CError::NoError(getName());
}

void ConnectivityAgentMsgProxy::OnRequest(AXIS::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, AXIS::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE(logger, "OnRequest()");

   Message const* req = reinterpret_cast<Message const*>(pPayload);

   assert(req->header.size + sizeof(Message) == payloadSize);
   assert(bufferSize >= sizeof(Message));

   switch(req->header.type)
   {
   case CA_SC_CONNECTION_ESTABLISHED:
      onCounterCAConnected();
      break;
   case CA_SC_CONNECTION_LOST:
      onCounterCADisconnected();
      break;
   default:
      break;
   }

   bufferSize = 0;
}

void ConnectivityAgentMsgProxy::OnConnection(AXIS::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE(logger, "OnConnection()");

   onConnectivityAgentAvailable();
}

void ConnectivityAgentMsgProxy::OnConnectionLost(AXIS::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE(logger, "OnConnectionLost()");

   onConnectivityAgentNotAvailable();
}

ConnectivityAgentMsgProxy::CMsgIdGen::CMsgIdGen() :
      mId(-1)
{
}

ConnectivityAgentMsgProxy::CMsgIdGen::~CMsgIdGen()
{

}

AXIS::Ipc::MsgID ConnectivityAgentMsgProxy::CMsgIdGen::getNext()
{
   mId += 2;
   return mId;
}

}

