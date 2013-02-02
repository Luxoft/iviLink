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


#ifndef CPMPCOREPROFILEINFO_HPP_
#define CPMPCOREPROFILEINFO_HPP_


#include <string>

#include "Logger.hpp"
#include "UID.hpp"

namespace iviLink
{

namespace PMP
{

/**
* Used for representing of Profile in PMP Core
*/
class PmpCoreProfileInfo
{
public:

    /**
    * Default constructor
    */
    PmpCoreProfileInfo();

    /**
    * Constructor. Parses manifest and sets members
    */
    PmpCoreProfileInfo(const std::string & profileManifest);

    /**
    * Copy constructor
    */
    PmpCoreProfileInfo(const PmpCoreProfileInfo &info);

    /**
    * Operator =
    */
    PmpCoreProfileInfo& operator = (const PmpCoreProfileInfo &info);

    /**
    * Destructor
    */
    ~PmpCoreProfileInfo();

    /**
    * Returns UID of Profile
    */
    Profile::Uid uid() const;

    /**
    * Returns Profile API UID that implements Profile
    */
    Profile::ApiUid api() const;

    /**
    * Returns UID of complement Profile
    */
    Profile::Uid complement() const;

    /**
    * Returns path to DLL library for current system
    */
    std::string library() const;

    /**
    * @return true if Profile is locked
    */
    bool locked() const;

    /**
    * @return true if Profile is enabled
    */
    bool enabled() const;

    /**
    * @return true if Profile is available (enabled and not locked)
    */
    bool available() const;

    /**
    * locks Profile
    */
    void lock();

    /**
    * unlocks Profile
    */
    void unlock();

    /**
    * enables Profile by clients request
    */
    void enableByClient();

    /**
    * disables Profile by clients request
    */
    void disableByClient();

    /**
    * disables Profile because of no complement on another side
    */
    void disableByComplement();

    /**
    * enables Profile because of complement on another side
    */
    void enableByComplement();

    /**
    * Sets path of DLL library
    */
    void setLibrary(const std::string & library);

private:
    /**
    * Parses XML manifest and loads parsed data
    */
    void parseXml(const std::string & profileManifest);

    bool mEnabledByComplement;  ///< true if profile has complement on another side
    bool mEnabledByClient;      ///< true if profile is enabled by client's request
    bool mLocked;               ///< true if profile is locked
    Profile::Uid mUid;          ///< UID of Profile
    Profile::ApiUid mApi;       ///< UID of Profile API
    Profile::Uid mComplement;   ///< UID of Profile Complement
    std::string mLibrary;       ///< Path to DLL with Profile Implementation

    static Logger msLogger;     ///< object of logger
};

}

}

#endif /* CPMPCOREPROFILEINFO_HPP_ */
