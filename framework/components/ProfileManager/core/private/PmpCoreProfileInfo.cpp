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
 * @file                PmpCoreProfileInfo.cpp
 * @ingroup             Profile Manager
 * @author              Plachkov Vyacheslav <vplachkov@luxoft.com>
 * @date                10.01.2013
 *
 * Implements PmpCoreProfileInfo class
 */


#include "PmpCoreProfileInfo.hpp"
#include "pugixml.hpp"

namespace iviLink
{

namespace PMP
{

Logger PmpCoreProfileInfo::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.PMP.PmpCoreProfileInfo"));

PmpCoreProfileInfo::PmpCoreProfileInfo()
    : mEnabledByComplement(false)
    , mEnabledByClient(true)
    , mLocked(false)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

PmpCoreProfileInfo::PmpCoreProfileInfo(const LibInfo & libInfo)
    : mEnabledByComplement(false)
    , mEnabledByClient(true)
    , mLocked(false)
	, mLibInfo(libInfo)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

PmpCoreProfileInfo::PmpCoreProfileInfo(const PmpCoreProfileInfo &info)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mEnabledByClient = info.mEnabledByClient;
    mEnabledByComplement = info.mEnabledByComplement;
    mLocked = info.mLocked;
    mLibInfo = info.mLibInfo;
}

PmpCoreProfileInfo& PmpCoreProfileInfo::operator = (const PmpCoreProfileInfo &info)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mEnabledByClient = info.mEnabledByClient;
    mEnabledByComplement = info.mEnabledByComplement;
    mLocked = info.mLocked;
    mLibInfo = info.mLibInfo;
    return *this;
}

void PmpCoreProfileInfo::setLibrary(const std::string & library)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

PmpCoreProfileInfo::~PmpCoreProfileInfo()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

Profile::Uid PmpCoreProfileInfo::uid() const
{
    return mLibInfo.uid;
}

Profile::ApiUid PmpCoreProfileInfo::api() const
{
    return mLibInfo.apiUid;
}

Profile::Uid PmpCoreProfileInfo::complement() const
{
    return mLibInfo.complement;
}

std::string PmpCoreProfileInfo::library() const
{
    LOG4CPLUS_INFO(msLogger, "mLibrary = " + mLibInfo.path);
    return mLibInfo.path;
}

bool PmpCoreProfileInfo::locked() const
{
    return mLocked;
}

bool PmpCoreProfileInfo::enabled() const
{
    return mEnabledByClient && mEnabledByComplement;
}

bool PmpCoreProfileInfo::available() const
{
    return enabled() && !mLocked;
}

void PmpCoreProfileInfo::lock()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mLocked = true;
}

void PmpCoreProfileInfo::unlock()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mLocked = false;
}

void PmpCoreProfileInfo::enableByClient()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mEnabledByClient = true;
}

void PmpCoreProfileInfo::disableByClient()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mEnabledByClient = false;
}

void PmpCoreProfileInfo::disableByComplement()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mEnabledByComplement = false;
}

void PmpCoreProfileInfo::enableByComplement()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mEnabledByComplement = true;
}

UInt32 PmpCoreProfileInfo::version() const
{
	return mLibInfo.version;
}

}

}
