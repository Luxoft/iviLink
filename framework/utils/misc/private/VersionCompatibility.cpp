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


#include "VersionCompatibility.hpp"

using namespace std;

namespace iviLink
{
namespace Utils
{


VersionCompatibility::VersionCompatibility()
{
    //TODO find out how to do it more beautiful
    // example of adding compatibility pair:
    // compatMap.insert(pair<UInt16, UInt16>((1 << 8) | 1, (1 << 8) | 2)); // 1.1 - 1.2
}

VersionCompatibility::~VersionCompatibility()
{

}

int VersionCompatibility::checkCompatibility(UInt16 ourVersion, UInt16 otherSideVersion)
{
    if (ourVersion == otherSideVersion)
    {
        return IVILINK_COMPATIBLE;
    }

    pair<multimap<UInt16, UInt16>::iterator, multimap<UInt16, UInt16>::iterator> range;

    range = compatMap.equal_range(ourVersion);
    for (multimap<UInt16, UInt16>::iterator it = range.first; it != range.second; ++it)
    {
        if (it->second == otherSideVersion)
        {
            return IVILINK_COMPATIBLE;
        }
    }

    range = compatMap.equal_range(otherSideVersion);
    for (multimap<UInt16, UInt16>::iterator it = range.first; it != range.second; ++it)
    {
        if (it->second == ourVersion)
        {
            return IVILINK_COMPATIBLE;
        }
    }

    if (ourVersion < otherSideVersion)
    {
        return IVILINK_UNKNOWN_VERSION;
    }

    return IVILINK_NOT_COMPATIBLE;
}

} //namespace Utils
} //namespace iviLink
