/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 26.03.2013
 *
 * Structures for ServicesInfoProvider
 */

#ifndef iviLinkCoreStack_ServicesInfoTypes_h
#define iviLinkCoreStack_ServicesInfoTypes_h

namespace iviLink
{
struct ProfileInfo
{
    Profile::Uid mUid; // profile implementation Uid of a local profile
    Profile::Uid mComplementUid; // profile implementation Uid of a complementing remote profile
    UInt32 mVersion; // version of the profile and its complement
    
    ProfileInfo()
    {
        mUid = Profile::Uid("");
        mComplementUid = Profile::Uid("");
        mVersion = 0;
    }
    // assymmetrical profiles case
    ProfileInfo(std::string const& uid, std::string const& complementUid, UInt32 version)
    {
        mUid = Profile::Uid(uid);
        mComplementUid = Profile::Uid(complementUid);
        mVersion = version;
    }
    // for the case when the profile complements self
    ProfileInfo(std::string const& uid, UInt32 version)
    {
        mUid = Profile::Uid(uid);
        mComplementUid = Profile::Uid(uid);
        mVersion = version;
    }
};

struct ProfileApiInfo
{
    Profile::ApiUid mApiUid; // profile API uid
    ProfileInfo mImplInfo; // information about implementation of the api and it's complement

    ProfileApiInfo()
    {
        mApiUid = Profile::ApiUid("");
    }

    ProfileApiInfo(std::string const& apiUid, ProfileInfo const& profileInfo)
    {
        mApiUid = Profile::ApiUid(apiUid);
        mImplInfo = profileInfo;
    }
    // for the case when the profile complements self
    ProfileApiInfo(std::string const& apiUid, std::string const& profileImpl, UInt32 profileVersion)
    {
        mApiUid = Profile::ApiUid(apiUid);
        mImplInfo = ProfileInfo(profileImpl, profileVersion);
    }
};

typedef std::list<ProfileApiInfo> tApisList;

struct ServiceInfo
{
    Service::Uid mServiceUid; // uid of a service that can be loaded
    Service::Uid mServiceComplement; // uid of a service that should be loaded in response (complement)
    tApisList mApiInfos; // profile APIs of the service

    ServiceInfo()
    {
        mServiceUid = Service::Uid("");
        mServiceComplement = Service::Uid("");
    }

    // simplest case: service complements self, consists only of one API, and the API's implementation also complements self
    ServiceInfo(std::string const& serviceUid, std::string const& apiUid,
            std::string const& implUid, UInt32 profileVersion)
    {
        mServiceUid = Service::Uid(serviceUid);
        mServiceComplement = Service::Uid(serviceUid);
        mApiInfos.push_back(ProfileApiInfo(apiUid, implUid, profileVersion));
    }
    // service complements self, contains one profile API
    ServiceInfo(std::string const& uid, ProfileApiInfo const& apiInfo)
    {
        mServiceUid = Service::Uid(uid);
        mServiceComplement = Service::Uid(uid);
        mApiInfos.push_back(apiInfo);
    }
    // service complements self, consists of multiple APIs
    ServiceInfo(std::string const& uid, tApisList apiInfo)
    {
        mServiceUid = Service::Uid(uid);
        mServiceComplement = Service::Uid(uid);
        mApiInfos = apiInfo;
    }

    ServiceInfo(std::string const& uid, std::string const& complement,
            ProfileApiInfo const& apiInfo)
    {
        mServiceUid = Service::Uid(uid);
        mServiceComplement = Service::Uid(complement);
        mApiInfos.push_back(apiInfo);
    }

    ServiceInfo(std::string const& uid, std::string const& complement, tApisList apiInfo)
    {
        mServiceUid = Service::Uid(uid);
        mServiceComplement = Service::Uid(complement);
        mApiInfos = apiInfo;
    }
};

typedef std::map<std::string, ServiceInfo> tServiceInfoMap;
} // namespace iviLink

#endif
