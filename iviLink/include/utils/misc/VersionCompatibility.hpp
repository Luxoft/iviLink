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


#ifndef VERSION_COMPATIBILITY_HPP_
#define VERSION_COMPATIBILITY_HPP_

#include <string>
#include <map>

#include "Types.hpp"
#include "iviLinkVersion.hpp"

namespace iviLink
{
namespace Utils
{

enum Compatibilities
{
    IVILINK_COMPATIBLE      = 0,
    IVILINK_NOT_COMPATIBLE  = 1,
    IVILINK_UNKNOWN_VERSION = 2
};


class VersionCompatibility
{
public:

    VersionCompatibility();

    ~VersionCompatibility();

    /**
     * check compatibility of 2 versions
     * @param version1 first version for checking
     * @param version2 second version for checking
     * @return IVILINK_COMPATIBLE if versions are compatible
     *         IVILINK_NOT_COMPATIBLE if versions are not compatible and
     *         IVILINK_UNKNOWN_VERSION if version on our side is less than other side version
     */
    int checkCompatibility(UInt16 ourVersion, UInt16 otherSideVersion);

private:
    std::multimap<UInt16, UInt16> compatMap;
};

} //namespace Utils
} //namespace iviLink

#endif //VERSION_COMPATIBILITY_HPP_
