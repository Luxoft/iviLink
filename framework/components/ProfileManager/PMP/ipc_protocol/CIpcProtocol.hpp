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









#ifndef CIPCPROTOCOL_HPP_
#define CIPCPROTOCOL_HPP_

#include "IPMPIpcToCore.hpp"
#include "IPMPIpcToPIM.hpp"

#include "utils/ipc/ICallbackHandler.hpp"

#include "utils/misc/Logger.hpp"
#include "utils/misc/CError.hpp"

#include "utils/ipc/helpers/CBufferManager.hpp"

namespace iviLink {
namespace Ipc {

class CIpc;

}  // namespace Ipc

namespace ProfileManager{
   namespace Ipc {

   struct Message;

   }  // namespace Ipc
}

namespace PMP {

class IPMPPIMToIpc;
class IPmpCoreToIpc;

namespace Ipc {

class CIpcProtocol : public IPMPIpcToCore, public IPMPIpcToPIM,
   public iviLink::Ipc::ICallbackHandler
{
public:

   // from IPMPIpcToPIM

   virtual CError incomingProfile(iviLink::CUid const& profileUid,
         iviLink::Profile::ApiUid const& papiUid,
         iviLink::Profile::IUid const& piuid,
         iviLink::Service::Uid const& serviceUID,
         iviLink::Ipc::DirectionID const& dirId);

   virtual CError profileDied(iviLink::Profile::IUid const& piuid,
      iviLink::Ipc::DirectionID const& dirId);

public:

   // from IPMPIpcToCore

protected:
   // from ICallbackHandler

   virtual void OnConnection(iviLink::Ipc::DirectionID);

   virtual void OnConnectionLost(iviLink::Ipc::DirectionID);

   virtual void OnRequest(iviLink::Ipc::MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      UInt8* const pResponseBuffer, UInt32& bufferSize,
      iviLink::Ipc::DirectionID);

public:

   explicit CIpcProtocol(IPMPPIMToIpc* pPim, IPmpCoreToIpc* pCore, char const* pPmpAddress = NULL);
   virtual ~CIpcProtocol();

   CError connect();

   bool isConnected() const;


private:

   void processGeneratePIUIDRequest     (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processReleasePIUIDRequest      (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processCreateProfileRequest     (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processProfileDiedRequest       (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processDisableByUidRequest      (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processEnableByUidRequest       (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processEnableAllRequest         (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processGetManifestRequest       (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processGetProfileLibPathRequest (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processFindProfilesRequest      (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processReadyToServeRequest      (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);

private:

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

   iviLink::Ipc::CIpc* mpIpc;

   iviLink::Ipc::Helpers::CBufferManager mReadBufMgr;
   iviLink::Ipc::Helpers::CBufferManager mWriteBufMgr;

   IPMPPIMToIpc* mpPim;
   IPmpCoreToIpc* mpCore;

   static Logger msLogger;
};

}  // namespace Ipc
}  // namespace ProfileManager
}  // namespace AXIS




#endif /* CIPCPROTOCOL_HPP_ */
