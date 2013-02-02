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


#include <cassert>
#include <cstdlib>

#include "pugixml.hpp"
#include "ProfileApiInfo.hpp"

namespace iviLink
{
namespace PMP
{

Logger ProfileApiInfo::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Repository"));

ProfileApiInfo::ProfileApiInfo()
{
}

ProfileApiInfo::ProfileApiInfo(std::string xmlPath)
    : mXmlPath(xmlPath)
    , mFailed(false)
{
    loadProfileApiInfo();
}

std::string ProfileApiInfo::xmlPath() const
{
    return mXmlPath;
}

ProfileApiInfo::~ProfileApiInfo()
{
}

Profile::ApiUid ProfileApiInfo::uid() const
{
    return mUid;
}

std::string ProfileApiInfo::role() const
{
    return mRole;
}

int ProfileApiInfo::version() const
{
    return mVersion;
}

void ProfileApiInfo::loadProfileApiInfo()
{
    pugi::xml_document doc;

    pugi::xml_parse_result res = doc.load_file(mXmlPath.c_str());
    if (pugi::status_ok != res.status)
    {
        mFailed = true;
        LOG4CPLUS_ERROR(msLogger, "Error while parsing Profile API manifest\n"
        "Path: " + mXmlPath);
        return;
    }
    pugi::xml_node profApi = doc.child("profileApi");
    mRole = profApi.child_value("role");
    mUid = Profile::ApiUid(profApi.child_value("uid"));
    mVersion = atoi(profApi.child_value("version"));
}

void ProfileApiInfo::print() const
{
    std::string print = "*** Profile API\n"
            " UID: " + mUid.value() +
            " Role: " + mRole +
            " Version: " + convertIntegerToString(mVersion) +
            " XML Path: " + mXmlPath;
    LOG4CPLUS_INFO(msLogger, print);
}

bool ProfileApiInfo::failed() const
{
    return mFailed;
}

}
}

