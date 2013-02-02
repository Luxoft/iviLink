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


#include <cassert>


#include "CPMALError.hpp"
#include "CComponentMgr.hpp"
#include "IPMALPIMToIpc.hpp"

#include "ProfileManagerIpcCommon.hpp"
#include "PmMessage.hpp"
#include "buffer_helpers.hpp"
#include "CSignalSemaphore.hpp"
#include "CMutex.hpp"

#include "CIpc.hpp"

#include "CPMALIpcProtocol.hpp"

static const char gModuleName[] = "CIpcProtocol";

using iviLink::Ipc::CIpc;
using iviLink::Ipc::Helpers::CBufferManager;
using iviLink::Ipc::Helpers::CBuffer;

namespace iviLink {
namespace PMAL {
namespace Ipc {

   Logger CIpcProtocol::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMAL.CIpcProtocol"));

   const int syncTimeout = 10000;

   /// It's a helper entity. Used for simulate blocking IPC requests over non-blocking.
   /// It'l be changed after refactoring of AppLib entities
   struct SyncRequestsData
   {
       SyncRequestsData()
           : generatePIUidResponseResult(false)
           , getManifestResponseResult(false)
           , getProfileLibPathResponseResult(false)
           , findProfilesResponseResult(false)
           , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMAL.CIpcProtocol")))
       {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
       }

       void generatePIUidResponseReset()
       {
           generatePIUidResponseResult = false;
       }

       bool generatePIUidResponseWait(iviLink::Profile::IUid & piuid)
       {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

           generatePIUidResponseSem.waitTimeout(syncTimeout);
           piuid = generatePIUidResponsePIUid;
           return generatePIUidResponseResult;
       }

       void generatePIUidResponseSignal(const iviLink::Profile::IUid & piuid)
       {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

           generatePIUidResponseResult = true;
           generatePIUidResponsePIUid = piuid;
           generatePIUidResponseSem.signal();
       }

       void getManifestResponseReset()
       {
           getManifestResponseResult = false;
       }

       bool getManifestResponseWait(std::string & manifest)
       {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

           getManifestResponseSem.waitTimeout(syncTimeout);
           manifest = getManifestResponseManifest;
           return getManifestResponseResult;
       }

       void getManifestResponseSignal(const std::string & manifest)
       {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

           getManifestResponseResult = true;
           getManifestResponseManifest = manifest;
           getManifestResponseSem.signal();
       }

       void getProfileLibPathResponseReset()
       {
           getProfileLibPathResponseResult = false;
       }

       bool getProfileLibPathResponseWait(std::string & path)
       {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

           getProfileLibPathResponseSem.waitTimeout(syncTimeout);
           path = getProfileLibPathResponsePath;
           return getProfileLibPathResponseResult;

       }

       void getProfileLibPathResponseSignal(const std::string & path)
       {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

           getProfileLibPathResponseResult = true;
           getProfileLibPathResponsePath = path;
           getProfileLibPathResponseSem.signal();
       }

       void findProfilesResponseReset()
       {
           findProfilesResponseResult = false;
       }

       bool findProfilesResponseWait(std::list<iviLink::Profile::Uid> & profiles)
       {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

           findProfilesResponseSem.waitTimeout(syncTimeout);
           profiles = findProfilesResponseProfiles;
           return findProfilesResponseResult;
       }

       void findProfilesResponseSignal(const std::list<iviLink::Profile::Uid> & profiles)
       {
           LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );

           findProfilesResponseResult = true;
           findProfilesResponseProfiles = profiles;
           findProfilesResponseSem.signal();
       }

   private:
       /// Generate Profile Instance UID
       CSignalSemaphore generatePIUidResponseSem;
       mutable iviLink::Profile::IUid generatePIUidResponsePIUid;
       mutable bool generatePIUidResponseResult;

       /// Get Manifest
       CSignalSemaphore getManifestResponseSem;
       mutable std::string getManifestResponseManifest;
       mutable bool getManifestResponseResult;

       /// Get Profile Lib Path
       CSignalSemaphore getProfileLibPathResponseSem;
       mutable std::string getProfileLibPathResponsePath;
       mutable bool getProfileLibPathResponseResult;

