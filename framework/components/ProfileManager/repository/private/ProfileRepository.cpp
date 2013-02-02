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


#include <cstddef>
#include <cassert>
#include <unistd.h>

#include "ProfileRepository.hpp"
#include "ProfileApiDatabase.hpp"
#include "ProfileDatabase.hpp"

namespace iviLink
{
namespace PMP
{

Logger ProfileRepository::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Repository"));

ProfileRepository::ProfileRepository(conf::Configurator * configurator)
    : mProfileApiDatabase(NULL)
    , mProfileDatabase(NULL)
    , mConfigurator(configurator)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
#ifndef ANDROID
    char buf[256];
    getcwd(buf,255);
    LOG4CPLUS_INFO(msLogger, "CWD: " + std::string(buf));
    std::string iviLinkRootPath = std::string(buf) + "/../";
    mProfileApiDatabase = new ProfileApiDatabase(iviLinkRootPath + "database/profileAPIs/", "ProfileApiDatabase.xml");
    mProfileDatabase = new ProfileDatabase(iviLinkRootPath, "database/profiles/", "ProfileDatabase.xml");
#else
    const std::string dirPath = mConfigurator->getParam("path_to_database");
    mProfileApiDatabase = new ProfileApiDatabase(dirPath, "ProfileApiDatabase.xml");
    mProfileDatabase = new ProfileDatabase(dirPath, "", "ProfileDatabase.xml");
#endif //ANDROID
    assert(mProfileApiDatabase);
    assert(mProfileDatabase);
}

ProfileRepository::~ProfileRepository()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    delete mProfileDatabase;
    delete mProfileApiDatabase;
}

BaseError ProfileRepository::addProfileApi(std::string manifestPath)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mProfileApiDatabase);
    return mProfileApiDatabase->addProfileApi(manifestPath);
}

BaseError ProfileRepository::removeProfileApi(Profile::ApiUid profileApiUid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mProfileApiDatabase);
    return mProfileApiDatabase->removeProfileApi(profileApiUid);
}

BaseError ProfileRepository::addProfile(const std::string xmlManifestPath)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mProfileDatabase);
    return mProfileDatabase->addProfile(xmlManifestPath);
}

BaseError ProfileRepository::removeProfile(Profile::Uid profileUid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mProfileDatabase);
    return mProfileDatabase->removeProfile(profileUid);
}

BaseError ProfileRepository::addProfileImplementation(Profile::Uid profileID, const LibDescriptor& library)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mProfileDatabase);
    return mProfileDatabase->addProfileImplementation(profileID, library);
}

BaseError ProfileRepository::removeProfileImplementation(Profile::Uid profileUid, std::string platform)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mProfileDatabase);
    return mProfileDatabase->removeProfileImplementation(profileUid, platform);
}

BaseError ProfileRepository::removeProfileImplementation(Profile::Uid profileUid, LibDescriptor ld)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mProfileDatabase);
    return mProfileDatabase->removeProfileImplementation(profileUid, ld);
}

std::list<LibInfo> ProfileRepository::findProfiles(Profile::ApiUid id, const std::map<std::string, std::string> & profileArguments, std::string platform)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mProfileDatabase);
    return mProfileDatabase->findProfiles(id, profileArguments, platform);
}

std::string ProfileRepository::getManifest(Profile::Uid uid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mProfileDatabase);
    return mProfileDatabase->getManifest(uid);
}

std::list<Profile::Uid> ProfileRepository::getProfilesList()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );

    assert(mProfileDatabase);
    return mProfileDatabase->getProfilesList();
}

} /* namespace PMP */
} /* namespace iviLink */
