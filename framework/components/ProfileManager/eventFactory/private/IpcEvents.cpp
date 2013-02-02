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


#include "IpcEvents.hpp"
#include "PmpIpcProtocol.hpp"
#include "PmpCore.hpp"
#include "PmpPim.hpp"

namespace iviLink
{
namespace PMP
{

CoreIpcDisableByClient::CoreIpcDisableByClient(PmpCore * core, const iviLink::BaseUid & id,
        const iviLink::Ipc::DirectionID & dirId)
    : mCore(core)
    , mId(id)
    , mDirId(dirId)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreIpcDisableByClient::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->disableByClient(mId);
}

CoreIpcEnableByClient::CoreIpcEnableByClient(PmpCore * core, const iviLink::BaseUid & id,
        const iviLink::Ipc::DirectionID & dirId)
    : mCore(core)
    , mId(id)
    , mDirId(dirId)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreIpcEnableByClient::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->enableByClient(mId);
}

CoreIpcEnableByClientAll::CoreIpcEnableByClientAll(PmpCore * core, const iviLink::Ipc::DirectionID & dirId)
    : mCore(core)
    , mDirId(dirId)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreIpcEnableByClientAll::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mCore->enableByClientAll();
}

CoreIpcGetManifest::CoreIpcGetManifest(PmpCore * core, const iviLink::BaseUid & uid,
        const iviLink::Ipc::DirectionID & dirId, PMP::Ipc::PmpIpcProtocol * ipcProtocol)
    : mCore(core)
    , mId(uid)
    , mDirId(dirId)
    , mIpcProtocol(ipcProtocol)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreIpcGetManifest::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    std::string manifest;
    mCore->getManifest(mId,manifest);
    mIpcProtocol->getManifestResponse(manifest, mDirId);
}

CoreIpcGetProfileLibPath::CoreIpcGetProfileLibPath(PmpCore * core, const iviLink::Profile::Uid & uid,
        const iviLink::Ipc::DirectionID & dirId, PMP::Ipc::PmpIpcProtocol * ipcProtocol)
    : mCore(core)
    , mId(uid)
    , mDirId(dirId)
    , mIpcProtocol(ipcProtocol)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreIpcGetProfileLibPath::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    std::string path;
    mCore->getProfileLibPath(mId, path);
    mIpcProtocol->getProfileLibPathResponse(path, mDirId);
}

CoreIpcFindProfiles::CoreIpcFindProfiles(PmpCore * core, const iviLink::BaseUid & id,
        const std::map<std::string, std::string> & profileParameters,
        bool enabledProfiles, const iviLink::Ipc::DirectionID & dirId,
        PMP::Ipc::PmpIpcProtocol * ipcProtocol)
    : mCore(core)
    , mId(id)
    , mProfileParameters(profileParameters)
    , mEnabledProfiles(enabledProfiles)
    , mDirId(dirId)
    , mIpcProtocol(ipcProtocol)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void CoreIpcFindProfiles::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    std::list<Profile::Uid> profiles;
    mCore->findProfiles(mId, mProfileParameters, profiles, mEnabledProfiles);
    mIpcProtocol->findProfilesResponse(profiles, mDirId);
}

PimIpcGeneratePIUID::PimIpcGeneratePIUID(PmpPim * pim, iviLink::Ipc::DirectionID const& dirId,
        PMP::Ipc::PmpIpcProtocol * ipcProtocol)
    : mPim(pim)
    , mDirId(dirId)
    , mIpcProtocol(ipcProtocol)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void PimIpcGeneratePIUID::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    Profile::IUid piuid;
    mPim->generatePIUID(piuid, mDirId);
    mIpcProtocol->generatePIUidResponse(piuid, mDirId);
}

PimIpcReleasePIUID::PimIpcReleasePIUID(PmpPim * pim, iviLink::Profile::IUid const& piuid,
        iviLink::Ipc::DirectionID const& dirId)
    : mPim(pim)
    , mDirId(dirId)
    , mProfileIUid(piuid)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void PimIpcReleasePIUID::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mPim->releasePIUID(mProfileIUid, mDirId);
}

PimIpcCreateProfile::PimIpcCreateProfile(PmpPim * pim, iviLink::BaseUid const& profileUid,
        iviLink::Profile::IUid const& piuid,
        iviLink::Service::Uid const& sid,
        iviLink::Ipc::DirectionID const& dirId)
    : mPim(pim)
    , mDirId(dirId)
    , mProfileUid(profileUid)
    , mProfileIUid(piuid)
    , mServiceUid(sid)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void PimIpcCreateProfile::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mPim->createProfile(mProfileUid, mProfileIUid, mServiceUid, mDirId);
}

PimIpcProfileDied::PimIpcProfileDied(PmpPim * pim, iviLink::Profile::IUid const& piuid,
        iviLink::Ipc::DirectionID const& dirId)
    : mPim(pim)
    , mDirId(dirId)
    , mProfileIUid(piuid)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void PimIpcProfileDied::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mPim->profileDied(mProfileIUid, mDirId);
}

PimIpcReadyToServe::PimIpcReadyToServe(PmpPim * pim, iviLink::Service::SessionUid const& sesUid,
        iviLink::Ipc::DirectionID const& dirId)
    : mPim(pim)
    , mDirId(dirId)
    , mSessionUid(sesUid)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void PimIpcReadyToServe::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mPim->readyToServe(mSessionUid, mDirId);
}

PimIpcOnConnection::PimIpcOnConnection(PmpPim * pim, iviLink::Ipc::DirectionID const& dirId)
    : mPim(pim)
    , mDirId(dirId)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void PimIpcOnConnection::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mPim->onConnection(mDirId);
}

PimIpcOnConnectionLost::PimIpcOnConnectionLost(PmpPim * pim, iviLink::Ipc::DirectionID const& dirId)
    : mPim(pim)
    , mDirId(dirId)
    , mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.Events")))
{
}

void PimIpcOnConnectionLost::handleEvent()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    mPim->onConnectionLost(mDirId);
}

} /* namespace PMP */
} /* namespace iviLink */
