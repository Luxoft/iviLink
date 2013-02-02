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


#include <algorithm>
#include <cassert>

#include "ClientAppRegistry.hpp"
#include "helpers.hpp"

const char gModuleName[] = "ClientAppRegistry";


namespace iviLink
{
namespace PMP
{
namespace PIM
{

Logger ClientAppRegistry::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.PIM.ClientAppRegistry"));

struct ClientAppRegistry::IsPiuidInProfDataPred
    : public std::unary_function<ProfileData const&, bool>
{
    iviLink::Profile::IUid const& piuid;

    IsPiuidInProfDataPred(iviLink::Profile::IUid const& piuid)
        : piuid(piuid)
    {
    }

    bool operator()(ProfileData const& pdata) const
    {
        return pdata.piuid == piuid;
    }
};

struct ClientAppRegistry::IsProfileUidInProfDataPred
    : public std::unary_function<ProfileData const&, bool>
{
    iviLink::Profile::Uid const& uid;

    IsProfileUidInProfDataPred(iviLink::Profile::Uid const& uid)
        : uid(uid)
    {
    }

    bool operator()(ProfileData const& pdata) const
    {
        return pdata.profileUid == uid;
    }
};

BaseError ClientAppRegistry::registerApp(BaseUid const& appId, iviLink::Ipc::DirectionID const& dirId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_INFO(msLogger, "appId: " + appId.value());
    BaseError err = BaseError::NoError(gModuleName);

    AppDatas::iterator it = mAppDatas.find(appId);
    if (it != mAppDatas.end())
    {
        err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "appId already known");
    }
    else
    {
        mAppDatas.insert(std::make_pair(appId, AppData()));
    }

    if (err.isNoError())
    {
        mAppProfiles.insert(std::make_pair(appId, ProfileDataSet()));
        assert(mDirIdAppMap.find(dirId) == mDirIdAppMap.end());
        mDirIdAppMap.insert(std::make_pair(dirId, appId));
    }

    return err;
}

BaseError ClientAppRegistry::unregisterApp(BaseUid const& appId)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    BaseError err = BaseError::NoError(gModuleName);

    AppDatas::iterator it = mAppDatas.find(appId);
    if (it == mAppDatas.end())
    {
        err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "appId is unknown");
    }
    else
    {
        mAppDatas.erase(it);
    }

    if (err.isNoError())
    {
        mAppProfiles.erase(appId);

        PairValueEquealPred<DirIdAppMap::value_type> pred(appId);
        DirIdAppMap::iterator it = std::find_if(mDirIdAppMap.begin(), mDirIdAppMap.end(), pred);
        assert(it != mDirIdAppMap.end());
        mDirIdAppMap.erase(it);
    }

    return err;
}

BaseError ClientAppRegistry::registerProfile(BaseUid const& appId, ProfileData const& data)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_INFO(msLogger, "appId '" + appId.value() + "'");

    BaseError err = BaseError::NoError(gModuleName);

    ProfileToApp::iterator it = mProfileToApp.find(data.piuid);
    if (it != mProfileToApp.end())
    {
        err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "piuid already known");
    }
    else
    {
        mProfileToApp.insert(std::make_pair(data.piuid, appId));
    }

    if (err.isNoError())
    {
        AppProfiles::iterator it = mAppProfiles.find(appId);
        assert(it != mAppProfiles.end());

        ProfileDataSet& ds = it->second;
        ds.insert(data);
    }

    return err;
}

BaseError ClientAppRegistry::unregisterProfile(iviLink::Profile::IUid const& piuid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    BaseError err = BaseError::NoError(gModuleName);

    BaseUid appId;

    ProfileToApp::iterator it = mProfileToApp.find(piuid);
    if (it == mProfileToApp.end())
    {
        err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "piuid is unknown");
    }
    else
    {
        appId = it->second;
        mProfileToApp.erase(piuid);
    }

    if (err.isNoError())
    {
        AppProfiles::iterator it = mAppProfiles.find(appId);
        assert(it != mAppProfiles.end());
        ProfileDataSet& ds = it->second;
        IsPiuidInProfDataPred pred(piuid);
        ProfileDataSet::const_iterator dsit = std::find_if(ds.begin(), ds.end(), pred);
        if (dsit != ds.end())
        {
            ds.erase(dsit);
        }
    }

    return err;
}

BaseError ClientAppRegistry::findAppByDirId(iviLink::Ipc::DirectionID const& dirId, BaseUid& appId) const
{
    BaseError err = BaseError::NoError(gModuleName);

    DirIdAppMap::const_iterator it = mDirIdAppMap.find(dirId);
    if (it == mDirIdAppMap.end())
    {
        err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "given dirId is unknown");
    }
    else
    {
        appId = it->second;
    }

    return err;
}

BaseError ClientAppRegistry::findDirIdByApp(BaseUid const& appId, iviLink::Ipc::DirectionID& dirId) const
{
    BaseError err = BaseError::NoError(gModuleName);

    PairValueEquealPred<DirIdAppMap::value_type> pred(appId);
    DirIdAppMap::const_iterator it = std::find_if(mDirIdAppMap.begin(), mDirIdAppMap.end(), pred);
    if (it == mDirIdAppMap.end())
    {
        err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "given appId is unknown");
    }
    else
    {
        dirId = it->first;
    }

    return err;
}

