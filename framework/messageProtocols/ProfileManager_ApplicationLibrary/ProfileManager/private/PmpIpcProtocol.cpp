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
#include <set>

#include "PmpIpcProtocol.hpp"
#include "IPmpPimForIpc.hpp"
#include "IPmpCoreForIpc.hpp"
#include "ProfileManagerIpcCommon.hpp"
#include "PmMessage.hpp"
#include "buffer_helpers.hpp"

#include "CIpc.hpp"


static const char gModuleName[] = "PmpIpcProtocol";

using iviLink::Ipc::CIpc;
using iviLink::Ipc::Helpers::CBufferManager;
using iviLink::Ipc::Helpers::CBuffer;

namespace iviLink
{
namespace PMP
{
namespace Ipc
{

Logger PmpIpcProtocol::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.IPC.PmpIpcProtocol"));


PmpIpcProtocol::MsgIdGen::MsgIdGen()
    : mId(0)
{
}

PmpIpcProtocol::MsgIdGen::~MsgIdGen()
{
}

iviLink::Ipc::MsgID PmpIpcProtocol::MsgIdGen::getNext()
{
    mId += 2;
    return mId;
}

bool PmpIpcProtocol::incomingProfile(iviLink::BaseUid const& profileUid,
         iviLink::Profile::ApiUid const& papiUid,
         iviLink::Profile::IUid const& piuid,
         iviLink::Service::Uid const& serviceUID,
         iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    CBuffer writeBuf = mWriteBufMgr.getBuffer();
    ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
    req->header.type = ProfileManager::Ipc::PMP_PMAL_INCOMMING_PROFILE;

    iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

    BaseError err = writer.write(profileUid);
    if (!err.isNoError())
    {
    LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
    return false;
    }

    err = writer.write(papiUid);
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

    err = writer.write(serviceUID);
    if (!err.isNoError())
    {
    LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
    return false;
    }

    req->header.size = writer.getUsedSize();

    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

    UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;
    err = mIpc->asyncRequest(id, writeBuf.get(), reqSize, &dirId);
    return err.isNoError();
}

bool PmpIpcProtocol::profileDied(iviLink::Profile::IUid const& piuid,
        iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    CBuffer writeBuf = mWriteBufMgr.getBuffer();
    ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
    req->header.type = ProfileManager::Ipc::PMP_PMAL_PROFILE_DIED;
    iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

    BaseError err = writer.write(piuid);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return false;
    }

    req->header.size = writer.getUsedSize();
    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();
    UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;

    err = mIpc->asyncRequest(id, writeBuf.get(), reqSize, &dirId);

    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return false;
    }

    return true;
}

bool PmpIpcProtocol::generatePIUidResponse(const iviLink::Profile::IUid& piuid,
        iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    CBuffer writeBuf = mWriteBufMgr.getBuffer();
    ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
    req->header.type = ProfileManager::Ipc::PMP_PMAL_GENERATE_PIUID;
    iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

    BaseError err = writer.write(piuid);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return false;
    }

    req->header.size = writer.getUsedSize();
    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

    UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;

    err = mIpc->asyncRequest(id, writeBuf.get(), reqSize, &dirId);

    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return false;
    }

    return true;
}

bool PmpIpcProtocol::getManifestResponse(const std::string & manifest,
        iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    CBuffer writeBuf = mWriteBufMgr.getBuffer();
    ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
    req->header.type = ProfileManager::Ipc::PMP_PMAL_GET_MANIFEST;
    iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));


    BaseError err = writer.write(manifest);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return false;
    }

    req->header.size = writer.getUsedSize();
    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

    UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;

    err = mIpc->asyncRequest(id, writeBuf.get(), reqSize, &dirId);

    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return false;
    }

    return true;
}

bool PmpIpcProtocol::getProfileLibPathResponse(const std::string & path,
        iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    CBuffer writeBuf = mWriteBufMgr.getBuffer();
    ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
    req->header.type = ProfileManager::Ipc::PMP_PMAL_GET_PROFILE_LIB_PATH;
    iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

    BaseError err = writer.write(path);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return false;
    }

    req->header.size = writer.getUsedSize();
    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

    UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;

    err = mIpc->asyncRequest(id, writeBuf.get(), reqSize, &dirId);

    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return false;
    }

    return true;
}

