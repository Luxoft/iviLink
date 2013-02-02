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


#ifndef CIPCPROTOCOL_HPP_
#define CIPCPROTOCOL_HPP_

#include "Logger.hpp"

#include "IPMALIpcToPIM.hpp"
#include "IPMALIpcToCore.hpp"
#include "ICallbackHandler.hpp"
#include "CBufferManager.hpp"

namespace iviLink {
namespace Ipc {

class CIpc;

}  // namespace Ipc

namespace ProfileManager {

namespace Ipc {

struct PmMessage;

}  // namespace Ipc

} // namespace ProfileManager

namespace PMAL {

namespace Ipc {

struct SyncRequestsData;

class CIpcProtocol : public IPMALIpcToPIM, public IPMALIpcToCore, public iviLink::Ipc::ICallbackHandler
{
public:
   // from IPMALIpcToPIM

   virtual bool generatePIUID(iviLink::Profile::IUid& piuid);

   virtual bool releasePIUID(iviLink::Profile::IUid const& piuid);

   virtual bool createProfile(iviLink::BaseUid const& profileUid,
         iviLink::Profile::IUid const& piuid,
         iviLink::Service::Uid const& sid);

   virtual bool profileDied(iviLink::Profile::IUid const& piuid);

   virtual bool readyToServe(iviLink::Service::SessionUid const& sesId);

public:
   // from IPMALIpcToCore

   virtual bool disableByUid(iviLink::BaseUid const& id);

   virtual bool enableByUid(iviLink::BaseUid const& id);

   virtual bool enableAll();

   virtual bool getManifest(iviLink::Profile::Uid const& uid, std::string& manifest);

   virtual bool getProfileLibPath(iviLink::Profile::Uid const& uid, std::string& path);

   virtual bool findProfiles(iviLink::BaseUid const& profileApiUid,
                  std::map<std::string, std::string> const& profileParameters,
                  std::list<iviLink::Profile::Uid> & profiles,
                  bool enabledProfiles);

protected:
   // from ICallbackHandler

   virtual void OnConnection(iviLink::Ipc::DirectionID);

   virtual void OnConnectionLost(iviLink::Ipc::DirectionID);

   virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID);
   virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, iviLink::Ipc::DirectionID);

public:

   CIpcProtocol(char const* pPmpAddress = NULL);
   virtual ~CIpcProtocol();

   BaseError connect();

   bool isConnected() const;

private:

   void processProfileDiedRequest       (ProfileManager::Ipc::PmMessage const* const req);
   void processIncomingProfileRequest   (ProfileManager::Ipc::PmMessage const* const req);

   /// responses for syc functions
   void processGeneratePIUidResponse    (ProfileManager::Ipc::PmMessage const* const req);
   void processGetManifestResponse      (ProfileManager::Ipc::PmMessage const* const req);
   void processGetProfileLibPathResponse(ProfileManager::Ipc::PmMessage const* const req);
   void processFindProfilesResponse     (ProfileManager::Ipc::PmMessage const* const req);

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
   SyncRequestsData & mSyncData;

   static Logger msLogger;
};


}  // namespace Ipc
}  // namespace PMAL
}  // namespace AXIS


#endif /* CIPCPROTOCOL_HPP_ */
