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
 

#include <cassert>


#include "CPMALError.hpp"
#include "CComponentMgr.hpp"
#include "IPMALPIMToIpc.hpp"

#include "ProfileManagerIpcCommon.hpp"
#include "PmMessage.hpp"
#include "buffer_helpers.hpp"

#include "CIpc.hpp"

#include "CPMALIpcProtocol.hpp"


static const char gModuleName[] = "CPMALIpcProtocol";

using iviLink::Ipc::CIpc;
using iviLink::Ipc::Helpers::CBufferManager;
using iviLink::Ipc::Helpers::CBuffer;

namespace iviLink {
namespace PMAL {
namespace Ipc {

   Logger CPMALIpcProtocol::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMAL.CPMALIpcProtocol"));


CPMALIpcProtocol::CMsgIdGen::CMsgIdGen() :
      mId(-1)
{
}

CPMALIpcProtocol::CMsgIdGen::~CMsgIdGen()
{

}

iviLink::Ipc::MsgID CPMALIpcProtocol::CMsgIdGen::getNext()
{
   mId += 2;
   return mId;
}


///

CError CPMALIpcProtocol::generatePIUID(iviLink::Profile::IUid& piuid)
{
   if (!mpIpc)
      return CError(1, gModuleName, CError::FATAL, "no ipc");

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_GENERATE_PIUID;
   req->header.size = 0;

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   UInt32 respSize = readBuf.getSize();
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, readBuf.get(), respSize);
   if (!err.isNoError())
      return err;

   ProfileManager::Ipc::PmMessage* resp = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(readBuf.get());
   if (resp->header.type != ProfileManager::Ipc::PMP_PMAL_GENERATE_PIUID)
      return CError(1, gModuleName, CError::ERROR, "wrong response");
   if (resp->header.size == 0)
      return CError(1, gModuleName, CError::ERROR, "empty uid");

   {
      iviLink::Ipc::Helpers::CBufferReader reader(resp->data, resp->header.size);
      CError err = reader.read(piuid);
      if (!err.isNoError()) return err;
   }

   return CError::NoError(gModuleName);
}

CError CPMALIpcProtocol::releasePIUID(iviLink::Profile::IUid const& piuid)
{
   if (!mpIpc)
      return CError(1, gModuleName, CError::FATAL, "no ipc");

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_RELEASE_PIUID;

   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));
      CError err = writer.write(piuid);
      if (!err.isNoError()) return err;

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   UInt32 respSize = 0;
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, NULL, respSize);
   if (!err.isNoError())
      return err;

   return CError::NoError(gModuleName);
}

CError CPMALIpcProtocol::createProfile(iviLink::CUid const& profileUid,
      iviLink::Profile::IUid const& piuid,
      iviLink::Service::Uid const& sid)
{
   if (!mpIpc)
      return CError(1, gModuleName, CError::FATAL, "no ipc");

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_CREATE_PROFILE;

   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(profileUid);
      if (!err.isNoError()) return err;

      err = writer.write(piuid);
      if (!err.isNoError()) return err;

      err = writer.write(sid);
      if (!err.isNoError()) return err;

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   UInt32 respSize = 0;
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, NULL, respSize);
   if (!err.isNoError())
      return err;

   return CError::NoError(gModuleName);
}

CError CPMALIpcProtocol::profileDied(iviLink::Profile::IUid const& piuid)
{
   if (!mpIpc)
      return CError(1, gModuleName, CError::FATAL, "no ipc");

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_PROFILE_DIED;

   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(piuid);
      if (!err.isNoError()) return err;

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   UInt32 respSize = 0;
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, NULL, respSize);
   if (!err.isNoError())
      return err;

   return CError::NoError(gModuleName);
}

CError CPMALIpcProtocol::readyToServe(iviLink::Service::SessionUid const& sesId)
{
   if (!mpIpc)
      return CError(1, gModuleName, CError::FATAL, "no ipc");

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_READY_TO_SERVE;

   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(sesId);
      if (!err.isNoError()) return err;

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   UInt32 respSize = 0;
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, NULL, respSize);
   if (!err.isNoError())
      return err;

   return CError::NoError(gModuleName);
}

CPMALError CPMALIpcProtocol::disableByUid(iviLink::Profile::Uid const& uid)
{
   if (!mpIpc)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "Not implemented", CError::FATAL);

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_DISABLE_BY_UID;
   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(uid);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   UInt32 respSize = readBuf.getSize();
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, readBuf.get(), respSize);
   if (!err.isNoError())
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

   ProfileManager::Ipc::PmMessage* resp = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(readBuf.get());
   if (resp->header.type != ProfileManager::Ipc::PMP_PMAL_DISABLE_BY_UID)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "wrong response");
   if (resp->header.size == 0)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "empty manifest");

   {
      iviLink::Ipc::Helpers::CBufferReader reader(resp->data, resp->header.size);
      bool result = false;
      CError err = reader.read(result);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());
      if (!result)
         return CPMALError(CPMALError::ERROR_UNKNOWN_PROFILE_UID, gModuleName);
   }

   return CPMALError::NoPMALError(gModuleName);
}

