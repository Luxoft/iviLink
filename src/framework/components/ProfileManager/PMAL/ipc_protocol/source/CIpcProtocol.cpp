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





#include <cassert>


#include "framework/components/ProfileManager/PMAL/include/CPMALError.hpp"
#include "framework/components/ProfileManager/PMAL/include/CComponentMgr.hpp"

#include "framework/components/ProfileManager/PMAL/ipc_protocol/include/CIpcProtocol.hpp"

#include "framework/components/ProfileManager/PMAL/PIM/include/IPMALPIMToIpc.hpp"

#include "framework/components/ProfileManager/IpcProtocol/include/common.hpp"
#include "utils/ipc/include/CIpc.hpp"

#include "framework/components/ProfileManager/IpcProtocol/include/message.hpp"
#include "framework/components/ProfileManager/IpcProtocol/include/buffer_helpers.hpp"



static const char gModuleName[] = "CIpcProtocol";

using AXIS::Ipc::CIpc;
using AXIS::ProfileManager::Ipc::Helpers::CBufferManager;
using AXIS::ProfileManager::Ipc::Helpers::CBuffer;

namespace AXIS {
namespace PMAL {
namespace Ipc {

   Logger CIpcProtocol::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMAL.CIpcProtocol"));


CIpcProtocol::CMsgIdGen::CMsgIdGen() :
      mId(-1)
{
}

CIpcProtocol::CMsgIdGen::~CMsgIdGen()
{

}

AXIS::Ipc::MsgID CIpcProtocol::CMsgIdGen::getNext()
{
   mId += 2;
   return mId;
}


///

CError CIpcProtocol::generatePIUID(AXIS::Profile::IUid& piuid)
{
   if (!mpIpc)
      return CError(1, gModuleName, CError::FATAL, "no ipc");

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_GENERATE_PIUID;
   req->header.size = 0;

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = readBuf.getSize();
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, readBuf.get(), respSize);
   if (!err.isNoError())
      return err;

   ProfileManager::Ipc::Message* resp = reinterpret_cast<ProfileManager::Ipc::Message*>(readBuf.get());
   if (resp->header.type != ProfileManager::Ipc::PMP_PMAL_GENERATE_PIUID)
      return CError(1, gModuleName, CError::ERROR, "wrong response");
   if (resp->header.size == 0)
      return CError(1, gModuleName, CError::ERROR, "empty uid");

   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(resp->data, resp->header.size);
      CError err = reader.read(piuid);
      if (!err.isNoError()) return err;
   }

   return CError::NoError(gModuleName);
}

CError CIpcProtocol::releasePIUID(AXIS::Profile::IUid const& piuid)
{
   if (!mpIpc)
      return CError(1, gModuleName, CError::FATAL, "no ipc");

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_RELEASE_PIUID;

   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));
      CError err = writer.write(piuid);
      if (!err.isNoError()) return err;

      req->header.size = writer.getUsedSize();
   }

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = 0;
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, NULL, respSize);
   if (!err.isNoError())
      return err;

   return CError::NoError(gModuleName);
}

CError CIpcProtocol::createProfile(AXIS::CUid const& profileUid,
      AXIS::Profile::IUid const& piuid,
      AXIS::Service::Uid const& sid)
{
   if (!mpIpc)
      return CError(1, gModuleName, CError::FATAL, "no ipc");

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_CREATE_PROFILE;

   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(profileUid);
      if (!err.isNoError()) return err;

      err = writer.write(piuid);
      if (!err.isNoError()) return err;

      err = writer.write(sid);
      if (!err.isNoError()) return err;

      req->header.size = writer.getUsedSize();
   }

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = 0;
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, NULL, respSize);
   if (!err.isNoError())
      return err;

   return CError::NoError(gModuleName);
}

CError CIpcProtocol::profileDied(AXIS::Profile::IUid const& piuid)
{
   if (!mpIpc)
      return CError(1, gModuleName, CError::FATAL, "no ipc");

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_PROFILE_DIED;

   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(piuid);
      if (!err.isNoError()) return err;

      req->header.size = writer.getUsedSize();
   }

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = 0;
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, NULL, respSize);
   if (!err.isNoError())
      return err;

   return CError::NoError(gModuleName);
}