bool PmpIpcProtocol::findProfilesResponse(const std::list<iviLink::Profile::Uid> & profiles,
        iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    CBuffer writeBuf = mWriteBufMgr.getBuffer();
    ProfileManager::Ipc::PmMessage* req = reinterpret_cast<ProfileManager::Ipc::PmMessage*>(writeBuf.get());
    req->header.type = ProfileManager::Ipc::PMP_PMAL_FIND_PROFILES;
    iviLink::Ipc::Helpers::CBufferWriter writer(req->data, writeBuf.getSize() - sizeof(req->header));

    BaseError err = writer.write(profiles.begin(), profiles.end());
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return false;
    }

    req->header.size = writer.getUsedSize();
    iviLink::Ipc::MsgID id = mMsgIdGen.getNext();

    UInt32 const reqSize = sizeof(ProfileManager::Ipc::PmMessage) + req->header.size;

    err = mIpc->asyncRequest(id, writeBuf.get(), reqSize, &dirId);

    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return false;
    }

    return true;
}

void PmpIpcProtocol::OnConnection(iviLink::Ipc::DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mPim->onConnection(dirId);
}

void PmpIpcProtocol::OnConnectionLost(iviLink::Ipc::DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mPim->onConnectionLost(dirId);
}

void PmpIpcProtocol::OnRequest(iviLink::Ipc::MsgID id,
        UInt8 const* pPayload, UInt32 payloadSize,
        UInt8* const pResponseBuffer, UInt32& bufferSize,
        iviLink::Ipc::DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void PmpIpcProtocol::OnAsyncRequest(iviLink::Ipc::MsgID id,
        UInt8 const* pPayload, UInt32 payloadSize,
        iviLink::Ipc::DirectionID dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    ProfileManager::Ipc::PmMessage const* req = reinterpret_cast<ProfileManager::Ipc::PmMessage const*>(pPayload);

    assert(req->header.size + sizeof(ProfileManager::Ipc::PmMessage) == payloadSize);

    switch (static_cast<ProfileManager::Ipc::ePmalPmpMessages>(req->header.type))
    {
    case ProfileManager::Ipc::PMAL_PMP_GENERATE_PIUID:
        processGeneratePIUIDRequest(dirId);
        break;
    case ProfileManager::Ipc::PMAL_PMP_RELEASE_PIUID:
        processReleasePIUIDRequest(req, dirId);
        break;
    case ProfileManager::Ipc::PMAL_PMP_CREATE_PROFILE:
        processCreateProfileRequest(req, dirId);
        break;
    case ProfileManager::Ipc::PMAL_PMP_PROFILE_DIED:
        processProfileDiedRequest(req, dirId);
        break;
    case ProfileManager::Ipc::PMAL_PMP_DISABLE_BY_UID:
        processDisableByUidRequest(req, dirId);
        break;
    case ProfileManager::Ipc::PMAL_PMP_ENABLE_BY_UID:
        processEnableByUidRequest(req, dirId);
        break;
    case ProfileManager::Ipc::PMAL_PMP_ENABLE_ALL:
        processEnableAllRequest(req, dirId);
        break;
    case ProfileManager::Ipc::PMAL_PMP_GET_MANIFEST:
        processGetManifestRequest(req, dirId);
        break;
    case ProfileManager::Ipc::PMAL_PMP_GET_PROFILE_LIB_PATH:
        processGetProfileLibPathRequest(req, dirId);
        break;
    case ProfileManager::Ipc::PMAL_PMP_FIND_PROFILES:
        processFindProfilesRequest(req, dirId);
        break;
    case ProfileManager::Ipc::PMAL_PMP_READY_TO_SERVE:
        processReadyToServeRequest(req, dirId);
        break;
    }
}


PmpIpcProtocol::PmpIpcProtocol()
    : mIpc(0)
    , mPim(0)
    , mCore(0)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

PmpIpcProtocol::~PmpIpcProtocol()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    delete mIpc;
}

