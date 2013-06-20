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
 * @file                ProfileApiInfo.hpp
 * @ingroup             Profile Manager
 * @author              Plachkov Vyacheslav <vplachkov@luxoft.com>
 * @date                10.01.2013
 *
 * Contains information about Profile API gotten from XML manifest
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
    ProfileApiInfo(const Profile::ApiUid & profileApi);

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
    * Makes logs with Profile API information
    */
    void print() const;

private:
    Profile::ApiUid mUid;               ///< UID of Profile API

    static Logger msLogger; ///< object of logger
};

}
}

#endif /* CPROFILEAPIINFO_HPP_ */
