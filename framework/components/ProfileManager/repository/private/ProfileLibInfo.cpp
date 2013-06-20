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
 * @file                ProfileLibInfo.cpp
 * @ingroup             Profile Manager
 * @author              Plachkov Vyacheslav <vplachkov@luxoft.com>
 * @date                01.04.2013
 *
 * Contains information about Profile lib gotten from XML database
 */

#include "ProfileLibInfo.hpp"

namespace iviLink
{
namespace PMP
{

ProfileLibInfo::ProfileLibInfo(UInt32 version, const std::string & path, const std::string & platform)
	: mVersion(version)
	, mPath(path)
	, mPlatform(platform)
{
}

ProfileLibInfo::ProfileLibInfo(const ProfileLibInfo & profInfo)
	: mVersion(profInfo.mVersion)
	, mPath(profInfo.mPath)
	, mPlatform(profInfo.mPlatform)
{
}

ProfileLibInfo & ProfileLibInfo::operator = (const ProfileLibInfo & profInfo)
{
	if (this != &profInfo)
	{
		mVersion = profInfo.mVersion;
		mPath = profInfo.mPath;
		mPlatform = profInfo.mPlatform;
	}
	return *this;
}

bool ProfileLibInfo::operator < (const ProfileLibInfo & rVal) const
{
    return mVersion < rVal.mVersion;
}

ProfileLibInfo::~ProfileLibInfo()
{
}

const std::string & ProfileLibInfo::platform () const
{
	return mPlatform;
}

UInt32 ProfileLibInfo::version() const
{
	return mVersion;
}

const std::string & ProfileLibInfo::path () const
{
	return mPath;
}

} /* namespace PMP */
} /* namespace iviLink */
