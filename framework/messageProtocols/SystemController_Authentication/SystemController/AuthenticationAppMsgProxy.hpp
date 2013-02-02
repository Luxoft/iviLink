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


#ifndef CAUTHENTICATION_APP_MDG_PROXY_HPP
#define CAUTHENTICATION_APP_MDG_PROXY_HPP

#include <string>
#include <unistd.h>
#include "BaseError.hpp"
#include "ICallbackHandler.hpp"
#include "ipc_common.hpp"
#include "CIpc.hpp"

#include "Logger.hpp"
#include "SysCtrlAuthProtocol.hpp"
#include "CommonMessage.hpp"

using namespace std;
using namespace iviLink::Ipc;

namespace SystemControllerMsgProtocol
{

class AuthenticationAppMsgProxy : public iviLink::Ipc::ICallbackHandler
{
private:
   static Logger logger;

   const char* getName() {return "AuthenticationAppMsgProxy";}

   iviLink::Ipc::CIpc* mpIpc;

public:
   explicit AuthenticationAppMsgProxy(const string& connectionName);
   virtual ~AuthenticationAppMsgProxy();

protected:

   // Outgoing messages
   //
   BaseError requestShutDown();

   // Incoming messages
   // should be implemented by implementation
   virtual void onAuthenticationOK() = 0;
   virtual void onAuthenticationNOK() = 0;
   virtual void onAuthenticationCanceled() = 0;
   virtual void onAuthenticationError() = 0;

protected:
   // from ICallbackHandler

   virtual void OnConnection(iviLink::Ipc::DirectionID);
   virtual void OnConnectionLost(iviLink::Ipc::DirectionID);

   virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID);
   virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, iviLink::Ipc::DirectionID);

   virtual void onAuthenticationAppAvailable() = 0;
   virtual void onAuthenticationAppNotAvailable() = 0;

   CMsgIdGen mMsgIdGen;

   BaseError connectAuthenticationApp();

   bool isConnected() const;

   UInt8 mWriteBuffer[sizeof(AuthenticationMessage)];
};

}
#endif /* CAUTHENTICATION_APP_MDG_PROXY_HPP */
