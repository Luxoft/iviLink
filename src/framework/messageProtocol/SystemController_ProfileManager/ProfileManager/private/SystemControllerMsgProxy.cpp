/* 
 * 
 * iviLINK SDK, version 1.1.2
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
#ifndef ANDROID
#include "SystemControllerMsgProxy.hpp"
#else
#include "framework/messageProtocol/SystemController_ProfileManager/ProfileManager/SystemControllerMsgProxy.hpp"
#endif //ANDROID
#include "framework/messageProtocol/SystemController_ProfileManager/messages.hpp"
#include "utils/ipc/helpers/buffer_helpers.hpp"

using namespace iviLink::Ipc;
using iviLink::Ipc::Helpers::CBufferReader;
using iviLink::Ipc::Helpers::CBufferWriter;

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

CError SystemControllerMsgProxy::requestConnected()
{
   //LOG4CPLUS_TRACE(logger, "requestConnected()");

   if (!mpIpc)
      return CError(1, getName(), CError::FATAL, "no ipc");

   Message* req = reinterpret_cast<Message*>(mWriteBuffer);
   req->header.type = PM_SC_CONNECTION_ESTABLISHED;
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

CError SystemControllerMsgProxy::requestDisconnected()
{
   //LOG4CPLUS_TRACE(logger, "requestDisconnected()");

   if (!mpIpc)
      return CError(1, getName(), CError::FATAL, "no ipc");

   Message* req = reinterpret_cast<Message*>(mWriteBuffer);
   req->header.type = PM_SC_CONNECTION_LOST;
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
   Message const* req = reinterpret_cast<Message const*>(pPayload);

   assert(req->header.size + sizeof(Message) == payloadSize);
   assert(bufferSize >= sizeof(Message));

   Message* resp = reinterpret_cast<Message*>(pResponseBuffer);

   switch(req->header.type)
   {
   case SC_PM_SHUTDOWN:
      {
         onShutDown();
         bufferSize = 0;
      }
      break;
   case SC_PM_UNLOCK_PROFILES:
      {
         bool wasError = !onUnlockProfiles().isNoError();

         CBufferWriter writer(resp->data, bufferSize - sizeof(MessageHeader));
         writer.write(wasError);

         resp->header.size = writer.getUsedSize();

         bufferSize = sizeof(Message) + resp->header.size;
      }
      break;
   case SC_PM_UNLOCK_AUTHENTICATION_PROFILE:
      {
         bool wasError = !onUnlockAuthenticationProfile().isNoError();

         CBufferWriter writer(resp->data, bufferSize - sizeof(MessageHeader));
         writer.write(wasError);

         resp->header.size = writer.getUsedSize();

         bufferSize = sizeof(Message) + resp->header.size;
      }
      break;
   default:
      assert(false);
      break;
   }
}

void SystemControllerMsgProxy::OnConnection(iviLink::Ipc::DirectionID)
{

}

void SystemControllerMsgProxy::OnConnectionLost(iviLink::Ipc::DirectionID)
{

}

SystemControllerMsgProxy::CMsgIdGen::CMsgIdGen() :
      mId(-1)
{
}

SystemControllerMsgProxy::CMsgIdGen::~CMsgIdGen()
{

}

iviLink::Ipc::MsgID SystemControllerMsgProxy::CMsgIdGen::getNext()
{
   mId += 2;
   return mId;
}

}