       /// Find Profiles
       CSignalSemaphore findProfilesResponseSem;
       mutable std::list<iviLink::Profile::Uid> findProfilesResponseProfiles;
       mutable bool findProfilesResponseResult;

       Logger mLogger;
   };

CIpcProtocol::CMsgIdGen::CMsgIdGen() :
      mId(-1)
{
}

CIpcProtocol::CMsgIdGen::~CMsgIdGen()
{
}

iviLink::Ipc::MsgID CIpcProtocol::CMsgIdGen::getNext()
{
   mId += 2;
   return mId;
}

bool CIpcProtocol::generatePIUID(iviLink::Profile::IUid& piuid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpIpc)
    {
        LOG4CPLUS_ERROR(msLogger, "no ipc");
        return false;
    }

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_GENERATE_PIUID;
   req->header.size = 0;

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   mSyncData.generatePIUidResponseReset();
   BaseError err = mpIpc->asyncRequest(id, writeBuf.get(), reqSize);

   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return false;
   }

   return mSyncData.generatePIUidResponseWait(piuid);
}

bool CIpcProtocol::releasePIUID(iviLink::Profile::IUid const& piuid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpIpc)
    {
        LOG4CPLUS_ERROR(msLogger, "no ipc");
        return false;
    }

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_RELEASE_PIUID;

   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));
      BaseError err = writer.write(piuid);

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   BaseError err = mpIpc->asyncRequest(id, writeBuf.get(), reqSize);

   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return false;
   }

   return true;
}

bool CIpcProtocol::createProfile(iviLink::BaseUid const& profileUid,
      iviLink::Profile::IUid const& piuid,
      iviLink::Service::Uid const& sid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpIpc)
    {
        LOG4CPLUS_ERROR(msLogger, "no ipc");
        return false;
    }

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_CREATE_PROFILE;

   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      BaseError err = writer.write(profileUid);

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }


      err = writer.write(piuid);

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }


      err = writer.write(sid);

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }


      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   BaseError err = mpIpc->asyncRequest(id, writeBuf.get(), reqSize);

   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return false;
   }

   return true;
}

bool CIpcProtocol::profileDied(iviLink::Profile::IUid const& piuid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpIpc)
    {
        LOG4CPLUS_ERROR(msLogger, "no ipc");
        return false;
    }

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_PROFILE_DIED;

   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      BaseError err = writer.write(piuid);

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }


      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   BaseError err = mpIpc->asyncRequest(id, writeBuf.get(), reqSize);


   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return false;
   }

   return true;
}

bool CIpcProtocol::readyToServe(iviLink::Service::SessionUid const& sesId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpIpc)
    {
        LOG4CPLUS_ERROR(msLogger, "no ipc");
        return false;
    }

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_READY_TO_SERVE;

   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      BaseError err = writer.write(sesId);

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   BaseError err = mpIpc->asyncRequest(id, writeBuf.get(), reqSize);

   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return false;
   }

   return true;
}

bool CIpcProtocol::disableByUid(iviLink::Profile::Uid const& uid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpIpc)
    {
        LOG4CPLUS_ERROR(msLogger, "no ipc");
        return false;
    }

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_DISABLE_BY_UID;
   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      BaseError err = writer.write(uid);

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   BaseError err = mpIpc->asyncRequest(id, writeBuf.get(), reqSize);

   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return false;
   }

   return true;
}

bool CIpcProtocol::enableByUid(iviLink::Profile::Uid const& uid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpIpc)
    {
        LOG4CPLUS_ERROR(msLogger, "no ipc");
        return false;
    }

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_ENABLE_BY_UID;
   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      BaseError err = writer.write(uid);

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   BaseError err = mpIpc->asyncRequest(id, writeBuf.get(), reqSize);

   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return false;
   }

   return true;
}

bool CIpcProtocol::enableAll()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpIpc)
    {
        LOG4CPLUS_ERROR(msLogger, "no ipc");
        return false;
    }

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_ENABLE_ALL;
   req->header.size = 0;

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   BaseError err = mpIpc->asyncRequest(id, writeBuf.get(), reqSize);

   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return false;
   }

   return true;
}

