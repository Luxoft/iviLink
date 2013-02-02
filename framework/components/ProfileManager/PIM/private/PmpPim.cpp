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
#include <unistd.h>

#include "PmpPim.hpp"
#include "ProfileData.hpp"
#include "ProfileInstanceData.hpp"
#include "helpers.hpp"
#include "ClientAppRegistry.hpp"
#include "PmpPimProtocol.hpp"
#include "PmpIpcProtocol.hpp"
#include "PmpComponentManager.hpp"
#include "IAmpRequest.hpp"
#include "EventFactory.hpp"

namespace iviLink
{
namespace PMP
{

Logger PmpPim::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.PIM.PmpPim"));

using PIM::ProfileData;
using PIM::ProfileInstanceData;
using PIM::ClientAppRegistry;

void PmpPim::generatePIUID(iviLink::Profile::IUid& piuid,
        iviLink::Ipc::DirectionID const& dirId)
{
    piuid = iviLink::generateUid();
}

void PmpPim::releasePIUID(iviLink::Profile::IUid const& ,
        iviLink::Ipc::DirectionID const& )
{
    /// @todo Currently, it is not used.
    /// I was planned, that this method will somehow return the uid to the pool of
    /// available id's, but this is not necessary now.
}

void PmpPim::createProfile(iviLink::BaseUid const& profileUid,
    iviLink::Profile::IUid const& piuid,
    iviLink::Service::Uid const& sid,
    iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_INFO(msLogger, "prfUid: " + profileUid.value() +
            ", piuid: " + piuid.value() + ", sid: " + sid.value() +
            + ", dirId: " + convertIntegerToString(dirId));

    BaseUid appId;
    BaseError err = mRegistry->findAppByDirId(dirId, appId);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return;
    }

    bool remoteCreation = (mIncomingProfiles.erase(piuid) != 0);

    ProfileData data = { piuid, sid, profileUid };
    err = mRegistry->registerProfile(appId, data);

    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return;
    }

    if (remoteCreation)
    {
        // Creation of profiles is started on other side. No need to message.
        return;
    }

    if (mCore)
    {
        Profile::Uid compUid;
        LOG4CPLUS_INFO(msLogger, "looking for complementary profile");
        bool res = mCore->getComplementary(profileUid, compUid);
        if (res)
        {
            LOG4CPLUS_INFO(msLogger, "found complementary " + compUid.value() +
                    ", sending to other side");
            mProtocol->createProfile(compUid, piuid, sid);
        }
        else
        {
            LOG4CPLUS_INFO(msLogger, "unable to find complementary for " +
            profileUid.value());
        }
    }
    else
    {
        LOG4CPLUS_INFO(msLogger, "mCore is NULL - unable to get complementary profile");
    }

}


void PmpPim::profileDied(iviLink::Profile::IUid const& piuid,
        iviLink::Ipc::DirectionID const&/* dirId*/)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    iviLink::Service::Uid sid;
    BaseError err = mRegistry->findSidByPIUID(piuid, sid);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return;
    }

    err = mRegistry->unregisterProfile(piuid);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return;
    }

    mProtocol->profileDied(piuid, sid);
}

void PmpPim::readyToServe(iviLink::Service::SessionUid const& sesUid,
        iviLink::Ipc::DirectionID const &dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    PendingService pendingServ;
    BaseUid appId;

    LOG4CPLUS_INFO(msLogger, "this = " + convertIntegerToString((intptr_t)this) +
            " mPendingServiceMap = " + convertIntegerToString((intptr_t)&mPendingServiceMap));
    LOG4CPLUS_INFO(msLogger, "mPendingServiceMap.size() " +
            convertIntegerToString(static_cast<int>(mPendingServiceMap.size())));
    for (PendingServiceMap::iterator it = mPendingServiceMap.begin(); it != mPendingServiceMap.end(); ++it)
    {
        LOG4CPLUS_INFO(msLogger, "service ses " + it->first.value());
    }

    PendingServiceMap::iterator it = mPendingServiceMap.find(sesUid);
    if (it == mPendingServiceMap.end())
    {
        LOG4CPLUS_WARN(msLogger, "unknown sesUid '" + sesUid.value() +
                "' from dirId " + convertIntegerToString(dirId));
    }
    else
    {
        pendingServ = it->second;
        iviLink::Service::Uid sid = std::tr1::get<2>(pendingServ);

        BaseError err = mRegistry->findAppByDirId(dirId, appId);
        if (!err.isNoError())
        {
            LOG4CPLUS_ERROR(msLogger, "unknown dirId " + convertIntegerToString(dirId));
        }
        else
        {
            mRegistry->addServiceToApp(appId, sid);
        }

        mPendingServiceMap.erase(it);
    }

    if (appId.empty())
    {
        LOG4CPLUS_ERROR(msLogger, "PmpPim::readyToServe appId not found");
        return;
    }

    LOG4CPLUS_INFO(msLogger, "PmpPim::readyToServe saveIncomingProfileForProcessing");
    saveIncomingProfileForProcessing(appId,
    std::tr1::get<0>(pendingServ),
    std::tr1::get<1>(pendingServ),
    std::tr1::get<2>(pendingServ));
}

