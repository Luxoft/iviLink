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

/*
 * CIpcProtocol.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: pilin
 */

#include <cassert>

#include "framework/components/ProfileManager/PMP/PIM/include/IPMPPIMToIpc.hpp"
#include "framework/components/ProfileManager/PMP/core/include/IPmpCoreToIpc.hpp"

#include "framework/components/ProfileManager/PMP/ipc_protocol/include/CIpcProtocol.hpp"

#include "framework/components/ProfileManager/IpcProtocol/include/common.hpp"
#include "framework/components/ProfileManager/IpcProtocol/include/message.hpp"
#include "framework/components/ProfileManager/IpcProtocol/include/buffer_helpers.hpp"

#include "utils/ipc/include/CIpc.hpp"



static const char gModuleName[] = "CIpcProtocol";

using AXIS::Ipc::CIpc;
using AXIS::ProfileManager::Ipc::Helpers::CBufferManager;
using AXIS::ProfileManager::Ipc::Helpers::CBuffer;

namespace AXIS {
namespace PMP {
namespace Ipc {

Logger CIpcProtocol::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.IPC.CIpcProtocol"));


CIpcProtocol::CMsgIdGen::CMsgIdGen() :
      mId(0)
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


CError CIpcProtocol::incomingProfile(AXIS::CUid const& profileUid,
         AXIS::Profile::ApiUid const& papiUid,
         AXIS::Profile::IUid const& piuid,
         AXIS::Service::Uid const& serviceUID,
         AXIS::Ipc::DirectionID const& dirId)
{
   if (!mpIpc)
      return CError(1, gModuleName, CError::FATAL, "no ipc");

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMP_PMAL_INCOMMING_PROFILE;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(profileUid);
      if (!err.isNoError())
         return CError(1, gModuleName, CError::ERROR, err.getDescription());

      err = writer.write(papiUid);
      if (!err.isNoError())
         return CError(1, gModuleName, CError::ERROR, err.getDescription());

      err = writer.write(piuid);
      if (!err.isNoError())
         return CError(1, gModuleName, CError::ERROR, err.getDescription());

      err = writer.write(serviceUID);
      if (!err.isNoError())
         return CError(1, gModuleName, CError::ERROR, err.getDescription());

      req->header.size = writer.getUsedSize();
   }

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = 0;
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, NULL, respSize);
   if (!err.isNoError())
      return CError(1, gModuleName, CError::ERROR, err.getDescription());

   return CError::NoError(gModuleName);
}

CError CIpcProtocol::profileDied(AXIS::Profile::IUid const& piuid,
   AXIS::Ipc::DirectionID const& dirId)
{
   if (!mpIpc)
      return CError(1, gModuleName, CError::FATAL, "no ipc");

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::Message* req = reinterpret_cast<ProfileManager::Ipc::Message*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMP_PMAL_PROFILE_DIED;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      CError err = writer.write(piuid);
      if (!err.isNoError())
         return CError(1, gModuleName, CError::ERROR, err.getDescription());

      req->header.size = writer.getUsedSize();
   }

   AXIS::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::Message) + req->header.size;
   UInt32 respSize = 0;
   CError err = mpIpc->request(id, writeBuf.get(), reqSize, NULL, respSize);
   if (!err.isNoError())
      return CError(1, gModuleName, CError::ERROR, err.getDescription());

   return CError::NoError(gModuleName);
}


///

void CIpcProtocol::OnConnection(AXIS::Ipc::DirectionID dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   if (mpPim)
   {
      mpPim->onConnection(dirId);
   }
   else
   {
      LOG4CPLUS_WARN(msLogger, "OnConnection - no PIM");
   }
}

void CIpcProtocol::OnConnectionLost(AXIS::Ipc::DirectionID dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   if (mpPim)
   {
      mpPim->onConnectionLost(dirId);
   }
   else
   {
      LOG4CPLUS_WARN(msLogger, "OnConnectionLost - no PIM");
   }
}

