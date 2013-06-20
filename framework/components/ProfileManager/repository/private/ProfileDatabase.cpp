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
 * @file                ProfileDatabase.cpp
 * @ingroup             Profile Manager
 * @author              Plachkov Vyacheslav <vplachkov@luxoft.com>
 * @date                10.01.2013
 *
 * Implements ProfileDatabase class
 */


#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "ProfileDatabase.hpp"
#include "pugixml.hpp"

namespace iviLink
{
namespace PMP
{

Logger ProfileDatabase::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Repository"));

ProfileDatabase::ProfileDatabase(const std::string & rootFolderPath, const std::string & relativeFolderPath, const std::string & dbName)
    : mFolderPath(rootFolderPath + relativeFolderPath)
    , mDbName(dbName)
    , mRootFolderPath(rootFolderPath)
    , mDbState(EDB_NORMAL_OK)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    loadDatabase();
    printDB();
}

ProfileDatabase::~ProfileDatabase()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
}

void ProfileDatabase::loadDatabase()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_INFO(msLogger, "Folder path: " + mFolderPath +
            " DB name: " + mDbName);
    mProfiles.clear();
    pugi::xml_document doc;
    std::string path = mFolderPath + mDbName;
    pugi::xml_parse_result res = doc.load_file(path.c_str());
    switch (res.status)
    {
    case pugi::status_ok:              // No error
        mDbState = EDB_NORMAL_OK;
        break;
    case pugi::status_file_not_found:      // File was not found during load_file()
    case pugi::status_io_error:            // Error reading from file/stream
    case pugi::status_out_of_memory:       // Could not allocate memory
    case pugi::status_internal_error:      // Internal error occurred
        LOG4CPLUS_ERROR(msLogger, "Profile API Database open error");
        mDbState = EDB_OPEN_ERROR;
        break;
    case pugi::status_unrecognized_tag:    // Parser could not determine tag type
    case pugi::status_bad_pi:              // Parsing error occurred while parsing document declaration/processing instruction
    case pugi::status_bad_comment:         // Parsing error occurred while parsing comment
    case pugi::status_bad_cdata:           // Parsing error occurred while parsing CDATA section
    case pugi::status_bad_doctype:         // Parsing error occurred while parsing document type declaration
    case pugi::status_bad_pcdata:          // Parsing error occurred while parsing PCDATA section
    case pugi::status_bad_start_element:   // Parsing error occurred while parsing start element tag
    case pugi::status_bad_attribute:       // Parsing error occurred while parsing element attribute
    case pugi::status_bad_end_element:     // Parsing error occurred while parsing end element tag
    case pugi::status_end_element_mismatch: // There was a mismatch of start-end tags (closing tag had incorrect name, some tag was not closed or there was an excessive closing tag)
        LOG4CPLUS_ERROR(msLogger, "Profile Database parsing error");
        mDbState = EDB_PARSING_ERROR;
        break;
    default:
        assert(0);
        break;
    }
    loadParsedDatabase(doc);
}

bool ProfileDatabase::loadParsedDatabase(const pugi::xml_document &doc)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    if (EDB_NORMAL_OK != mDbState)
    {
        LOG4CPLUS_ERROR(msLogger, "parsing state error");
        return false;
    }
    pugi::xml_node profs = doc.child("profiles");

    for (pugi::xml_node_iterator it = profs.begin(); it != profs.end(); ++it)
    {
        LOG4CPLUS_INFO(msLogger, "Profile adding");
        Profile::Uid profileUid = Profile::Uid(it->child_value("uid"));
        Profile::ApiUid apiUid = Profile::ApiUid(it->child_value("api-uid"));
        std::string name = it->child_value("name");
        Profile::Uid complement = Profile::Uid(it->child_value("complement"));
        if ("" == profileUid.value() || "" == apiUid.value() || "" == complement.value() || "" == name)
        {
            LOG4CPLUS_WARN(msLogger, "Profile adding error\nProfile UID: " + profileUid.value() +
                    ", API UID: " + apiUid.value() + ", Complement UID: " + complement.value() +
                    ", name: " + name);
            continue;
        }
        ProfileInfo profile(profileUid, apiUid, complement, name);

        pugi::xml_node libs = it->child("libs");
        for (pugi::xml_node_iterator sit = libs.begin(); sit != libs.end(); ++sit)
        {
            std::string pathToLib = mRootFolderPath + (std::string)sit->attribute("path").value();
            UInt32 version = atoi(sit->attribute("version").value());
            std::string platform = sit->attribute("platform").value();
            profile.addLib(ProfileLibInfo(version, pathToLib, platform));
        }

        ProfileInfoMap::iterator mit = mProfiles.find(profile.uid());
        if (mProfiles.end() == mit)
        {
            mProfiles.insert(std::make_pair(profile.uid(), profile));
            LOG4CPLUS_INFO(msLogger, "Added profile into profiles map, profile UID: " + profile.uid().value());
        }
        else
        {
            LOG4CPLUS_WARN(msLogger, "Error: UID repetition: " + profile.uid().value());
        }
    }
    return true;
}

int ProfileDatabase::getRelevance(const Profile::Uid & profile, const UInt32 version, const std::map<std::string, std::string> & profileArguments) const
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    ProfileInfoMap::const_iterator it = mProfiles.find(profile);
    if (mProfiles.end() == it)
    {
        return 0;
    }
    const std::vector<ProfileLibInfo> &libs = (*it).second.libs();

    int i=1;
    for (std::vector<ProfileLibInfo>::const_iterator it = libs.begin();
            libs.end() != it; ++it, ++i)
    {
        if (version == it->version())
        {
            return (100 * i) / libs.size(); // relevance works correctly if profile version <= 100
        }
    }
    return 0;
}

void ProfileDatabase::printDB()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_INFO(msLogger, "Folder path: " + mFolderPath + " DB name: " + mDbName);
    LOG4CPLUS_INFO(msLogger, "State: " + convertIntegerToString(static_cast<int>(mDbState)));

    for (ProfileInfoMap::const_iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        it->second.print();
    }
}

void ProfileDatabase::getAllLibs(std::list<LibInfo> & libs)
{
    libs.clear();
    for (ProfileInfoMap::const_iterator it = mProfiles.begin();
            mProfiles.end() != it; ++it)
    {
        const std::vector<ProfileLibInfo> & pLibs = it->second.libs();
        for (std::vector<ProfileLibInfo>::const_iterator pit = pLibs.begin();
                pLibs.end() != pit; ++pit)
        {
            libs.push_back(LibInfo(it->second.uid(), it->second.apiUid(), it->second.complement(),
                    pit->version(), pit->platform(),pit->path()));
        }
    }
}

}
}