void PmpIpcProtocol::init(IPmpPimForIpc* pPim, IPmpCoreForIpc* pCore, char const* pPmpAddress)
{
    if (!pPmpAddress)
    {
        pPmpAddress = PMP_ADDRESS;
    }
    mIpc = new CIpc(pPmpAddress ? pPmpAddress : PMP_ADDRESS, *this);
    mPim = pPim;
    mCore = pCore;
}

bool PmpIpcProtocol::connect()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    return mIpc->beginWaitForConnection().isNoError();
}

bool PmpIpcProtocol::isConnected() const
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    return mIpc->isConnected();
}

void PmpIpcProtocol::processGeneratePIUIDRequest(iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    iviLink::Profile::IUid piuid;
    mPim->generatePIUID(dirId);
}

void PmpIpcProtocol::processReleasePIUIDRequest(ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    iviLink::Profile::IUid piuid;
    {
        iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
        BaseError err = reader.read(piuid);
        if (!err.isNoError())
        {
            return;
        }
    }

    mPim->releasePIUID(piuid, dirId);
}

void PmpIpcProtocol::processCreateProfileRequest(ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);

    iviLink::Profile::Uid uid;
    BaseError err = reader.read(uid);
    if (!err.isNoError()) return;

    iviLink::Profile::IUid piuid;
    err = reader.read(piuid);
    if (!err.isNoError()) return;

    iviLink::Service::Uid sid;
    err = reader.read(sid);
    if (!err.isNoError()) return;

    mPim->createProfile(uid, piuid, sid, dirId);
}

void PmpIpcProtocol::processProfileDiedRequest(ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    iviLink::Profile::IUid piuid;
    iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
    BaseError err = reader.read(piuid);

    if (!err.isNoError())
    {
        return;
    }

    mPim->profileDied(piuid, dirId);
}

void PmpIpcProtocol::processDisableByUidRequest(ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    iviLink::Profile::Uid uid;
    iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
    BaseError err = reader.read(uid);
    if (!err.isNoError())
    {
        return;
    }
    mCore->disableByClient(uid, dirId);
}

void PmpIpcProtocol::processEnableByUidRequest(ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);

    iviLink::Profile::Uid uid;
    BaseError err = reader.read(uid);
    if(!err.isNoError())
    {
        return;
    }

    mCore->enableByClient(uid, dirId);
}

void PmpIpcProtocol::processEnableAllRequest(ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mCore->enableByClientAll(dirId);
}

void PmpIpcProtocol::processGetManifestRequest(ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
    iviLink::Profile::Uid uid;
    BaseError err = reader.read(uid);

    if (!err.isNoError())
    {
        return;
    }
    mCore->getManifest(uid, dirId);
}


void PmpIpcProtocol::processGetProfileLibPathRequest(ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
    iviLink::Profile::Uid uid;
    BaseError err = reader.read(uid);

    if (!err.isNoError())
    {
        return;
    }
    mCore->getProfileLibPath(uid, dirId);
}

void PmpIpcProtocol::processFindProfilesRequest(ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    iviLink::Profile::Uid uid;
    iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
    BaseError err = reader.read(uid);

    if (!err.isNoError())
    {
        return;
    }

    std::map<std::string, std::string> profileParameters;
    err = reader.readMap(std::inserter(profileParameters, profileParameters.begin()));

    if (!err.isNoError())
    {
        return;
    }

    bool enabledProfile;
    err = reader.read(enabledProfile);

    if (!err.isNoError())
    {
        return;
    }

    mCore->findProfiles(uid, profileParameters, enabledProfile, dirId);
}

void PmpIpcProtocol::processReadyToServeRequest(ProfileManager::Ipc::PmMessage const* const req, iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    iviLink::Ipc::Helpers::CBufferReader reader(req->data, req->header.size);
    iviLink::Service::SessionUid sesUid;
    BaseError err = reader.read(sesUid);
    if (!err.isNoError())
    {
        return;
    }

    mPim->readyToServe(sesUid, dirId);
}


}  // namespace Ipc
}  // namespace PMP
}  // namespace AXIS