CPMALError CPMALIpcProtocol::enableByUid(iviLink::Profile::Uid const& uid)
{
   if (!mpIpc)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "Not implemented", CError::FATAL);

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_ENABLE_BY_UID;
   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(uid);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   UInt32 respSize = readBuf.getSize();
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, readBuf.get(), respSize);
   if (!err.isNoError())
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

   ProfileManager::Ipc::PmMessage* resp = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(readBuf.get());
   if (resp->header.type != ProfileManager::Ipc::PMP_PMAL_ENABLE_BY_UID)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "wrong response");
   if (resp->header.size == 0)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "empty manifest");

   {
      iviLink::Ipc::Helpers::CBufferReader reader(resp->data, resp->header.size);
      bool result = false;
      CError err = reader.read(result);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());
      if (!result)
         return CPMALError(CPMALError::ERROR_UNKNOWN_PROFILE_UID, gModuleName);
   }

   return CPMALError::NoPMALError(gModuleName);
}

CPMALError CPMALIpcProtocol::enableAll()
{
   if (!mpIpc)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "Not implemented", CError::FATAL);

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_ENABLE_ALL;
   req->header.size = 0;

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   UInt32 respSize = 0;
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, NULL, respSize);
   if (!err.isNoError())
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

   return CPMALError::NoPMALError(gModuleName);
}

CPMALError CPMALIpcProtocol::getManifest(iviLink::CUid const& uid, std::string& manifest)
{
   if (!mpIpc)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "Not implemented", CError::FATAL);

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_GET_MANIFEST;
   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(uid);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   UInt32 respSize = readBuf.getSize();
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, readBuf.get(), respSize);
   if (!err.isNoError())
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

   ProfileManager::Ipc::PmMessage* resp = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(readBuf.get());
   if (resp->header.type != ProfileManager::Ipc::PMP_PMAL_GET_MANIFEST)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "wrong response");
   if (resp->header.size == 0)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "empty manifest");

   {
      iviLink::Ipc::Helpers::CBufferReader reader(resp->data, resp->header.size);

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

CPMALError CPMALIpcProtocol::getProfileLibPath(iviLink::Profile::Uid const& uid, std::string& path)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   if (!mpIpc)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "Not implemented", CError::FATAL);

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_GET_PROFILE_LIB_PATH;
   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(uid);
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   UInt32 respSize = readBuf.getSize();
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, readBuf.get(), respSize);
   if (!err.isNoError())
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

   ProfileManager::Ipc::PmMessage* resp = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(readBuf.get());
   if (resp->header.type != ProfileManager::Ipc::PMP_PMAL_GET_PROFILE_LIB_PATH)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "wrong response");
   if (resp->header.size == 0)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "empty path");

   {
      iviLink::Ipc::Helpers::CBufferReader reader(resp->data, resp->header.size);

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

CPMALError CPMALIpcProtocol::findProfiles(iviLink::CUid const& profileApiUid,
               std::map<std::string, std::string> const& profileParameters,
               std::list<iviLink::Profile::Uid> & profiles,
               bool enabledProfiles/* = true*/)
{
   if (!mpIpc)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "Not implemented", CError::FATAL);

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_FIND_PROFILES;
   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

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

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   UInt32 respSize = readBuf.getSize();
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, readBuf.get(), respSize);
   if (!err.isNoError())
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());

   ProfileManager::Ipc::PmMessage* resp = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(readBuf.get());
   if (resp->header.type != ProfileManager::Ipc::PMP_PMAL_FIND_PROFILES)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "wrong response");
   if (resp->header.size == 0)
      return CPMALError(CPMALError::ERROR_OTHER, gModuleName, "no profiles");

   {
      iviLink::Ipc::Helpers::CBufferReader reader(resp->data, resp->header.size);

      CError err = reader.readContainer(std::back_inserter(profiles));
      if (!err.isNoError())
         return CPMALError(CPMALError::ERROR_OTHER, gModuleName, err.getDescription());
   }

   return CPMALError::NoPMALError(gModuleName);
}

void CPMALIpcProtocol::OnConnection(iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CPMALIpcProtocol::OnConnectionLost(iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CPMALIpcProtocol::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   ProfileManager::Ipc::PmMessage const* req = reinterpret_cast<ProfileManager::Ipc::PmMessage const*>(pPayload);

   assert(req->header.size + sizeof(ProfileManager::Ipc::PmMessage) == payloadSize);
   assert(bufferSize >= sizeof(ProfileManager::Ipc::PmMessage));

   ProfileManager::Ipc::PmMessage* resp = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(pResponseBuffer);

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


CPMALIpcProtocol::CPMALIpcProtocol(char const* pPmpAddress/* = NULL*/) :
      mpIpc(NULL)
{
   if (!pPmpAddress)
      pPmpAddress = PMP_ADDRESS;
   mpIpc = new CIpc(pPmpAddress, *this);
}

CPMALIpcProtocol::~CPMALIpcProtocol()
{
   delete mpIpc;
}

CError CPMALIpcProtocol::connect()
{
   return mpIpc->connect();
}

bool CPMALIpcProtocol::isConnected() const
{
   return mpIpc->isConnected();
}


void CPMALIpcProtocol::processProfileDiedRequest(ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize)
{
   iviLink::Profile::IUid piuid;

   bufferSize = 0;

   {
      iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
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

void CPMALIpcProtocol::processIncomingProfileRequest(ProfileManager::Ipc::PmMessage const* const req, ProfileManager::Ipc::PmMessage* const resp, UInt32& bufferSize)
{
   iviLink::Profile::Uid uid;
   iviLink::Profile::ApiUid papiUid;
   iviLink::Profile::IUid piuid;
   iviLink::Service::Uid sid;

   bufferSize = 0;

   {
      iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
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
