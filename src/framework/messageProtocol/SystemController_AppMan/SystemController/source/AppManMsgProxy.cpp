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

#include "framework/messageProtocol/SystemController_AppMan/SystemController/include/AppManMsgProxy.hpp"
#include "framework/messageProtocol/SystemController_AppMan/messages.hpp"

using namespace AXIS::Ipc;

namespace SystemControllerMsgProtocol
{

Logger AppManMsgProxy::logger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.msgProtocol.AppManMsgProxy"));

AppManMsgProxy::AppManMsgProxy(const string connectionName):
   mpIpc(NULL)
{
   LOG4CPLUS_TRACE(logger, "AppManMsgProxy(" + connectionName + ")");

   mpIpc = new CIpc(connectionName, *this);
}

AppManMsgProxy::~AppManMsgProxy()
{
   LOG4CPLUS_TRACE(logger, "~AppManMsgProxy()");

   delete mpIpc;
}

CError AppManMsgProxy::connectAppMan()
{
   LOG4CPLUS_TRACE(logger, "beginWaitForConnection()");

   return mpIpc->beginWaitForConnection();
}

bool AppManMsgProxy::isConnected() const
{
   return mpIpc->isConnected();
}

CError AppManMsgProxy::requestShutDown()
{
   LOG4CPLUS_TRACE(logger, "requestShutDown()");

   if (!mpIpc)
      return CError(1, getName(), CError::FATAL, "no ipc");

   Message* req = reinterpret_cast<Message*>(mWriteBuffer);
   req->header.type = SC_AM_SHUTDOWN;
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

void AppManMsgProxy::OnRequest(AXIS::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, AXIS::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE(logger, "OnRequest()");
}

void AppManMsgProxy::OnConnection(AXIS::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE(logger, "OnConnection()");

   onAppManAvailable();
}

void AppManMsgProxy::OnConnectionLost(AXIS::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE(logger, "OnConnectionLost()");

   onAppManNotAvailable();
}

AppManMsgProxy::CMsgIdGen::CMsgIdGen() :
      mId(-1)
{
}

AppManMsgProxy::CMsgIdGen::~CMsgIdGen()
{

}

AXIS::Ipc::MsgID AppManMsgProxy::CMsgIdGen::getNext()
{
   mId += 2;
   return mId;
}

}
