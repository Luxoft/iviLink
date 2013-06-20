/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 26.03.2013
 * Stores ProfileHolders to be accessed by profile proxies, and redirects 
 * incoming service requests to Applications that support them.
 */

#ifndef __iviLinkCoreStack__CServiceManager__
#define __iviLinkCoreStack__CServiceManager__

#include <map>

#include "BaseProfileApi.hpp"
#include "UID.hpp"
#include "AppInfo.hpp"
#include "Logger.hpp"
#include "CMutex.hpp"
#include "ProfileHolder.hpp"
#include "PublicError.hpp"

#include "ServiceHolder.hpp"

namespace iviLink
{
namespace App
{
namespace Service
{
class CServiceManager
{
private:
    CServiceManager();
    ~CServiceManager();
public:
    static CServiceManager * getInstance(iviLink::Android::AppInfo info =
            iviLink::Android::AppInfo());
    // used by profile proxies
    BaseProfileApi * getProfile(const iviLink::Service::Uid& service, const Profile::ApiUid& api);
    // used by profile proxies
    void releaseProfile(const iviLink::Service::Uid& service, const Profile::ApiUid& api);
    // used by Applications
    void registerProfileCallbacks(const Profile::ApiUid& profileApi,
            Profile::IProfileCallbackProxy* callbacks);
    // used by ServiceHolder
    void setProfileForServiceApi(Profile::ProfileHolder*, const iviLink::Service::Uid& service,
            const Profile::ApiUid& api);
    // used by ServiceHolder
    void eraseProfileRecordForServiceApi(const iviLink::Service::Uid& service,
            const Profile::ApiUid& api);
    // used by ServiceHolder
    Profile::IProfileCallbackProxy * getProfileCallbacks(const Profile::ApiUid& profileApi);
    // called by PmpPIM
    void incomingProfileRequest(std::string const& service, std::string const& profileUid,
            std::string const& profileImplUid);
    // called by PmpPIM
    void incomingProfileDiedRequest(std::string const& service, std::string const& profileImplUid);
private:
    std::string getProfilesMapKey(const iviLink::Service::Uid& service, const Profile::ApiUid& api);
private:
    static Logger mLogger;
    CMutex mProfilesDataMutex;
    // map key is the concatenation of service uid and profile api uid
    std::map<std::string, Profile::ProfileHolder*> mProfilesMap;
    std::map<Profile::ApiUid, Profile::IProfileCallbackProxy*> mCallbacksMap;
    CMutex mCallbacksMutex;
};
}
}
}

#endif /* defined(__iviLinkCoreStack__CServiceManager__) */