void CIpcProtocol::OnRequest(AXIS::Ipc::MsgID id, 
   UInt8 const* pPayload, UInt32 payloadSize, 
   UInt8* const pResponseBuffer, UInt32& bufferSize,
   AXIS::Ipc::DirectionID dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   ProfileManager::Ipc::Message const* req = reinterpret_cast<ProfileManager::Ipc::Message const*>(pPayload);

   assert(req->header.size + sizeof(ProfileManager::Ipc::Message) == payloadSize);
   assert(bufferSize >= sizeof(ProfileManager::Ipc::Message));

   ProfileManager::Ipc::Message* resp = reinterpret_cast<ProfileManager::Ipc::Message*>(pResponseBuffer);

   switch (static_cast<ProfileManager::Ipc::ePmalPmpMessages>(req->header.type))
   {
   case ProfileManager::Ipc::PMAL_PMP_GENERATE_PIUID:
      processGeneratePIUIDRequest(req, resp, bufferSize, dirId);
      break;
   case ProfileManager::Ipc::PMAL_PMP_RELEASE_PIUID:
      processReleasePIUIDRequest(req, resp, bufferSize, dirId);
      break;
   case ProfileManager::Ipc::PMAL_PMP_CREATE_PROFILE:
      processCreateProfileRequest(req, resp, bufferSize, dirId);
      break;
   case ProfileManager::Ipc::PMAL_PMP_PROFILE_DIED:
      processProfileDiedRequest(req, resp, bufferSize, dirId);
      break;
   case ProfileManager::Ipc::PMAL_PMP_DISABLE_BY_UID:
      processDisableByUidRequest(req, resp, bufferSize, dirId);
      break;
   case ProfileManager::Ipc::PMAL_PMP_ENABLE_BY_UID:
      processEnableByUidRequest(req, resp, bufferSize, dirId);
      break;
   case ProfileManager::Ipc::PMAL_PMP_ENABLE_ALL:
      processEnableAllRequest(req, resp, bufferSize, dirId);
      break;
   case ProfileManager::Ipc::PMAL_PMP_GET_MANIFEST:
      processGetManifestRequest(req, resp, bufferSize, dirId);
      break;
   case ProfileManager::Ipc::PMAL_PMP_GET_PROFILE_LIB_PATH:
      processGetProfileLibPathRequest(req, resp, bufferSize, dirId);
      break;
   case ProfileManager::Ipc::PMAL_PMP_FIND_PROFILES:
      processFindProfilesRequest(req, resp, bufferSize, dirId);
      break;
   case ProfileManager::Ipc::PMAL_PMP_READY_TO_SERVE:
      processReadyToServeRequest(req, resp, bufferSize, dirId);
      break;
   }
}


CIpcProtocol::CIpcProtocol(IPMPPIMToIpc* pPim, IPmpCoreToIpc* pCore, char const* pPmpAddress/* = NULL*/) :
      mpIpc(NULL),
      mpPim(pPim),
      mpCore(pCore)
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
   return mpIpc->beginWaitForConnection();
}

bool CIpcProtocol::isConnected() const
{
   return mpIpc->isConnected();
}

void CIpcProtocol::processGeneratePIUIDRequest(AXIS::ProfileManager::Ipc::Message const* const req, AXIS::ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if (!mpPim)
   {
      bufferSize = 0;
      return;
   }

   AXIS::Profile::IUid piuid;
   mpPim->generatePIUID(piuid, dirId);

   resp->header.type = ProfileManager::Ipc::PMP_PMAL_GENERATE_PIUID;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(resp->data, bufferSize - sizeof(resp->header));
      CError err = writer.write(piuid);
      if (!err.isNoError())
      {
         bufferSize = 0;
         return;
      }

      resp->header.size = writer.getUsedSize();
   }
   bufferSize = sizeof(resp->header) + resp->header.size;
}

void CIpcProtocol::processReleasePIUIDRequest(ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   bufferSize = 0;

   if (!mpPim)
   {
      return;
   }

   AXIS::Profile::IUid piuid;
   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
      CError err = reader.read(piuid);
      if (!err.isNoError())
         return;
   }

   mpPim->releasePIUID(piuid, dirId);
}

void CIpcProtocol::processCreateProfileRequest(ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   bufferSize = 0;

   if (!mpPim)
   {
      return;
   }

   AXIS::Profile::Uid uid;
   AXIS::Profile::IUid piuid;
   AXIS::Service::Uid sid;
   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
      CError err = reader.read(uid);
      if (!err.isNoError()) return;

      err = reader.read(piuid);
      if (!err.isNoError()) return;

      err = reader.read(sid);
      if (!err.isNoError()) return;
   }

   mpPim->createProfile(uid, piuid, sid, dirId);
}

void CIpcProtocol::processProfileDiedRequest(ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   bufferSize = 0;

   if (!mpPim)
   {
      return;
   }

   AXIS::Profile::IUid piuid;
   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
      CError err = reader.read(piuid);
      if (!err.isNoError()) return;
   }

   mpPim->profileDied(piuid, dirId);
}

void CIpcProtocol::processDisableByUidRequest(ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if (!mpPim)
   {
      bufferSize = 0;
      return;
   }

   AXIS::Profile::Uid uid;
   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
      CError err = reader.read(uid);
      if (!err.isNoError()) return;
   }
   bool result = mpCore->disableByClient(uid);

   resp->header.type = ProfileManager::Ipc::PMP_PMAL_DISABLE_BY_UID;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(resp->data, bufferSize - sizeof(resp->header));
      CError err = writer.write(result);
      if (!err.isNoError())
      {
         bufferSize = 0;
         return;
      }

      resp->header.size = writer.getUsedSize();
   }
   bufferSize = sizeof(resp->header) + resp->header.size;
}