bool CIpcProtocol::getManifest(iviLink::BaseUid const& uid, std::string& manifest)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (!mpIpc)
    {
        LOG4CPLUS_ERROR(msLogger, "no ipc");
        return false;
    }

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_GET_MANIFEST;
   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      BaseError err = writer.write(uid);

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   mSyncData.getManifestResponseReset();
   BaseError err = mpIpc->asyncRequest(id, writeBuf.get(), reqSize);

   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return false;
   }

   return mSyncData.getManifestResponseWait(manifest);
}

bool CIpcProtocol::getProfileLibPath(iviLink::Profile::Uid const& uid, std::string& path)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

   if (!mpIpc)
   {
       LOG4CPLUS_ERROR(msLogger, "no ipc");
       return false;
   }

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_GET_PROFILE_LIB_PATH;
   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      BaseError err = writer.write(uid);

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   mSyncData.getProfileLibPathResponseReset();
   BaseError err = mpIpc->asyncRequest(id, writeBuf.get(), reqSize);

   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return false;
   }

   return mSyncData.getProfileLibPathResponseWait(path);
}

bool CIpcProtocol::findProfiles(iviLink::BaseUid const& profileApiUid,
               std::map<std::string, std::string> const& profileParameters,
               std::list<iviLink::Profile::Uid> & profiles,
               bool enabledProfiles)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    if (!mpIpc)
    {
        LOG4CPLUS_ERROR(msLogger, "no ipc");
        return false;
    }

   CBuffer writeBuf = mWriteBufMgr.getBuffer();
   CBuffer readBuf = mReadBufMgr.getBuffer();
   ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
   req->header.type = ProfileManager::Ipc::PMAL_PMP_FIND_PROFILES;
   {
      iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

      BaseError err = writer.write(profileApiUid);

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }

      err = writer.write(profileParameters.begin(), profileParameters.end());

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }

      err = writer.write(enabledProfiles);

      if (!err.isNoError())
      {
         LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
         return false;
      }

      req->header.size = writer.getUsedSize();
   }

   iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

   UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
   mSyncData.findProfilesResponseReset();
   BaseError err = mpIpc->asyncRequest(id, writeBuf.get(), reqSize);

   if (!err.isNoError())
   {
      LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
      return false;
   }

   return mSyncData.findProfilesResponseWait(profiles);
}

void CIpcProtocol::OnConnection(iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CIpcProtocol::OnConnectionLost(iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CIpcProtocol::OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void CIpcProtocol::OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, iviLink::Ipc::DirectionID)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    ProfileManager::Ipc::PmMessage const* req = reinterpret_cast<ProfileManager::Ipc::PmMessage const*>(pPayload);

    assert(req->header.size + sizeof(ProfileManager::Ipc::PmMessage) == payloadSize);

    switch (req->header.type)
    {
    case ProfileManager::Ipc::PMP_PMAL_PROFILE_DIED:
       processProfileDiedRequest(req);
       break;
    case ProfileManager::Ipc::PMP_PMAL_INCOMMING_PROFILE:
       processIncomingProfileRequest(req);
       break;
    case ProfileManager::Ipc::PMP_PMAL_GENERATE_PIUID:
       processGeneratePIUidResponse(req);
       break;
    case ProfileManager::Ipc::PMP_PMAL_GET_MANIFEST:
       processGetManifestResponse(req);
       break;
    case ProfileManager::Ipc::PMP_PMAL_GET_PROFILE_LIB_PATH:
       processGetProfileLibPathResponse(req);
       break;
    case ProfileManager::Ipc::PMP_PMAL_FIND_PROFILES:
       processFindProfilesResponse(req);
       break;
    default:
       CPMALError err(CPMALError::ERROR_OTHER, gModuleName, "Unknown request");
       LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
       break;
    }
}

CIpcProtocol::CIpcProtocol(char const* pPmpAddress/* = NULL*/)
    : mpIpc(NULL)
    , mSyncData(*(new SyncRequestsData()))
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if (!pPmpAddress)
      pPmpAddress = PMP_ADDRESS;
   mpIpc = new CIpc(pPmpAddress, *this);
}

CIpcProtocol::~CIpcProtocol()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   delete mpIpc;
   SyncRequestsData * pdata = &mSyncData;
   delete pdata;
}

BaseError CIpcProtocol::connect()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   return mpIpc->connect();
}

bool CIpcProtocol::isConnected() const
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   return mpIpc->isConnected();
}

