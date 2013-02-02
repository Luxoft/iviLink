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


#include <cassert>
#include "SystemControllerMsgProxy.hpp"
#include "buffer_helpers.hpp"
#include "Exit.hpp"

using namespace iviLink::Ipc;
using iviLink::Ipc::Helpers::CBufferReader;
using iviLink::Ipc::Helpers::CBufferWriter;

namespace AuthenticationAppMsgProtocol
{

Logger SystemControllerMsgProxy::logger = Logger::getInstance(LOG4CPLUS_TEXT("systemController.msgProtocol.SystemControllerMsgProxy"));

SystemControllerMsgProxy::SystemControllerMsgProxy(const string connectionName):
   mpIpc(NULL)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   mpIpc = new CIpc(connectionName, *this);
}

SystemControllerMsgProxy::~SystemControllerMsgProxy()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   delete mpIpc;
}

BaseError SystemControllerMsgProxy::connect()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

   return mpIpc->connect();
}

bool SystemControllerMsgProxy::isConnected() const
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);


   return mpIpc->isConnected();
}

BaseError SystemControllerMsgProxy::requestAuthenticationOK()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   return requestAuthenticationState(AA_SC_AUTHENTICATION_OK);
}

BaseError SystemControllerMsgProxy::requestAuthenticationNOK()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   return requestAuthenticationState(AA_SC_AUTHENTICATION_NOK);
}

BaseError SystemControllerMsgProxy::requestAuthenticationCanceled()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   return requestAuthenticationState(AA_SC_AUTHENTICATION_CANCELED);
}


BaseError SystemControllerMsgProxy::requestAuthenticationError()
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   return requestAuthenticationState(AA_SC_AUTHENTICATION_ERROR);
}


BaseError SystemControllerMsgProxy::requestAuthenticationState(AuthenticationToSystemController authenticationState)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);

    if (!mpIpc)
      return BaseError(1, getName(), BaseError::IVILINK_FATAL, "no ipc");

   AuthenticationMessage* req = reinterpret_cast<AuthenticationMessage*>(mWriteBuffer);
   req->requestType = authenticationState;
   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(Message);

   return mpIpc->asyncRequest(id, mWriteBuffer, reqSize);
}

void SystemControllerMsgProxy::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   LOG4CPLUS_FATAL(logger, "Some request came from System Controller, this should not happen!");
   bufferSize = 0;
}


void SystemControllerMsgProxy::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   AuthenticationMessage const* req = reinterpret_cast<AuthenticationMessage const*>(pPayload);
   if(req->requestType == SC_AA_DIE)
   {
       LOG4CPLUS_INFO(logger, "Request to shutdown came");
       killProcess();
   }
}

void SystemControllerMsgProxy::OnConnection(iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
}

void SystemControllerMsgProxy::OnConnectionLost(iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
   // to prevent auth app from leaking when stack has shut down / restarted
   killProcess();
}

}
