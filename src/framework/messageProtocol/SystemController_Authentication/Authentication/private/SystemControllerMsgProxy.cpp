/* 
 * 
 * iviLINK SDK, version 1.0
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







#include <cassert>

#include "SystemControllerMsgProxy.hpp"
#include "framework/messageProtocol/SystemController_Authentication/messages.hpp"
#include "utils/ipc/helpers/buffer_helpers.hpp"

using namespace iviLink::Ipc;
using iviLink::Ipc::Helpers::CBufferReader;
using iviLink::Ipc::Helpers::CBufferWriter;

namespace AuthenticationAppMsgProtocol
{

Logger SystemControllerMsgProxy::logger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.msgProtocol.SystemControllerMsgProxy"));

SystemControllerMsgProxy::SystemControllerMsgProxy(const string connectionName):
   mpIpc(NULL)
{
   LOG4CPLUS_TRACE(logger, "SystemControllerMsgProxy(" + connectionName + ")");

   mpIpc = new CIpc(connectionName, *this);
}

SystemControllerMsgProxy::~SystemControllerMsgProxy()
{
   LOG4CPLUS_TRACE(logger, "~SystemControllerMsgProxy()");

   delete mpIpc;
}

CError SystemControllerMsgProxy::connect()
{
   LOG4CPLUS_TRACE(logger, "connect()");

   return mpIpc->connect();
}

bool SystemControllerMsgProxy::isConnected() const
{
   LOG4CPLUS_TRACE(logger, "connect()");


   return mpIpc->isConnected();
}

CError SystemControllerMsgProxy::requestAuthenticationOK()
{
   LOG4CPLUS_TRACE(logger, "requestAuthenticationOK()");

    if (!mpIpc)
      return CError(1, getName(), CError::FATAL, "no ipc");

   Message* req = reinterpret_cast<Message*>(mWriteBuffer);
   req->header.type = AA_SC_AUTHENTICATION_OK;
   req->header.size = 0;

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(Message) + req->header.size;
   //UInt32 respSize = BUFFER_SIZE;
   UInt32 respSize = 0;

   CError err = mpIpc->request(id, mWriteBuffer, reqSize, mReadBuffer, respSize);

   if (!err.isNoError())
      return err;

   return CError::NoError(getName());
}

CError SystemControllerMsgProxy::requestAuthenticationNOK()
{
   LOG4CPLUS_TRACE(logger, "requestAuthenticationNOK()");

    if (!mpIpc)
      return CError(1, getName(), CError::FATAL, "no ipc");

   Message* req = reinterpret_cast<Message*>(mWriteBuffer);
   req->header.type = AA_SC_AUTHENTICATION_NOK;
   req->header.size = 0;

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(Message) + req->header.size;
   //UInt32 respSize = BUFFER_SIZE;
   UInt32 respSize = 0;

   CError err = mpIpc->request(id, mWriteBuffer, reqSize, mReadBuffer, respSize);

   if (!err.isNoError())
      return err;

   return CError::NoError(getName());
}

CError SystemControllerMsgProxy::requestAuthenticationCanceled()
{
   LOG4CPLUS_TRACE(logger, "requestAuthenticationCanceled()");

    if (!mpIpc)
      return CError(1, getName(), CError::FATAL, "no ipc");

   Message* req = reinterpret_cast<Message*>(mWriteBuffer);
   req->header.type = AA_SC_AUTHENTICATION_CANCELED;
   req->header.size = 0;

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(Message) + req->header.size;
   //UInt32 respSize = BUFFER_SIZE;
   UInt32 respSize = 0;

   CError err = mpIpc->request(id, mWriteBuffer, reqSize, mReadBuffer, respSize);

   if (!err.isNoError())
      return err;

   return CError::NoError(getName());
}


void SystemControllerMsgProxy::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE(logger, "OnRequest() in auth SystemControllerMsgProxy");

   Message const* req = reinterpret_cast<Message const*>(pPayload);

   assert(req->header.size + sizeof(Message) == payloadSize);
   assert(bufferSize >= sizeof(Message));

   UInt8 role = 0;
   CBufferReader bufReader(req->data, req->header.size);

   switch(req->header.type)
   {
   case SC_AA_SHUTDOWN:
      onRequestShutDown();
      break;
   default:
      break;
   }

   bufferSize = 0;
}

void SystemControllerMsgProxy::OnConnection(iviLink::Ipc::DirectionID)
{
	LOG4CPLUS_TRACE(logger, "OnConnection() in auth SystemControllerMsgProxy");
}

void SystemControllerMsgProxy::OnConnectionLost(iviLink::Ipc::DirectionID)
{
	LOG4CPLUS_TRACE(logger, "OnConnectionLost() in auth SystemControllerMsgProxy");
}

SystemControllerMsgProxy::CMsgIdGen::CMsgIdGen() :
      mId(-1)
{}

SystemControllerMsgProxy::CMsgIdGen::~CMsgIdGen()
{}

iviLink::Ipc::MsgID SystemControllerMsgProxy::CMsgIdGen::getNext()
{
   mId += 2;
   return mId;
}
}
