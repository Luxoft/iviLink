/* 
 * 
 * iviLINK SDK, version 1.0.1
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









#ifndef PROFILE_MANAGER_MSG_PROXY_HPP
#define PROFILE_MANAGER_MSG_PROXY_HPP

#include <string>
#include "utils/misc/CError.hpp"
#include "utils/ipc/ICallbackHandler.hpp"
#include "utils/ipc/ipc_common.hpp"
#include "utils/ipc/CIpc.hpp"

#include "utils/misc/Logger.hpp"

using namespace std;
using namespace iviLink::Ipc;

namespace SystemControllerMsgProtocol
{

class ProfileManagerMsgProxy : public iviLink::Ipc::ICallbackHandler
{
private:
   static Logger logger;

  const char* getName() {return "ProfileManagerMsgProxy";}

   iviLink::Ipc::CIpc* mpIpc;

public:
   explicit ProfileManagerMsgProxy(const string connectionName);
   virtual ~ProfileManagerMsgProxy();

protected:

   // Outgoing messages
   //
   CError requestShutDown();
   CError requestUnlockAuthenticationProfile();
   CError requestUnlockProfiles();

   // Incoming messages
   // should be implemented by implementation
   virtual CError onCounterPMConnected() = 0;
   virtual CError onCounterPMDisconnected() = 0;

protected:
   // from ICallbackHandler

   virtual void OnConnection(iviLink::Ipc::DirectionID);
   virtual void OnConnectionLost(iviLink::Ipc::DirectionID);
   virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID);

   virtual void onProfileManagerAvailable() = 0;
   virtual void onProfileManagerNotAvailable() = 0;

   class CMsgIdGen
   {
   public:
      CMsgIdGen();
      ~CMsgIdGen();
      iviLink::Ipc::MsgID getNext();
   private:
      iviLink::Ipc::MsgID mId;
   };

   CMsgIdGen mMsgIdGen;

   CError connectProfileManager();

   bool isConnected() const;

   enum
   {
      BUFFER_SIZE = 4096
   };

   UInt8 mReadBuffer[BUFFER_SIZE];
   UInt8 mWriteBuffer[BUFFER_SIZE];
};

}
#endif /* PROFILE_MANAGER_MSG_PROXY_HPP */
