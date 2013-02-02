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
#include "ProfileInfo.hpp"

namespace iviLink
{
namespace PMP
{

Logger CProfileInfo::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Repository"));

CProfileInfo::CProfileInfo(std::string xmlPath)
    : mXmlPath(xmlPath)
    , mFailed(false)
{
    LOG4CPLUS_INFO(msLogger, "XML Path: " + mXmlPath);
    loadProfileInfo();
}

CProfileInfo::CProfileInfo()
{
}

CProfileInfo::~CProfileInfo()
{
}

std::string CProfileInfo::xmlPath() const
{
    return mXmlPath;
}

Profile::Uid CProfileInfo::uid() const
{
    return mUid;
}

std::string CProfileInfo::name() const
{
    return mName;
}

int CProfileInfo::version() const
{
    return mVersion;
}

Profile::ApiUid CProfileInfo::apiUid() const
{
    return mApiUid;
}

const std::map<std::string,std::string> &CProfileInfo::attributes() const
{
    return mAttributes;
}

const std::map<std::string,std::string> &CProfileInfo::libs() const
{
    return mLibs;
}

std::string CProfileInfo::lib(std::string platform)
{
    std::map<std::string,std::string>::iterator lit = mLibs.find(platform);
    if(mLibs.end() == lit)
    {
        return "";
    }
    return mLibs[platform];
}

bool CProfileInfo::addLib(std::string platform, std::string libPath)
{
    std::map<std::string,std::string>::iterator lit = mLibs.find(platform);
    if (mLibs.end() != lit)
    {
        return false;
    }
    mLibs[platform] = libPath;
    return true;
}

bool CProfileInfo::removeLib(std::string platform)
{
    std::map<std::string,std::string>::iterator lit = mLibs.find(platform);
    if (mLibs.end() == lit)
    {
        return false;
    }
    mLibs.erase(lit);
    return true;
}

void CProfileInfo::loadProfileInfo()
{
    pugi::xml_document doc;
    pugi::xml_parse_result res = doc.load_file(mXmlPath.c_str());
    if (pugi::status_ok != res.status)
    {
        mFailed = true;
        LOG4CPLUS_ERROR(msLogger, "Error while parsing Profile manifest");
        return;
    }

    pugi::xml_node prof = doc.child("profile");
    mUid = Profile::Uid(prof.child_value("uid"));
    mName = prof.child_value("name");
    mVersion = atoi(prof.child_value("version"));
    mApiUid = Profile::Uid(prof.child("api").attribute("uid").value());
    pugi::xml_node attrs = prof.child("attributes");

    for (pugi::xml_node_iterator it = attrs.begin(); it != attrs.end(); ++it)
    {
        if (it->name() == std::string("attribute"))
        {
            std::string atr = it->attribute("name").value();
            if (atr != "")
            {
                mAttributes[atr] = it->attribute("value").value();
            }
        }
    }
}

void CProfileInfo::print() const
{
    std::string print = "*** \t Profile\n";
    print += " UID: " + mUid.value();
    print += "\n Profile Name : " + mName;
    print += "\n Version: " + convertIntegerToString(mVersion);
    print += "\n API UID: " + mApiUid.value();
    print += "\n XML Path: " + mXmlPath;
    print += "\n  Attributes:\n";
    for (std::map<std::string,std::string>::const_iterator it = mAttributes.begin(); mAttributes.end() != it; ++it)
    {
        print +=  "\t" + it->first + " : " + it->second;
    }
    print += "\n  DLLs\n";
    for (std::map<std::string,std::string>::const_iterator it = mLibs.begin(); mLibs.end() != it; ++it)
    {
        print += "\t" + it->first + " : " + it->second + "\n";
    }
    LOG4CPLUS_INFO(msLogger, print);
}

bool CProfileInfo::failed() const
{
    return mFailed;
}

}
}