CError CIpcProtocol::readyToServe(AXIS::Service::SessionUid const& sesId)
{
   if (!mpIpc)
      return CError(1, gModuleName, CError::FATAL, "no ipc");

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_READY_TO_SERVE;

   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(sesId);
      if (!err.isNoError()) return err;

      req->header.size = writer.getUsedSize();
   }

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = 0;
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, NULL, respSize);
   if (!err.isNoError())
      return err;

   return CError::NoError(gModuleName);
}

CPMALError CIpcProtocol::disableByUid(AXIS::Profile::Uid const& uid)
{
   if (!mpIpc)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "Not implemented", CError::FATAL);

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_DISABLE_BY_UID;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(uid);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

      req->header.size = writer.getUsedSize();
   }

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = readBuf.getSize();
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, readBuf.get(), respSize);
   if (!err.isNoError())
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

   ProfileManager::Ipc::Message* resp = reinterpret_cast<ProfileManager::Ipc::Message*>(readBuf.get());
   if (resp->header.type != ProfileManager::Ipc::PMP_PMAL_DISABLE_BY_UID)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "wrong response");
   if (resp->header.size == 0)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "empty manifest");

   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(resp->data, resp->header.size);
      bool result = false;
      CError err = reader.read(result);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());
      if (!result)
         return CPMALError(CPMALError::ERROR_UNKNOWN_PROFILE_UID, gModuleName);
   }

   return CPMALError::NoPMALError(gModuleName);
}

CPMALError CIpcProtocol::enableByUid(AXIS::Profile::Uid const& uid)
{
   if (!mpIpc)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "Not implemented", CError::FATAL);

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_ENABLE_BY_UID;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(uid);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

      req->header.size = writer.getUsedSize();
   }

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = readBuf.getSize();
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, readBuf.get(), respSize);
   if (!err.isNoError())
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

   ProfileManager::Ipc::Message* resp = reinterpret_cast<ProfileManager::Ipc::Message*>(readBuf.get());
   if (resp->header.type != ProfileManager::Ipc::PMP_PMAL_ENABLE_BY_UID)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "wrong response");
   if (resp->header.size == 0)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "empty manifest");

   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(resp->data, resp->header.size);
      bool result = false;
      CError err = reader.read(result);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());
      if (!result)
         return CPMALError(CPMALError::ERROR_UNKNOWN_PROFILE_UID, gModuleName);
   }

   return CPMALError::NoPMALError(gModuleName);
}

CPMALError CIpcProtocol::enableAll()
{
   if (!mpIpc)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "Not implemented", CError::FATAL);

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_ENABLE_ALL;
   req->header.size = 0;

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = 0;
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, NULL, respSize);
   if (!err.isNoError())
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

   return CPMALError::NoPMALError(gModuleName);
}

CPMALError CIpcProtocol::getManifest(AXIS::CUid const& uid, std::string& manifest)
{
   if (!mpIpc)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "Not implemented", CError::FATAL);

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_GET_MANIFEST;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(uid);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

      req->header.size = writer.getUsedSize();
   }

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = readBuf.getSize();
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, readBuf.get(), respSize);
   if (!err.isNoError())
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

   ProfileManager::Ipc::Message* resp = reinterpret_cast<ProfileManager::Ipc::Message*>(readBuf.get());
   if (resp->header.type != ProfileManager::Ipc::PMP_PMAL_GET_MANIFEST)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "wrong response");
   if (resp->header.size == 0)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "empty manifest");

   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(resp->data, resp->header.size);

      bool result = false;
      CError err = reader.read(result);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());
      if (!result)
         return CPMALError(CPMALError::ERROR_UNKNOWN_PROFILE_UID, gModuleName);

      err = reader.read(manifest);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());
   }

   return CPMALError::NoPMALError(gModuleName);
}

CPMALError CIpcProtocol::getProfileLibPath(AXIS::Profile::Uid const& uid, std::string& path)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   if (!mpIpc)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "Not implemented", CError::FATAL);

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_GET_PROFILE_LIB_PATH;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(uid);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

      req->header.size = writer.getUsedSize();
   }

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = readBuf.getSize();
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, readBuf.get(), respSize);
   if (!err.isNoError())
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

   ProfileManager::Ipc::Message* resp = reinterpret_cast<ProfileManager::Ipc::Message*>(readBuf.get());
   if (resp->header.type != ProfileManager::Ipc::PMP_PMAL_GET_PROFILE_LIB_PATH)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "wrong response");
   if (resp->header.size == 0)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "empty path");

   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(resp->data, resp->header.size);

      bool result = false;
      CError err = reader.read(result);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());
      if (!result)
         return CPMALError(CPMALError::ERROR_UNKNOWN_PROFILE_UID, gModuleName);

      err = reader.read(path);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());
   }

   return CPMALError::NoPMALError(gModuleName);
}

