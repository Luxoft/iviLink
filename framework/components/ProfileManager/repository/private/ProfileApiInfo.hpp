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


#ifndef CPROFILEAPIINFO_HPP_
#define CPROFILEAPIINFO_HPP_

#include <string>

#include "Logger.hpp"
#include "RepositoryTypes.hpp"
#include "RepositoryError.hpp"

namespace iviLink
{
namespace PMP
{

/**
* This class is used to present Profile API information in Profile Repository
*/
class ProfileApiInfo
{
public:
    /**
    * Constructor
    * @param xmlPath contains path to manifest. Constructor parses this manifest
    */
    ProfileApiInfo(std::string xmlPath);

    /**
    * Constructor
    */
    ProfileApiInfo();

    /**
    * Destructor
    */
    ~ProfileApiInfo();

    /**
    * @return UID of Profile API
    */
    Profile::ApiUid uid() const;

    /**
    * @return role of Profile API
    */
    std::string role() const;

    /**
    * @return version of Profile API
    */
    int version() const;

    /**
    * @return path to manifest
    */
    std::string xmlPath() const;

    /**
    * Makes logs with Profile API information
    */
    void print() const;

    /**
    * @return true if it was parsing troubles
    */
    bool failed() const;

private:
    /**
    * Loads Profile API info from manifest
    */
    void loadProfileApiInfo();

    std::string mXmlPath;   ///< Manifest path
    Profile::ApiUid mUid;               ///< UID of Profile API
    std::string mRole;      ///< Role of Profile API
    int mVersion;           ///< Version of Profile API
    bool mFailed;           ///< Is state of object incorrect?

    static Logger msLogger; ///< object of logger
};

}
}

#endif /* CPROFILEAPIINFO_HPP_ */
