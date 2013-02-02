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


#ifndef IPCEVENTS_HPP_
#define IPCEVENTS_HPP_

#include "EventHandler.hpp"
#include "IAmpHandler.hpp"
#include "IPmpCoreForIpc.hpp"
#include "IPmpPimForIpc.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace PMP
{

namespace Ipc
{
class PmpIpcProtocol;
}

class PmpCore;
class PmpPim;

class CoreIpcDisableByClient : public Utils::Event
{
public:
    CoreIpcDisableByClient(PmpCore * core, const iviLink::BaseUid & id,
            const iviLink::Ipc::DirectionID & dirId);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    BaseUid mId;
    iviLink::Ipc::DirectionID mDirId;
    Logger mLogger;
};

class CoreIpcEnableByClient : public Utils::Event
{
public:
    CoreIpcEnableByClient(PmpCore * core, const iviLink::BaseUid & id,
            const iviLink::Ipc::DirectionID & dirId);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    BaseUid mId;
    iviLink::Ipc::DirectionID mDirId;
    Logger mLogger;
};

class CoreIpcEnableByClientAll : public Utils::Event
{
public:
    CoreIpcEnableByClientAll(PmpCore * core, const iviLink::Ipc::DirectionID & dirId);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    iviLink::Ipc::DirectionID mDirId;
    Logger mLogger;
};

class CoreIpcGetManifest : public Utils::Event
{
public:
    CoreIpcGetManifest(PmpCore * core, const iviLink::BaseUid & uid,
            const iviLink::Ipc::DirectionID & dirId, PMP::Ipc::PmpIpcProtocol * ipcProtocol);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    BaseUid mId;
    iviLink::Ipc::DirectionID mDirId;
    PMP::Ipc::PmpIpcProtocol * mIpcProtocol;
    Logger mLogger;
};

class CoreIpcGetProfileLibPath : public Utils::Event
{
public:
    CoreIpcGetProfileLibPath(PmpCore * core, const iviLink::Profile::Uid & uid,
            const iviLink::Ipc::DirectionID & dirId, PMP::Ipc::PmpIpcProtocol * ipcProtocol);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    Profile::Uid mId;
    iviLink::Ipc::DirectionID mDirId;
    PMP::Ipc::PmpIpcProtocol * mIpcProtocol;
    Logger mLogger;
};

class CoreIpcFindProfiles : public Utils::Event
{
public:
    CoreIpcFindProfiles(PmpCore * core, const iviLink::BaseUid & id,
            const std::map<std::string, std::string> & profileParameters,
            bool enabledProfiles, const iviLink::Ipc::DirectionID & dirId,
            PMP::Ipc::PmpIpcProtocol * ipcProtocol);

private:
    virtual void handleEvent();

    PmpCore * mCore;
    BaseUid mId;
    std::map<std::string, std::string> mProfileParameters;
    bool mEnabledProfiles;
    iviLink::Ipc::DirectionID mDirId;
    PMP::Ipc::PmpIpcProtocol * mIpcProtocol;
    Logger mLogger;
};

/////PIM IPC
class PimIpcGeneratePIUID : public Utils::Event
{
public:
    PimIpcGeneratePIUID(PmpPim * pim, iviLink::Ipc::DirectionID const& dirId,
            PMP::Ipc::PmpIpcProtocol * ipcProtocol);

private:
    virtual void handleEvent();

    PmpPim * mPim;
    iviLink::Ipc::DirectionID mDirId;
    PMP::Ipc::PmpIpcProtocol * mIpcProtocol;
    Logger mLogger;
};

class PimIpcReleasePIUID : public Utils::Event
{
public:
    PimIpcReleasePIUID(PmpPim * pim, iviLink::Profile::IUid const& piuid,
            iviLink::Ipc::DirectionID const& dirId);

private:
    virtual void handleEvent();

    PmpPim * mPim;
    iviLink::Ipc::DirectionID mDirId;
    Profile::IUid mProfileIUid;
    Logger mLogger;
};

class PimIpcCreateProfile : public Utils::Event
{
public:
    PimIpcCreateProfile(PmpPim * pim, iviLink::BaseUid const& profileUid,
            iviLink::Profile::IUid const& piuid,
            iviLink::Service::Uid const& sid,
            iviLink::Ipc::DirectionID const& dirId);

private:
    virtual void handleEvent();

    PmpPim * mPim;
    iviLink::Ipc::DirectionID mDirId;
    BaseUid mProfileUid;
    Profile::IUid mProfileIUid;
    Service::Uid mServiceUid;
    Logger mLogger;
};

class PimIpcProfileDied : public Utils::Event
{
public:
    PimIpcProfileDied(PmpPim * pim, iviLink::Profile::IUid const& piuid,
            iviLink::Ipc::DirectionID const& dirId);

private:
    virtual void handleEvent();

    PmpPim * mPim;
    iviLink::Ipc::DirectionID mDirId;
    Profile::IUid mProfileIUid;
    Logger mLogger;
};

class PimIpcReadyToServe : public Utils::Event
{
public:
    PimIpcReadyToServe(PmpPim * pim, iviLink::Service::SessionUid const& sesUid,
            iviLink::Ipc::DirectionID const& dirId);

private:
    virtual void handleEvent();

    PmpPim * mPim;
    iviLink::Ipc::DirectionID mDirId;
    Service::SessionUid mSessionUid;
    Logger mLogger;
};

class PimIpcOnConnection : public Utils::Event
{
public:
    PimIpcOnConnection(PmpPim * pim, iviLink::Ipc::DirectionID const& dirId);

private:
    virtual void handleEvent();

    PmpPim * mPim;
    iviLink::Ipc::DirectionID mDirId;
    Logger mLogger;
};

class PimIpcOnConnectionLost : public Utils::Event
{
public:
    PimIpcOnConnectionLost(PmpPim * pim, iviLink::Ipc::DirectionID const& dirId);

private:
    virtual void handleEvent();

    PmpPim * mPim;
    iviLink::Ipc::DirectionID mDirId;
    Logger mLogger;
};

} /* namespace PMP */
} /* namespace iviLink */
#endif /* IPCEVENTS_HPP_ */
