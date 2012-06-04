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





#ifndef CIPCPROTOCOL_HPP_
#define CIPCPROTOCOL_HPP_

#include "IPMPIpcToCore.hpp"
#include "IPMPIpcToPIM.hpp"

#include "utils/ipc/include/ICallbackHandler.hpp"

#include "utils/misc/include/Logger.hpp"
#include "utils/misc/include/CError.hpp"

#include "framework/components/ProfileManager/IpcProtocol/include/CBufferManager.hpp"

namespace AXIS {
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
   public AXIS::Ipc::ICallbackHandler
{
public:

   // from IPMPIpcToPIM

   virtual CError incomingProfile(AXIS::CUid const& profileUid,
         AXIS::Profile::ApiUid const& papiUid,
         AXIS::Profile::IUid const& piuid,
         AXIS::Service::Uid const& serviceUID,
         AXIS::Ipc::DirectionID const& dirId);

   virtual CError profileDied(AXIS::Profile::IUid const& piuid,
      AXIS::Ipc::DirectionID const& dirId);

public:

   // from IPMPIpcToCore

protected:
   // from ICallbackHandler

   virtual void OnConnection(AXIS::Ipc::DirectionID);

   virtual void OnConnectionLost(AXIS::Ipc::DirectionID);

   virtual void OnRequest(AXIS::Ipc::MsgID id, 
      UInt8 const* pPayload, UInt32 payloadSize, 
      UInt8* const pResponseBuffer, UInt32& bufferSize,
      AXIS::Ipc::DirectionID);

public:

   explicit CIpcProtocol(IPMPPIMToIpc* pPim, IPmpCoreToIpc* pCore, char const* pPmpAddress = NULL);
   virtual ~CIpcProtocol();

   CError connect();

   bool isConnected() const;


private:

   void processGeneratePIUIDRequest     (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId);
   void processReleasePIUIDRequest      (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId);
   void processCreateProfileRequest     (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId);
   void processProfileDiedRequest       (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId);
   void processDisableByUidRequest      (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId);
   void processEnableByUidRequest       (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId);
   void processEnableAllRequest         (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId);
   void processGetManifestRequest       (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId);
   void processGetProfileLibPathRequest (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId);
   void processFindProfilesRequest      (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId);
   void processReadyToServeRequest      (ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId);

private:

   class CMsgIdGen
   {
   public:
      CMsgIdGen();
      ~CMsgIdGen();
      AXIS::Ipc::MsgID getNext();
   private:
      AXIS::Ipc::MsgID mId;
   };
   CMsgIdGen mMsgIdGen;

   AXIS::Ipc::CIpc* mpIpc;

   ProfileManager::Ipc::Helpers::CBufferManager mReadBufMgr;
   ProfileManager::Ipc::Helpers::CBufferManager mWriteBufMgr;

   IPMPPIMToIpc* mpPim;
   IPmpCoreToIpc* mpCore;

   static Logger msLogger;
};

}  // namespace Ipc
}  // namespace ProfileManager
}  // namespace AXIS




#endif /* CIPCPROTOCOL_HPP_ */