CPMALError CIpcProtocol::findProfiles(AXIS::CUid const& profileApiUid,
               std::map<std::string, std::string> const& profileParameters,
               std::list<AXIS::Profile::Uid> & profiles,
               bool enabledProfiles/* = true*/)
{
   if (!mpIpc)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "Not implemented", CError::FATAL);

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_FIND_PROFILES;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(profileApiUid);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());
      err = writer.write(profileParameters.begin(), profileParameters.end());
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());
      err = writer.write(enabledProfiles);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

      req->header.size = writer.getUsedSize();
   }

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = readBuf.getSize();
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, readBuf.get(), respSize);
   if (!err.isNoError())
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

   ProfileManager::Ipc::Message* resp = reinterpret_cast<ProfileManager::Ipc::Message*>(readBuf.get());
   if (resp->header.type != ProfileManager::Ipc::PMP_PMAL_FIND_PROFILES)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "wrong response");
   if (resp->header.size == 0)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "no profiles");

   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(resp->data, resp->header.size);
      CError err = reader.readContainer(std::back_inserter(profiles));
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());
   }

   return CPMALError::NoPMALError(gModuleName);
}

void CIpcProtocol::OnConnection(AXIS::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CIpcProtocol::OnConnectionLost(AXIS::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CIpcProtocol::OnRequest(AXIS::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, AXIS::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   ProfileManager::Ipc::Message const* req = reinterpret_cast<ProfileManager::Ipc::Message const*>(pPayload);

   assert(req->header.size + sizeof(ProfileManager::Ipc::Message) == payloadSize);
   assert(bufferSize >= sizeof(ProfileManager::Ipc::Message));

   ProfileManager::Ipc::Message* resp = reinterpret_cast<ProfileManager::Ipc::Message*>(pResponseBuffer);

   switch (req->header.type)
   {
   case ProfileManager::Ipc::PMP_PMAL_PROFILE_DIED:
      processProfileDiedRequest(req, resp, bufferSize);
      break;
   case ProfileManager::Ipc::PMP_PMAL_INCOMMING_PROFILE:
      processIncomingProfileRequest(req, resp, bufferSize);
      break;
   default:
      CPMALError err(CPMALError::ERROR_OTHER, gModuleName, "Unknown request");
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      bufferSize = 0;
      break;
   }
}


CIpcProtocol::CIpcProtocol(char const* pPmpAddress/* = NULL*/) :
      mpIpc(NULL)
{
   if (!pPmpAddress)
      pPmpAddress = PMP_ADDRESS;
   mpIpc = new CIpc(pPmpAddress, *this);
}

CIpcProtocol::~CIpcProtocol()
{
   delete mpIpc;
}

CError CIpcProtocol::connect()
{
   return mpIpc->connect();
}

bool CIpcProtocol::isConnected() const
{
   return mpIpc->isConnected();
}


void CIpcProtocol::processProfileDiedRequest(ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize)
{
   AXIS::Profile::IUid piuid;

   bufferSize = 0;

   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
      CError err = reader.read(piuid);
      if (!err.isNoError()) return;
   }

   CPMALComponentMgr* mgr = CPMALComponentMgr::getInstance();
   if (!mgr)
      return;
   IPMALPIMToIpc* pim = mgr->getPIMToIpc();
   if (!pim)
      return;
   pim->profileDied(piuid);
}

void CIpcProtocol::processIncomingProfileRequest(ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize)
{
   AXIS::Profile::Uid uid;
   AXIS::Profile::ApiUid papiUid;
   AXIS::Profile::IUid piuid;
   AXIS::Service::Uid sid;

   bufferSize = 0;

   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
      CError err = reader.read(uid);
      if (!err.isNoError()) return;
      err = reader.read(papiUid);
      if (!err.isNoError()) return;
      err = reader.read(piuid);
      if (!err.isNoError()) return;
      err = reader.read(sid);
      if (!err.isNoError()) return;
   }

   CPMALComponentMgr* mgr = CPMALComponentMgr::getInstance();
   if (!mgr)
      return;
   IPMALPIMToIpc* pim = mgr->getPIMToIpc();
   if (!pim)
      return;
   pim->incomingProfile(uid, papiUid, piuid, sid);
}



}  // namespace PMAL_Ipc
}  // namespace ProfileManager
}  // namespace AXIS
