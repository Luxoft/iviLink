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
 * @file                ProfileInfo.cpp
 * @ingroup             Profile Manager
 * @author              Plachkov Vyacheslav <vplachkov@luxoft.com>
 * @date                10.01.2013
 *
 * Implements ProfileInfo class
 */


#include <cassert>
#include <cstdlib>
#include <algorithm>

#include "pugixml.hpp"
#include "ProfileInfo.hpp"

namespace iviLink
{
namespace PMP
{

Logger ProfileInfo::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Repository"));

ProfileInfo::ProfileInfo(const Profile::Uid & profile, const Profile::ApiUid & apiUid,
        const Profile::Uid & complement, const std::string & name)
    : mUid(profile)
    , mName(name)
    , mApiUid(apiUid)
    , mComplement(complement)
{
}

ProfileInfo::~ProfileInfo()
{
}

const Profile::Uid & ProfileInfo::uid() const
{
    return mUid;
}

const std::string & ProfileInfo::name() const
{
    return mName;
}

const Profile::ApiUid & ProfileInfo::apiUid() const
{
    return mApiUid;
}

const std::map<std::string,std::string> &ProfileInfo::attributes() const
{
    return mAttributes;
}

const std::vector<ProfileLibInfo> & ProfileInfo::libs() const
{
    return mLibs;
}

void ProfileInfo::addLib(const ProfileLibInfo & libInfo)
{
    mLibs.push_back(libInfo);
    std::sort(mLibs.begin(), mLibs.end());
}

void ProfileInfo::print() const
{
    LOG4CPLUS_INFO(msLogger, "ProfileInfo::print\nProfile UID: " + mUid.value() +
            ", API UID: " + mApiUid.value() + ", Complement UID: " + mComplement.value() +
            ", name: " + mName);
}

const Profile::Uid & ProfileInfo::complement() const
{
    return mComplement;
}

}
}
