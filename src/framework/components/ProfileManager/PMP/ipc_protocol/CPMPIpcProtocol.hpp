/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#ifndef CPMPIpcProtocol_HPP_
#define CPMPIpcProtocol_HPP_

#include "IPMPIpcToCore.hpp"
#include "IPMPIpcToPIM.hpp"

#include "ICallbackHandler.hpp"

#include "Logger.hpp"
#include "CError.hpp"

#include "CBufferManager.hpp"

namespace iviLink {
namespace Ipc {

class CIpc;

}  // namespace Ipc

namespace ProfileManager{
   namespace Ipc {

   struct PmMessage;

   }  // namespace Ipc
}

namespace PMP {

class IPMPPIMToIpc;
class IPmpCoreToIpc;

namespace Ipc {

class CPMPIpcProtocol : public IPMPIpcToCore, public IPMPIpcToPIM,
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
   virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      iviLink::Ipc::DirectionID) {}

public:

   explicit CPMPIpcProtocol(IPMPPIMToIpc* pPim, IPmpCoreToIpc* pCore, char const* pPmpAddress = NULL);
   virtual ~CPMPIpcProtocol();

   CError connect();

   bool isConnected() const;


private:

   void processGeneratePIUIDRequest     (ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processReleasePIUIDRequest      (ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processCreateProfileRequest     (ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processProfileDiedRequest       (ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processDisableByUidRequest      (ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processEnableByUidRequest       (ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processEnableAllRequest         (ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processGetManifestRequest       (ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processGetProfileLibPathRequest (ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processFindProfilesRequest      (ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);
   void processReadyToServeRequest      (ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize, iviLink::Ipc::DirectionID const& dirId);

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


#endif /* CPMPIpcProtocol_HPP_ */
