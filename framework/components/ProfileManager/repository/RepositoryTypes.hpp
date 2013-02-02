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


#ifndef PMP_REPOSITORY_TYPES_HPP_
#define PMP_REPOSITORY_TYPES_HPP_

#include <string>
#include <map>

#include "Types.hpp"
#include "UID.hpp"

namespace iviLink
{
/**
* Profile Implementation library descriptor
*/
struct LibDescriptor
{
    std::string platform;   ///< platform
    std::string libPath;    ///< path to library
};

/**
* Structure is used to in findProfiles functionality of Profile Repository to represent results
*/
struct LibInfo
{
    /**
    * Constructor
    */
    LibInfo()
    {

    }

    /**
    * Constructor
    */
    LibInfo(Profile::Uid profileUid, std::string libPlatform, std::string libPath, UInt32 libRelevance = 100)
        : uid(profileUid)
        , platform(libPlatform)
        , path(libPath)
        , relevance(libRelevance)
    {
    }

    Profile::Uid uid;       ///< UID of Profile
    std::string platform;   ///< string with platform
    std::string path;       ///<path to library
    UInt32 relevance;       ///< Relevance of result 0...100
};

}

#endif //PMP_REPOSITORY_TYPES_HPP_

