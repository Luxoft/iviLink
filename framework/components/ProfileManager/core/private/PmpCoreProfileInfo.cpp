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

PmpCoreProfileInfo::PmpCoreProfileInfo(const std::string & profileManifest)
    : mEnabledByComplement(false)
    , mEnabledByClient(true)
    , mLocked(false)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    parseXml(profileManifest);
}

PmpCoreProfileInfo::PmpCoreProfileInfo(const PmpCoreProfileInfo &info)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mEnabledByClient = info.mEnabledByClient;
    mEnabledByComplement = info.mEnabledByComplement;
    mLocked = info.mLocked;
    mUid = info.mUid;
    mApi = info.mApi;
    mComplement = info.mComplement;
    mLibrary = info.mLibrary;
}

PmpCoreProfileInfo& PmpCoreProfileInfo::operator = (const PmpCoreProfileInfo &info)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mEnabledByClient = info.mEnabledByClient;
    mEnabledByComplement = info.mEnabledByComplement;
    mLocked = info.mLocked;
    mUid = info.mUid;
    mApi = info.mApi;
    mComplement = info.mComplement;
    mLibrary = info.mLibrary;
    return *this;
}

void PmpCoreProfileInfo::parseXml(const std::string & profileManifest)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    pugi::xml_document doc;
    pugi::xml_parse_result res = doc.load(profileManifest.c_str());
    if(pugi::status_ok != res.status)
    {
        LOG4CPLUS_INFO(msLogger, "Error while parsing Profile API manifest");
        return;
    }

    pugi::xml_node prof = doc.child("profile");
    mUid = Profile::Uid(prof.child_value("uid"));
    mApi = Profile::ApiUid(prof.child("api").attribute("uid").value());
    mComplement = Profile::Uid(prof.child_value("complement"));
}

void PmpCoreProfileInfo::setLibrary(const std::string & library)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    mLibrary = library;
}

PmpCoreProfileInfo::~PmpCoreProfileInfo()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

Profile::Uid PmpCoreProfileInfo::uid() const
{
    return mUid;
}

Profile::ApiUid PmpCoreProfileInfo::api() const
{
    return mApi;
}

Profile::Uid PmpCoreProfileInfo::complement() const
{
    return mComplement;
}

std::string PmpCoreProfileInfo::library() const
{
    LOG4CPLUS_INFO(msLogger, "mLibrary = " + mLibrary);
    return mLibrary;
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

}

}
