/* 
 * 
 * iviLINK SDK, version 1.1.2
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
 * 
 */


/**
 * @file                ProfileInfo.hpp
 * @ingroup             Profile Manager
 * @author              Plachkov Vyacheslav <vplachkov@luxoft.com>
 * @date                10.01.2013
 *
 * Contains information about Profile gotten from XML manifest
 */


#ifndef CPROFILEINFO_HPP_
#define CPROFILEINFO_HPP_

#include <string>
#include <map>
#include <vector>

#include "Logger.hpp"
#include "RepositoryTypes.hpp"
#include "ProfileLibInfo.hpp"

namespace iviLink
{
namespace PMP
{
/**
 * Class is used to present information about Profile in Profile Repository
 */
class ProfileInfo
{
public:
    /**
     * Constructor
     * @param profile - Profile UID
     * @param apiUid - UID of inherited Profile API
     * @param complement - UID of complement Profile
     * @param name - Profile name
     */
    ProfileInfo(const Profile::Uid & profile, const Profile::ApiUid & apiUid,
            const Profile::Uid & complement, const std::string & name);

    /**
     * Destructor
     */
    ~ProfileInfo();

    /**
     * @return UID of Profile
     */
    const Profile::Uid & uid() const;

    /**
     * @return name of Profile
     */
    const std::string & name() const;

    /**
     * @return version of Profile
     */
    const Profile::Uid & complement() const;

    /**
     * @return UIA of implemented Profile API
     */
    const Profile::ApiUid & apiUid() const;

    /**
     * @return map of Profile attributes
     */
    const std::map<std::string,std::string> & attributes() const;

    /**
     * @return map of Profile Implementations
     */
    const std::vector<ProfileLibInfo> & libs() const;

    /**
     * Adds profile implementation info
     */
    void addLib(const ProfileLibInfo & libInfo);

    /**
     * Prints Profile information to log
     */
    void print() const;

private:

    Profile::Uid mUid;                 ///< UID of Profile
    std::string mName;                 ///< Name of Profile
    Profile::ApiUid mApiUid;           ///< UID of implemented Profile API
    Profile::Uid mComplement;          ///< UID of complement profile
    std::map<std::string, std::string> mAttributes; ///< Map of Profile Attributes
    std::vector<ProfileLibInfo> mLibs;   ///< Map of Profile Implementations, sorted by version

    static Logger msLogger;           ///< object of logger
};

}
}

#endif /* CPROFILEINFO_HPP_ */
