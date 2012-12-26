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
 

#ifndef CPMALIpcProtocol_HPP_
#define CPMALIpcProtocol_HPP_

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


class CPMALIpcProtocol : public IPMALIpcToPIM, public IPMALIpcToCore, public iviLink::Ipc::ICallbackHandler
{
public:
   // from IPMALIpcToPIM

   virtual CError generatePIUID(iviLink::Profile::IUid& piuid);

   virtual CError releasePIUID(iviLink::Profile::IUid const& piuid);

   virtual CError createProfile(iviLink::CUid const& profileUid,
         iviLink::Profile::IUid const& piuid,
         iviLink::Service::Uid const& sid);

   virtual CError profileDied(iviLink::Profile::IUid const& piuid);

   virtual CError readyToServe(iviLink::Service::SessionUid const& sesId);

public:
   // from IPMALIpcToCore

   virtual CPMALError disableByUid(iviLink::CUid const& id);

   virtual CPMALError enableByUid(iviLink::CUid const& id);

   virtual CPMALError enableAll();

   virtual CPMALError getManifest(iviLink::Profile::Uid const& uid, std::string& manifest);

   virtual CPMALError getProfileLibPath(iviLink::Profile::Uid const& uid, std::string& path);

   virtual CPMALError findProfiles(iviLink::CUid const& profileApiUid,
                  std::map<std::string, std::string> const& profileParameters,
                  std::list<iviLink::Profile::Uid> & profiles,
                  bool enabledProfiles = true);

protected:
   // from ICallbackHandler

   virtual void OnConnection(iviLink::Ipc::DirectionID);

   virtual void OnConnectionLost(iviLink::Ipc::DirectionID);

   virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID);
   virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, iviLink::Ipc::DirectionID) {}

public:

   CPMALIpcProtocol(char const* pPmpAddress = NULL);
   virtual ~CPMALIpcProtocol();

   CError connect();

   bool isConnected() const;

private:

   void processProfileDiedRequest(ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize);
   void processIncomingProfileRequest(ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize);

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

   static Logger msLogger;
};


}  // namespace Ipc
}  // namespace PMAL
}  // namespace AXIS


#endif /* CPMALIpcProtocol_HPP_ */