void PmpPim::onConnection(iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    BaseUid appId = iviLink::generateUid();

    BaseError err = mRegistry->registerApp(appId, dirId);
    assert(err.isNoError());
}

void PmpPim::onConnectionLost(iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    BaseUid appId;
    BaseError err = mRegistry->findAppByDirId(dirId, appId);
    assert(err.isNoError());

    std::vector<iviLink::Profile::IUid> profiles;
    mRegistry->getProfilesOfApp(appId, profiles);
    for (std::vector<iviLink::Profile::IUid>::const_iterator it = profiles.begin(); it != profiles.end(); ++it)
    {
        profileDied((*it), -1);
    }

    err = mRegistry->unregisterApp(appId);
}

bool PmpPim::hasInstance(Profile::Uid const& profileId)
{
    return mRegistry->hasInstancesOf(profileId);
}

void PmpPim::unloadInstances(Profile::Uid const& profileId)
{
    std::vector<ProfileInstanceData> profiles;

    mRegistry->findInstancesOf(profileId, profiles);

    iviLink::Ipc::DirectionID dirId = -1;

    for (std::vector<ProfileInstanceData>::iterator it = profiles.begin(); it != profiles.end(); ++it)
    {
        ProfileInstanceData const& data = *it;

        BaseError err = mRegistry->findDirIdByApp(data.appId, dirId);
        if (!err.isNoError())
        {
            dirId = -1;

            LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        }

        bool res = mProtocol->profileDied(data.piuid, data.sid);
        if (!res)
        {
            LOG4CPLUS_ERROR(msLogger, "Request failed");
        }

        if (dirId != -1)
        {
            res = mIpc->profileDied(data.piuid, dirId);
            if (!res)
            {
                LOG4CPLUS_ERROR(msLogger, "IPC request failed");
            }
        }

        err = mRegistry->unregisterProfile(data.piuid);
        if (!err.isNoError())
        {
            /// @todo error check
            LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        }
    }
}

void PmpPim::appRequestError(iviLink::Service::SessionUid session)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void PmpPim::appManConnectionLost()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

bool PmpPim::onCreateProfile(iviLink::Profile::Uid const& profileUid,
        iviLink::Profile::IUid const& piuid,
        iviLink::Service::Uid const& sid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    if (!mPendingServiceMap.empty())
    {
        return false;
    }

    BaseUid appId;

    BaseError err = mRegistry->findAppBySid(sid, appId);
    if (err.isNoError())
    {
        // service was pending, but now it is know
        LOG4CPLUS_INFO(msLogger, "service known");
        saveIncomingProfileForProcessing(appId, profileUid, piuid, sid);
    }
    else
    {
        // service is not pending and unknown. need to create request
        LOG4CPLUS_INFO(msLogger, "service unknown");

        Service::SessionUid sesUid = generateUid();
        LOG4CPLUS_INFO(msLogger, "pending sesUid '" + sesUid.value() + "'");
        mPendingServiceMap.insert(std::make_pair(sesUid,
                PendingService(profileUid, piuid, sid)));

        LOG4CPLUS_INFO(msLogger, "this = " + convertIntegerToString((intptr_t)this) +
                " mPendingServiceMap = " + convertIntegerToString((intptr_t)&mPendingServiceMap));
        LOG4CPLUS_INFO(msLogger, "mPendingServiceMap.size() " +
                convertIntegerToString(static_cast<int>(mPendingServiceMap.size())));
        for (PendingServiceMap::iterator it = mPendingServiceMap.begin(); it != mPendingServiceMap.end(); ++it)
        {
            LOG4CPLUS_INFO(msLogger, "service ses '" + it->first.value() + "'");
        }

        if (!mAmpRequest->applicationRequest(sesUid, sid))
        {
            LOG4CPLUS_ERROR(msLogger, "applicationRequest failed");
        }
    } // if no err

    return true;
}

