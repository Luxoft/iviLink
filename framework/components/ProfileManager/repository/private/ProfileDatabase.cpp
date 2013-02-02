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
#include <cstdio>
#include <fstream>
#include <sstream>

#include "ProfileDatabase.hpp"
#include "pugixml.hpp"

namespace iviLink
{
namespace PMP
{

Logger ProfileDatabase::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Repository"));

ProfileDatabase::ProfileDatabase(const std::string & rootFolderPath, const std::string & relativeFolderPath, const std::string & dbName)
    : mFolderPath(rootFolderPath + relativeFolderPath)
    , mRootFolderPath(rootFolderPath)
    , mDbName(dbName)
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
        LOG4CPLUS_ERROR(msLogger, "Profile API Database parsing error");
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
        CProfileInfo info(mFolderPath + std::string(it->child_value("manifest-path")));
        if (info.failed())
        {
            LOG4CPLUS_WARN(msLogger, "Failed loading of XML Manifest");
            continue;
        }
        ProfileInfoMap::iterator mit = mProfiles.find(info.uid());
        if (mProfiles.end() == mit)
        {
            mProfiles[info.uid()] = info;
            pugi::xml_node libs = it->child("libs");
            for (pugi::xml_node_iterator sit = libs.begin(); sit != libs.end(); ++sit)
            {
                std::string pathToLib = mRootFolderPath + (std::string)sit->attribute("path").value();
                mProfiles[info.uid()].addLib(sit->attribute("platform").value(), pathToLib);
            }
        }
        else if ( 0 != it->child_value("uid"))
        {
            LOG4CPLUS_WARN(msLogger, "Error: UID repetition: " + std::string(it->child_value("uid")));
        }
        else
        {
            LOG4CPLUS_WARN(msLogger, "Error: UID repetition");
        }
    }
    return true;
}

bool ProfileDatabase::saveChanges()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    pugi::xml_document doc;
    pugi::xml_node profiles = doc.append_child("profiles");
    for (ProfileInfoMap::const_iterator it = mProfiles.begin(); it != mProfiles.end(); ++it)
    {
        pugi::xml_node profile = profiles.append_child("profile");
        pugi::xml_node maniPath = profile.append_child("manifest-path");
        maniPath.append_child(pugi::node_pcdata).set_value(it->second.xmlPath().c_str());
        pugi::xml_node libsNode = profile.append_child("libs");
        for (std::map<std::string,std::string>::const_iterator sit = it->second.libs().begin(); sit != it->second.libs().end(); ++sit)
        {
            pugi::xml_node libNode = libsNode.append_child("lib");
            libNode.append_attribute("platform").set_value(sit->first.c_str());
            libNode.append_attribute("path").set_value(sit->second.c_str());
        }
    }
    std::string path = mFolderPath + mDbName;
    bool result = doc.save_file(path.c_str());
    if (result)
    {
        mDbState = EDB_SAVE_ERROR;
    }
    printDB();
    return result;
}

BaseError ProfileDatabase::addProfile(const std::string xmlManifestPath)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    CProfileInfo info(xmlManifestPath);
    ProfileInfoMap::iterator mit = mProfiles.find(info.uid());
    if (mProfiles.end() != mit)
    {
        LOG4CPLUS_WARN(msLogger, "Error: UID repetition");
        return RepositoryError(RepositoryError::ERROR_UID_ALREADY_EXISTS);
    }
    else
    {
        mProfiles[info.uid()] = info;
    }
    if (saveChanges())
    {
        return RepositoryError::NoRepositoryError();
    }
    else
    {
        return RepositoryError(RepositoryError::ERROR_DATABASE_WRITE);
    }
}

BaseError ProfileDatabase::removeProfile(Profile::Uid profileUid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    ProfileInfoMap::iterator mit = mProfiles.find(profileUid);

    if (mProfiles.end() == mit)
    {
        return RepositoryError(RepositoryError::ERROR_UNKNOWN_UID);
    }
    else
    {
        for (std::map<std::string,std::string>::const_iterator it = mit->second.libs().begin(); it != mit->second.libs().end(); ++it)
        {
            mit->second.removeLib(it->first);
        }
        mProfiles.erase(mit);
    }

    if (saveChanges())
    {
        return RepositoryError::NoRepositoryError();
    }
    else
    {
        return RepositoryError(RepositoryError::ERROR_DATABASE_WRITE);
    }
}

