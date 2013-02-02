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


#ifndef CCLIENTAPPREGISTRY_HPP_
#define CCLIENTAPPREGISTRY_HPP_

#include <map>
#include <set>
#include <vector>

#include "Logger.hpp"
#include "ipc_common.hpp"
#include "BaseError.hpp"

#include "UID.hpp"

#include "AppData.hpp"
#include "ProfileData.hpp"
#include "ProfileInstanceData.hpp"

namespace iviLink
{
namespace PMP
{
namespace PIM
{


class AppData;
class ProfileData;

class ClientAppRegistry
{
public:

    BaseError registerApp(BaseUid const& appId, iviLink::Ipc::DirectionID const& dirId);

    BaseError unregisterApp(BaseUid const& appId);

    BaseError registerProfile(BaseUid const& appId, ProfileData const& data);

    BaseError unregisterProfile(iviLink::Profile::IUid const& piuid);

    BaseError findAppByDirId(iviLink::Ipc::DirectionID const& dirId, BaseUid& appId) const;
    BaseError findDirIdByApp(BaseUid const& appId, iviLink::Ipc::DirectionID& dirId) const;

    BaseError findAppByPIUID(iviLink::Profile::IUid const& piuid, BaseUid& appId) const;

    BaseError findSidByPIUID(iviLink::Profile::IUid const& piuid, iviLink::Service::Uid & sid) const;

    bool hasInstancesOf(iviLink::Profile::Uid const& uid) const;

    void findInstancesOf(iviLink::Profile::Uid const& uid, std::vector<ProfileInstanceData> & result) const;

    void getProfilesOfApp(BaseUid const& appId, std::vector<iviLink::Profile::IUid> & result) const;

    bool isAppHaveService(iviLink::BaseUid const& appId, iviLink::Service::Uid const& sid) const;

    void addServiceToApp(iviLink::BaseUid const& appId, iviLink::Service::Uid const& sid);

    void removeServiceFromApp(iviLink::BaseUid const& appId, iviLink::Service::Uid const& sid);

    BaseError findAppBySid(iviLink::Service::Uid const& sid, iviLink::BaseUid & appId) const;

    /// @todo for testing. Remove it.
    BaseError getFirstApp(iviLink::BaseUid & appId);

    ClientAppRegistry();
    ~ClientAppRegistry();

private:
    ClientAppRegistry(ClientAppRegistry const&);
    ClientAppRegistry& operator=(ClientAppRegistry const&);

    typedef std::map<iviLink::Profile::IUid, iviLink::BaseUid> ProfileToApp;
    ProfileToApp mProfileToApp;

    struct IsPiuidInProfDataPred;
    struct IsProfileUidInProfDataPred;
    struct IsDirIdInAppDataPred;

    typedef std::set<ProfileData> ProfileDataSet;
    typedef std::map<iviLink::BaseUid, ProfileDataSet > AppProfiles;
    AppProfiles mAppProfiles;

    typedef std::map<iviLink::BaseUid, AppData> AppDatas;
    AppDatas mAppDatas;

    typedef std::map<iviLink::Ipc::DirectionID, iviLink::BaseUid> DirIdAppMap;
    DirIdAppMap mDirIdAppMap;

    static Logger msLogger;
};


}  // namespace PIM
}  // namespace ProfileManager
}  // namespace AXIS


#endif /* CCLIENTAPPREGISTRY_HPP_ */
