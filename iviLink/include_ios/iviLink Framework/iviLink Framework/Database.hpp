/**
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 22.03.2013
 * Provides information about known services, APIs and profile implementations
 * using ServicesInfoProvider. Also manages locking/unlocking of profiles.
 */

#ifndef __iviLinkCoreStack__Database__
#define __iviLinkCoreStack__Database__

#include <set>

#include "CRWMutex.hpp"
#include "Logger.hpp"
#include "ServicesInfoProvider.hpp"

typedef std::list<std::pair<std::string, UInt32>> tProfilesAndVersionsList;

namespace iviLink
{
namespace PMP
{
struct EnabledProfileInfo
{
    std::string mProfileImplUid;
    bool mIsEnabled;

    EnabledProfileInfo(std::string const& profileUid)
    {
        mProfileImplUid = profileUid;
        mIsEnabled = false;
    }

    EnabledProfileInfo()
    : mProfileImplUid("")
    {
        mIsEnabled = false;
    }

    EnabledProfileInfo(const EnabledProfileInfo& info)
    {
        mProfileImplUid = info.mProfileImplUid;
        mIsEnabled = info.mIsEnabled;
    }
};

typedef std::list<EnabledProfileInfo> tEnabledProfilesList;
    
class Database
{
private:
    Database();
    ~Database();
    Database(const Database&);
    Database& operator=(const Database&);
public:
    static Database * getInstance();
    /**
     * Mark the authentication profile uid as enabled.
     */
    void enableAuthenticationProfileLocally();
    /**
     * Mark the authentication profile uid as enabled
     * in list of remote profiles.
     */
    void enableAuthenticationProfileRemote();

    /**
     * Mark all local profiles as disabled.
     */
    void lockAll();
    /**
     * Mark all local profiles as enabled.
     */
    void unlockAll();
    /**
     * Mark all remote profiles as disabled.
     */
    void lockAllRemote();
    /**
     * Mark all remote profiles as enabled.
     */
    void unlockAllRemote();

    bool isLocalProfileEnabled(std::string const&) const;
    bool isRemoteProfileEnabled(std::string const&) const;
    /**
     * @return true if information about this service was contained in the
     * ServicesInfoProvider implementation, and (after PMP handshake) if the remote side
     * has complements for all uids belonging to the service.
     */
    bool isKnownService(std::string const&) const;
    void setAvailableRemoteComplements(tProfilesAndVersionsList complements);

    void forgetRemoteComplements();

    void printDatabase() const;
    /**
     * Get list of all profile UIDs listed in ServicesInfoProvider
     * implementation.
     */
    tProfilesAndVersionsList getAvailableProfiles() const;
    /**
     * Get list of all profile APIs and profile UIDs listed in ServicesInfoProvider
     * implementation for the service.
     */
    tApisList getProfilesAndApis(std::string const&) const;
    std::string getRemoteComplementToLocalService(std::string const&) const;
    std::string getLocalComplementToRemoteService(std::string const&) const;
    UInt32 getVersionForProfileInService(std::string const& profileUid, std::string const& serviceUid) const;
    Profile::Uid getAuthenticationProfileUid() const;
private:
    void updateProfileLists();
private:
    tEnabledProfilesList mProfiles;
    mutable CRWMutex mProfilesMutex;
    
    tEnabledProfilesList mRemoteProfiles;
    mutable CRWMutex mRemoteProfilesMutex;
    
    static Logger mLogger;
    ServicesInfoProvider mInformant;

    mutable CRWMutex mServicesMutex;
    tServiceInfoMap mServicesList;
};
}
}

#endif /* defined(__iviLinkCoreStack__Database__) */