void PmpPim::onProfileDied(iviLink::Profile::IUid const& piuid,
        iviLink::Service::Uid const& sid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    BaseUid appId;
    BaseError err = mRegistry->findAppByPIUID(piuid, appId);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return;
    }

    err = mRegistry->unregisterProfile(piuid);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return;
    }

    iviLink::Ipc::DirectionID dirId = -1;
    err = mRegistry->findDirIdByApp(appId, dirId);
    if (!err.isNoError())
    {
        LOG4CPLUS_ERROR(msLogger, static_cast<std::string>(err));
        return;
    }

    bool res = mIpc->profileDied(piuid, dirId);
    if (!res)
    {
        LOG4CPLUS_INFO(msLogger, "IPC request error");
    }
}


PmpPim::PmpPim()
    : mProtocol(NULL)
    , mIpc(NULL)
    , mCore(NULL)
    , mRegistry(new ClientAppRegistry())
    , mEventFactory(0)
    , mAmpRequest(0)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    assert(mRegistry);
}

void PmpPim::setPimProtocol(PmpPimProtocol* pProtocol)
{
    mProtocol = pProtocol;
}

void PmpPim::setEventFactory(EventFactory * factory)
{
    mEventFactory = factory;
}

void PmpPim::setIpc(Ipc::PmpIpcProtocol* pIpc)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    mIpc = pIpc;
    assert(mIpc);
}

void PmpPim::setCore(iviLink::PMP::IPmpCoreToPim *pCore)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mCore = pCore;
}

void PmpPim::setAmp(AppMan::IAmpRequest *amp)
{
    mAmpRequest = amp;
    assert(mAmpRequest);
}

PmpPim::~PmpPim()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    delete mRegistry;
}

void PmpPim::saveIncomingProfileForProcessing(iviLink::BaseUid const& appId,
        iviLink::Profile::Uid const& profileUid,
        iviLink::Profile::IUid const& piuid,
        iviLink::Service::Uid const& sid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    // Here is the thread border

    mEventFactory->pimProcessIncomingProfile(appId, profileUid, piuid, sid);
}

void PmpPim::processIncomingProfile(iviLink::BaseUid const& appId,
        iviLink::Profile::Uid const& profileUid,
        iviLink::Profile::IUid const& piuid,
        iviLink::Service::Uid const& sid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, std::string(__PRETTY_FUNCTION__) +
            " appId = " + appId.value() +
            " profileUid = " + profileUid.value() +
            " piuid = " + piuid.value() +
            " sid = " + sid.value());

    Profile::ApiUid papiUid;
    if (mCore)
    {
        LOG4CPLUS_INFO(msLogger, "looking for papiUid of profile");
        bool res = mCore->getPapiUid(profileUid, papiUid);
        if (res)
        {
            LOG4CPLUS_INFO(msLogger, "found papiUid " + papiUid.value());
        }
        else
        {
            LOG4CPLUS_INFO(msLogger, "unable to find papiUid for " + profileUid.value());
            return;
        }
    }
    else
    {
        LOG4CPLUS_ERROR(msLogger, "mCore is NULL - unable to get papiUid of profile");
        return;
    }

    iviLink::Ipc::DirectionID dirId = -1;
    BaseError err = mRegistry->findDirIdByApp(appId, dirId);
    if (!err.isNoError())
    {
        LOG4CPLUS_INFO(msLogger, static_cast<std::string>(err));
        return;
    }

    LOG4CPLUS_INFO(msLogger, "appId = " + appId.value() + " dirId = " + convertIntegerToString(dirId));

    BaseUid tmpAppId;
    err = mRegistry->findAppByPIUID(piuid, tmpAppId);
    if (err.isNoError())
    {
        LOG4CPLUS_INFO(msLogger, "known piuid " + piuid.value());
        return;
    }

    mIncomingProfiles.insert(piuid);

    bool res = mIpc->incomingProfile(profileUid, papiUid, piuid, sid, dirId);
    if (!res)
    {
        LOG4CPLUS_INFO(msLogger, "IPC request error");
    }
}

}  // namespace ProfileManager
}  // namespace AXIS