BaseError ClientAppRegistry::findAppByPIUID(iviLink::Profile::IUid const& piuid, BaseUid& appId) const
{
    BaseError err = BaseError::NoError(gModuleName);

    ProfileToApp::const_iterator it = mProfileToApp.find(piuid);
    if (it == mProfileToApp.end())
    {
        err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "piuid is unknown");
    }
    else
    {
        appId = it->second;
    }

    return err;
}


BaseError ClientAppRegistry::findSidByPIUID(iviLink::Profile::IUid const& piuid, iviLink::Service::Uid & sid) const
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    iviLink::BaseUid appId;
    BaseError err = findAppByPIUID(piuid, appId);

    if (!err.isNoError())
    {
        return err;
    }

    AppProfiles::const_iterator it = mAppProfiles.find(appId);
    if (it != mAppProfiles.end())
    {
        IsPiuidInProfDataPred pred(piuid);
        ProfileDataSet const& ds = it->second;
        ProfileDataSet::const_iterator dsit = std::find_if(ds.begin(), ds.end(), pred);
        if (dsit != ds.end())
        {
            sid = dsit->sid;
        }
        else
        {
            err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "piuid is unknown");
        }
    }
    else
    {
        err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "app for this piuid is unknown");
    }

    return err;
}

bool ClientAppRegistry::hasInstancesOf(iviLink::Profile::Uid const& uid) const
{
    bool res = false;

    IsProfileUidInProfDataPred pred(uid);

    for (AppProfiles::const_iterator it = mAppProfiles.begin(); it != mAppProfiles.end(); ++it)
    {
        ProfileDataSet const& ds = it->second;
        ProfileDataSet::const_iterator dsit = std::find_if(ds.begin(), ds.end(), pred);
        if (dsit != ds.end())
        {
            res = true;
            break;
        }
    }

    return res;
}

void ClientAppRegistry::findInstancesOf(iviLink::Profile::Uid const& uid, std::vector<ProfileInstanceData> & result) const
{
    IsProfileUidInProfDataPred pred(uid);

    for (AppProfiles::const_iterator it = mAppProfiles.begin(); it != mAppProfiles.end(); ++it)
    {
        ProfileDataSet const& ds = it->second;

        for (ProfileDataSet::const_iterator dsit = ds.begin(); dsit != ds.end(); ++dsit)
        {
            ProfileData const& pd = *dsit;

            if (pred(pd))
            {
            ProfileInstanceData data = {pd.piuid, pd.sid, it->first};
            result.push_back(data);
            }
        } // for dsit
    } // for it
}

void ClientAppRegistry::getProfilesOfApp(BaseUid const& appId, std::vector<iviLink::Profile::IUid> & result) const
{
    AppProfiles::const_iterator it = mAppProfiles.find(appId);
    if (it != mAppProfiles.end())
    {
        ProfileDataSet const& ds = it->second;

        for (ProfileDataSet::const_iterator dsit = ds.begin(); dsit != ds.end(); ++dsit)
        {
            ProfileData const& pd = *dsit;
            result.push_back(pd.piuid);
        }
    }
}

bool ClientAppRegistry::isAppHaveService(iviLink::BaseUid const& appId, iviLink::Service::Uid const& sid) const
{
    bool res = false;
    AppDatas::const_iterator it = mAppDatas.find(appId);
    if (it == mAppDatas.end())
    {
        LOG4CPLUS_WARN(msLogger, "unknown app " + appId.value());
        res = false;
    }
    else
    {
        res = it->second.hasService(sid);
    }
    return res;
}

void ClientAppRegistry::addServiceToApp(iviLink::BaseUid const& appId, iviLink::Service::Uid const& sid)
{
    AppDatas::iterator it = mAppDatas.find(appId);
    if (it == mAppDatas.end())
    {
        LOG4CPLUS_WARN(msLogger, "unknown app: " + appId.value());
    }
    else
    {
        it->second.addService(sid);
    }
}

void ClientAppRegistry::removeServiceFromApp(iviLink::BaseUid const& appId, iviLink::Service::Uid const& sid)
{
    AppDatas::iterator it = mAppDatas.find(appId);
    if (it == mAppDatas.end())
    {
        LOG4CPLUS_WARN(msLogger, "unknown app: " + appId.value());
    }
    else
    {
        it->second.removeServie(sid);
    }
}

BaseError ClientAppRegistry::findAppBySid(iviLink::Service::Uid const& sid, iviLink::BaseUid & appId) const
{
    BaseError err = BaseError::NoError(gModuleName);
    AppDatas::const_iterator it = mAppDatas.begin();
    for ( ; it != mAppDatas.end(); ++it)
    {
        if (it->second.hasService(sid))
        {
            appId = it->first;
            break;
        }
    }

    if (it == mAppDatas.end())
    {
        err = BaseError(1, gModuleName, BaseError::IVILINK_ERROR, "app for this sid is unknown");
    }

    return err;
}


ClientAppRegistry::ClientAppRegistry()
{
}

ClientAppRegistry::~ClientAppRegistry()
{
}


}  // namespace PIM
}  // namespace PMP
}  // namespace iviLink