void CIpcProtocol::processEnableByUidRequest(ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize,  AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if (!mpPim)
   {
      bufferSize = 0;
      return;
   }

   AXIS::Profile::Uid uid;
   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
      CError err = reader.read(uid);
      if (!err.isNoError()) return;
   }
   bool result = mpCore->enableByClient(uid);

   resp->header.type = ProfileManager::Ipc::PMP_PMAL_ENABLE_BY_UID;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(resp->data, bufferSize - sizeof(resp->header));
      CError err = writer.write(result);
      if (!err.isNoError())
      {
         bufferSize = 0;
         return;
      }

      resp->header.size = writer.getUsedSize();
   }
   bufferSize = sizeof(resp->header) + resp->header.size;
}

void CIpcProtocol::processEnableAllRequest(ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   bufferSize = 0;

   if (!mpPim)
   {
      return;
   }

   mpCore->enableByClientAll();
}

void CIpcProtocol::processGetManifestRequest(ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if (!mpPim)
   {
      bufferSize = 0;
      return;
   }

   AXIS::Profile::Uid uid;
   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
      CError err = reader.read(uid);
      if (!err.isNoError()) return;
   }
   std::string manifest;
   bool result = mpCore->getManifest(uid, manifest);

   resp->header.type = ProfileManager::Ipc::PMP_PMAL_GET_MANIFEST;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(resp->data, bufferSize - sizeof(resp->header));
      CError err = writer.write(result);
      if (!err.isNoError())
      {
         bufferSize = 0;
         return;
      }

      err = writer.write(manifest);
      if (!err.isNoError())
      {
         bufferSize = 0;
         return;
      }

      resp->header.size = writer.getUsedSize();
   }
   bufferSize = sizeof(resp->header) + resp->header.size;
}


void CIpcProtocol::processGetProfileLibPathRequest(ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if (!mpPim)
   {
      bufferSize = 0;
      return;
   }

   AXIS::Profile::Uid uid;
   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
      CError err = reader.read(uid);
      if (!err.isNoError()) return;
   }
   std::string path;
   bool result = mpCore->getProfileLibPath(uid, path);
   LOG4CPLUS_INFO(msLogger, "result = " + convertIntegerToString(static_cast<int>(result)));

   resp->header.type = ProfileManager::Ipc::PMP_PMAL_GET_PROFILE_LIB_PATH;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(resp->data, bufferSize - sizeof(resp->header));
      CError err = writer.write(result);
      if (!err.isNoError())
      {
         bufferSize = 0;
         return;
      }

      err = writer.write(path);
      if (!err.isNoError())
      {
         bufferSize = 0;
         return;
      }

      resp->header.size = writer.getUsedSize();
   }
   bufferSize = sizeof(resp->header) + resp->header.size;
}

void CIpcProtocol::processFindProfilesRequest(ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   int respBufSize = bufferSize;
   bufferSize = 0;

   if (!mpPim)
   {
      return;
   }

   AXIS::Profile::Uid uid;
   std::map<std::string, std::string> profileParameters;
   bool enabledProfile;
   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
      CError err = reader.read(uid);
      if (!err.isNoError()) return;
      err = reader.readMap(std::inserter(profileParameters, profileParameters.begin()));
      if (!err.isNoError()) return;
      err = reader.read(enabledProfile);
     if (!err.isNoError()) return;

   }

   std::list<Profile::Uid> profiles;
   mpCore->findProfiles(uid, profileParameters, profiles, enabledProfile);

   resp->header.type = ProfileManager::Ipc::PMP_PMAL_FIND_PROFILES;
   {
      ProfileManager::Ipc::Helpers::CBufferWriter writer(resp->data, respBufSize - sizeof(resp->header));
      CError err = writer.write(profiles.begin(), profiles.end());
      if (!err.isNoError())
      {
         return;
      }
      resp->header.size = writer.getUsedSize();
   }
   bufferSize = sizeof(resp->header) + resp->header.size;
}

void CIpcProtocol::processReadyToServeRequest(ProfileManager::Ipc::Message const* const req, ProfileManager::Ipc::Message* const resp, UInt32& bufferSize, AXIS::Ipc::DirectionID const& dirId)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   bufferSize = 0;

   if (!mpPim)
   {
      return;
   }

   AXIS::Service::SessionUid sesUid;
   {
      ProfileManager::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
      CError err = reader.read(sesUid);
      if (!err.isNoError()) return;
   }

   mpPim->readyToServe(sesUid, dirId);
}


}  // namespace Ipc
}  // namespace PMP
}  // namespace AXIS