void CIpcProtocol::processProfileDiedRequest(ProfileManager::Ipc::PmMessage const* const req)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   iviLink::Profile::IUid piuid;

    iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
    BaseError err = reader.read(piuid);
    if (!err.isNoError()) return;

   CPMALComponentMgr* mgr = CPMALComponentMgr::getInstance();
   if (!mgr)
      return;
   IPMALPIMToIpc* pim = mgr->getPIMToIpc();
   if (!pim)
      return;
   pim->profileDied(piuid);
}

void CIpcProtocol::processIncomingProfileRequest(ProfileManager::Ipc::PmMessage const* const req)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   iviLink::Profile::Uid uid;
   iviLink::Profile::ApiUid papiUid;
   iviLink::Profile::IUid piuid;
   iviLink::Service::Uid sid;


   {
      iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
      BaseError err = reader.read(uid);
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

void CIpcProtocol::processGeneratePIUidResponse(ProfileManager::Ipc::PmMessage const* const req)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
   if (req->header.type != ProfileManager::Ipc::PMP_PMAL_GENERATE_PIUID)
   {
       LOG4CPLUS_ERROR(msLogger, "wrong response");
   }
   else if (0 == req->header.size)
   {
       LOG4CPLUS_ERROR(msLogger, "empty message");
   }
   else
   {
        iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
        Profile::IUid piuid;
        BaseError err = reader.read(piuid);

        if (!err.isNoError())
        {
            LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        }
        else
        {
            mSyncData.generatePIUidResponseSignal(piuid);
        }
   }
}

void CIpcProtocol::processGetManifestResponse(ProfileManager::Ipc::PmMessage const* const req)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (req->header.type != ProfileManager::Ipc::PMP_PMAL_GET_MANIFEST)
    {
        LOG4CPLUS_ERROR(msLogger, "wrong response");
    }
    else if (0 == req->header.size)
    {
        LOG4CPLUS_ERROR(msLogger, "empty message");
    }
    else
    {
        iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);

        std::string manifest;
        BaseError err = err = reader.read(manifest);
        if (!err.isNoError())
        {
            LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        }
        else
        {
            mSyncData.getManifestResponseSignal(manifest);
        }
    }
}

void CIpcProtocol::processGetProfileLibPathResponse(ProfileManager::Ipc::PmMessage const* const req)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (req->header.type != ProfileManager::Ipc::PMP_PMAL_GET_PROFILE_LIB_PATH)
    {
        LOG4CPLUS_ERROR(msLogger, "wrong response");
    }
    else if (0 == req->header.size)
    {
        LOG4CPLUS_ERROR(msLogger, "empty message");
    }
    else
    {
        LOG4CPLUS_INFO(msLogger, "else");

        iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);

        LOG4CPLUS_INFO(msLogger, "CBufferReader");

        std::string path;
        BaseError err = reader.read(path);

        LOG4CPLUS_INFO(msLogger, "read");
        if (!err.isNoError())
        {
            LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        }
        else
        {
            LOG4CPLUS_INFO(msLogger, "else2");
            mSyncData.getProfileLibPathResponseSignal(path);
        }
    }
}

void CIpcProtocol::processFindProfilesResponse(ProfileManager::Ipc::PmMessage const* const req)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (req->header.type != ProfileManager::Ipc::PMP_PMAL_FIND_PROFILES)
    {
        LOG4CPLUS_ERROR(msLogger, "wrong response");
    }
    else if (0 == req->header.size)
    {
        LOG4CPLUS_ERROR(msLogger, "empty message");
    }
    else
    {
        iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);

        std::list<Profile::Uid> profiles;
        BaseError err = reader.readContainer(std::back_inserter(profiles));
        if (!err.isNoError())
        {
            LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        }
        else
        {
            LOG4CPLUS_INFO(msLogger, "Size: " + convertIntegerToString(profiles.size()));
            for (std::list<Profile::Uid>::const_iterator it = profiles.begin();
                    profiles.end() != it; ++it)
            {
                LOG4CPLUS_INFO(msLogger, (*it).value());
            }
            mSyncData.findProfilesResponseSignal(profiles);
        }
    }
}


}  // namespace PMAL_Ipc
}  // namespace ProfileManager
}  // namespace AXIS