BaseError ProfileDatabase::addProfileImplementation(Profile::Uid profileID, const LibDescriptor& library)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    ProfileInfoMap::iterator mit = mProfiles.find(profileID);
    if (mProfiles.end() == mit)
    {
        return RepositoryError(RepositoryError::ERROR_UNKNOWN_UID);
    }
    else
    {
        mit->second.addLib(library.platform,library.libPath);
    }
    if (saveChanges())
    {
        return RepositoryError::NoRepositoryError();
    }
    else
    {
        return RepositoryError(RepositoryError::ERROR_DATABASE_WRITE);
    }
}

BaseError ProfileDatabase::removeProfileImplementation(Profile::Uid profileUid, std::string platform)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    ProfileInfoMap::iterator mit = mProfiles.find(profileUid);

    if (mProfiles.end() == mit)
    {
        return RepositoryError(RepositoryError::ERROR_UNKNOWN_UID);
    }
    bool res = mit->second.removeLib(platform);

    if (!res)
    {
        return RepositoryError(RepositoryError::ERROR_NO_DLL);
    }

    if (saveChanges())
    {
        return RepositoryError::NoRepositoryError();
    }
    else
    {
        return RepositoryError(RepositoryError::ERROR_DATABASE_WRITE);
    }
}

BaseError ProfileDatabase::removeProfileImplementation(Profile::Uid profileUid, LibDescriptor ld)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    return removeProfileImplementation(profileUid,ld.platform);
}

std::list<LibInfo> ProfileDatabase::findProfiles(Profile::Uid id, const std::map<std::string, std::string> & profileArguments, std::string platform)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    LOG4CPLUS_INFO(msLogger, "- ProfileDatabase::findProfiles() Find UID : " + id.value());
    std::list<LibInfo> result;
    for (ProfileInfoMap::iterator pit = mProfiles.begin(); mProfiles.end() != pit; ++pit)
    {
        CProfileInfo &info = pit->second;
        LOG4CPLUS_INFO(msLogger, "-- UID: "+ info.uid().value() + ", API UID : " + info.apiUid().value());
        if ((info.uid() == id || info.apiUid() == id)/* && (platform == "" || "" != info.lib(platform))*/)
        {
            int rel = getRelevance(info,profileArguments);
            if (!rel)
            {
                continue;
            }
            if ("" == platform)
            {
                for (std::map<std::string,std::string>::const_iterator libIt = info.libs().begin(); info.libs().end() != libIt; ++libIt)
                {
                    LibInfo libInfo(info.uid(),libIt->first, libIt->second,rel);
                    result.push_back(libInfo);
                }
            }
            else
            {
                std::string rPath = info.lib(platform);
                if ("" != rPath)
                {
                    LibInfo libInfo(info.uid(),platform,rPath);
                    result.push_back(libInfo);
                }
            }
        }
    }
    /// todo: sort results by relevance (add operator < to to LibInfo structure) (after first AXIS implementation, this task is on future)
    return result;
}

int ProfileDatabase::getRelevance(const CProfileInfo & proInfo, const std::map<std::string, std::string> & profileArguments)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    for (std::map<std::string, std::string>::const_iterator it = profileArguments.begin(); profileArguments.end() != it; ++it)
    {
        std::map<std::string,std::string>::const_iterator fit = proInfo.attributes().find(it->first);
        if (fit == proInfo.attributes().end() || fit->second != it->second)
        {
            LOG4CPLUS_INFO(msLogger, "returned 0");
            return 0;
        }
    }
    LOG4CPLUS_INFO(msLogger, "returned 100");
    return 100;
}

std::string ProfileDatabase::getManifest(Profile::Uid uid)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    ProfileInfoMap::iterator mit = mProfiles.find(uid);
    if (mProfiles.end() == mit)
    {
        return std::string("");
    }
    std::stringstream txt;
    txt << std::ifstream(mit->second.xmlPath().c_str()).rdbuf();
    return txt.str();
}

std::list<Profile::Uid> ProfileDatabase::getProfilesList()
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__ );
    std::list<Profile::Uid> list;
    for (ProfileInfoMap::const_iterator it = mProfiles.begin(); mProfiles.end() != it; ++it)
    {
        list.push_back(it->first);
    }
    return list;
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

}
}
