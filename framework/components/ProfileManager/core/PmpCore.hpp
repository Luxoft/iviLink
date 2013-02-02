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


#ifndef CPMPCORE_HPP_
#define CPMPCORE_HPP_

#include <map>
#include <list>

#include "Logger.hpp"
#include "BaseError.hpp"

#include "IPmpPimToCore.hpp"
#include "RepositoryTypes.hpp"
#include "IPmpCoreToPim.hpp"


namespace iviLink
{

namespace PMP
{

class PmpCoreProfileInfo;
class ProfileRepository;
class PmpCoreProtocol;
class EventFactory;

class PmpCore : public IPmpCoreToPim
{
/**
* This class is used to make PMP Core functionality. This functionality consists of
* loading information about Profiles from Profile Repository, interaction with another
* side of system to enable Profiles that have complement on another side and
* perform actions like enable/disable Profile by client request and lock/unlock by Profile
* Update Profile request. This requests has influence on two sides of system (HU and device)
*/

public:
    typedef std::map<Profile::Uid,PmpCoreProfileInfo> CoreProfileInfo;

    typedef std::list<Profile::Uid> tComplementsList;

    /**
    * Constructor
    */
    PmpCore(ProfileRepository * repository, std::string &platform);

    /**
    * Destructor
    */
    virtual ~PmpCore();

    /**
    * Makes request to Profile Repository to add Profile
    * @param manifestPath is path to Profile XML manifest
    * @return error code
    */
    BaseError addProfile(const std::string &xmlManifestPath);

    /**
    * Makes request to Profile Repository to remove profile from repository
    * @param profileUid is UID of profile to remove
    * @return error code
    */
    BaseError removeProfile(iviLink::Profile::Uid profileUid);

    /**
    * Makes request to Profile Repository to add Profile Implementation
    * @param profileID is UID of profile
    * @param library is library with profile implementation library
    * @return error code
    */
    BaseError addProfileImplementation(iviLink::Profile::Uid profileID, const LibDescriptor& library);

    /**
    * Makes request to Profile Repository to remove profile implementation
    * @param profileUid is UID of profile
    * @param platform is a target platform of profile implementation
    * @return error code
    */
    BaseError removeProfileImplementation(iviLink::Profile::Uid profileUid, const std::string &platform);

    /**
    * Makes request to Profile Repository to find Profiles by UID and Profile parameters
    * @param id is UID of Profile or Profile API
    * @param profileArguments is a map of Profile Parametes
    * @param platform is target platform
    * @prarm onlyAvailable is used to get enabled and unlocked profiles
    */
    void findProfiles(Profile::ApiUid id, const std::map<std::string, std::string> & profileArguments,
            const std::string &platform, std::list<LibInfo>& profiles, bool onlyAvailable = true);

    /**
    * Function is used to clear cached Profiles information and load actual from Repository
    */
    void reloadProfilesFromRepository();

    /**
    * Finds complements for local profiles on another side and enables this pairs
    * of Profiles, resets locks and client's disables
    */
    void resetProfilesState();

    /**
    * Locks profiles on two sides (profile on 1 side and complement on enother.
    * If profile has instance - fails
    * @param id is UID of Profile
    * @return true if success
    */
    bool lock(iviLink::BaseUid id);

    /**
    * Unlocks profile on two sides (profile from one side and complement from another side)
    * @param id is UID of Profile
    * @return true if success
    */
    bool unlock(iviLink::BaseUid id);

    /**
    * Locks all profiles in system
    */
    void lockAll();

    /**
    * Unlocks all profiles in system
    */
    void unlockAll();

    /**
    * Disables Profile and complement (on another side) by client's request
    * @param id is UID of Profile to disable
    * @return true if success and false if UID is unknown
    */
    virtual bool disableByClient(iviLink::BaseUid id);

    /**
    * Enables Profile and complement (on another side) by client's request
    * @param id is UID of Profile
    * @return true if success and false if UID is unknown
    */
    virtual bool enableByClient(iviLink::BaseUid id);

    /**
    * Enables all Profiles by client's request
    */
    virtual void enableByClientAll();

    /**
    * Makes request to Profile Repository to get Profile of Profile API manifest
    * @param uid is UID of Profile of Profile API
    * @manifest[out] is result string with manifest
    * @return true if UID is unknown
    */
    virtual bool getManifest(iviLink::BaseUid uid, std::string & manifest);

    /**
    * Calls another version of findProfiles() function with list of LibInfo objects.
    * This version of function is used to make requests by clients of Profile Manager
    * @param id is UID of Profile API of Profile
    * @param profileParameters is map of Profile Parameters
    * @param[out] profiles is list of results
    * @param enabledProfiles is true if client want to get only enabled and unlocked Profiles
    */
    virtual void findProfiles(Profile::ApiUid id,
            const std::map<std::string, std::string> & profileParameters
            , std::list<Profile::Uid> & profiles, bool enabledProfiles = true);

    /**
    * Function is used to get path to Profile Implementation library
    * @param uid is UID of Profile
    * @param[out] path is path to Profile Implementation library
    * @return true if success and false if UID is unknown
    */
    virtual bool getProfileLibPath(Profile::Uid uid, std::string& path);

    /**
    * @return true if state of PMP Core is equal to Profile Repository state
    */
    bool actualCoreAndRepoState() const;

    /**
    * Initializes PMP Core and gives interface to PIM to Core
    */
    void init(IPmpPimToCore * pPim, PmpCoreProtocol * pCoreProtocol, EventFactory * eventFactory);

    /**
    * Function is used to Log state of Core members
    */
    void print() const;

    virtual bool getComplementary(Profile::Uid const& profUid,
            Profile::Uid& complProfUid) const;

    virtual bool getPapiUid(iviLink::Profile::Uid const& profUid,
            iviLink::Profile::ApiUid& papiUid) const;

    // from IPmpCoreToSysCtrl

    virtual bool unlockAuthProfile();

    virtual void unlockBySysCtrlAll();

    /**
    * Disables all Profiles (is used if it's no connection with another side or
    * to reset state for making re-enabling after this function
    */
    void disableByComplementAll();

    /**
    * Callback is called by another side to get list of Profiles that may be
    * complement for profiles on another side
    * @param[out] complements is list of profiles on this side (for another side it's
    * a complement Profiles
    */
    virtual void getAvailableProfileComplements();

    /**
    * Callback is used to lock Profile by request from another side
    * @param id is UID of Profile
    * @return true if success
    */
    virtual bool onLock(iviLink::BaseUid id);

    /**
    * Callback is used to unlock Profile by request from another side
    * @param id is UID of Profile
    * @return true if success
    */
    virtual bool onUnlock(iviLink::BaseUid id);

    /**
    * Callback is used to lock all Profiles by request from another side
    */
    virtual void onLockAll();


    /**
    * Callback is used to unlock all Profiles by request from another side
    */
    virtual void onUnlockAll();

    /**
    * Disables Profile by request from another side
    * @param id is UID of Profile
    * @return true if success
    */
    virtual bool onDisableByClient(iviLink::BaseUid id);

    /**
    * Enables Profile by request from another side
    * @param id is UID of Profile
    * @return true if success
    */
    virtual bool onEnableByClient(iviLink::BaseUid id);

    /**
    * Enables all Profiles by request from another side
    */
    virtual void onEnableByClientAll();

    /**
    * Request from another side to make reload Profiles from Profile Repository
    */
    virtual void onReloadProfilesFromRepository();

    /**
    * Request from another side to reset Profiles state in PMP Core
    */
    virtual void onResetProfilesState();

    /**
    * Callback is used to inform about losing of connection
    */
    virtual void onLostConnection();

    /**
    * Function is used by lockByUid() to lock Profiles that inherit given API
    * @param id is UID of Profile API
    * @return true if one or more Profiles were locked
    */
    bool lockByApiUid(Profile::ApiUid id);

    /**
    * Function is used by unclockByUid() to unlock Profiles that inherit given API
    * @param id is UID of Profile API
    * @return true if one or move Profiles were unlocked
    */
    bool unlockByApiUid(Profile::ApiUid id);

    /**
    * Function is used by disableByClient() to disable by client Profiles that inherit given API
    * @param id is UID of Profile API
    * @return true if one or more Profiles were disabled
    */
    bool disableApiUidByClient(Profile::ApiUid id);

    /**
    * Function is used by enableByClient() to enable by client Profiles that inherit given API
    * @param id is UID of Profile API
    * @return true if one or more Profiles were disabled
    */
    bool enableApiUidByClient(Profile::ApiUid id);

    void getAvailbleProfileComplementsResponse(const std::list<Profile::Uid> & complements);

private:

    ProfileRepository * mRepository; ///< Pointer to Profile Repository client
    CoreProfileInfo mProfiles; ///< Map of Profiles. Core gets if from Repository

    std::string mPlatform;  ///< Contains name of current platform
    bool mActualRepoState;  ///< is true if state of Core is similar with Profile Repository state
    IPmpPimToCore * mpPim; ///< interface to PIM
    PmpCoreProtocol * mpCoreProtocol; ///<interface to PMP Protocol

    EventFactory * mEventFactory;

    static Logger msLogger; ///< object of logger
};

}

}

#endif /* CPMPCORE_HPP_ */
