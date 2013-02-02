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


#include "ServiceInfo.hpp"
#include "Exit.hpp"
#include "pugixml.hpp"

namespace iviLink
{
namespace PMP
{

ServiceInfo::ServiceInfo(std::string & xmlPath, UInt32 priority)
    : mLogger(Logger::getInstance(LOG4CPLUS_TEXT("profileManager.Repository")))
    , mPriority(priority)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "Service manifest path: " + xmlPath);

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(xmlPath.c_str());
    if (result)
    {
        mUid = doc.child("service").attribute("name").value();
        mPairUid = doc.child("service").child("pair").attribute("name").value();

        for (pugi::xml_node profile_node = doc.child("service").child("profile"); profile_node;
                profile_node = profile_node.next_sibling("profile"))
        {
            Profile::ApiUid apiUid = Profile::ApiUid(profile_node.attribute("api").value());
            if (Profile::ApiUid("") == apiUid)
            {
                LOG4CPLUS_FATAL(mLogger, "Could not parse xml: " + xmlPath
                   + ". Malformed xml: no value for node api");
                killProcess(1);
            }
            mProfileApis[apiUid];

            for (pugi::xml_node param_node = profile_node.child("param"); param_node; param_node =
                    param_node.next_sibling("param"))
            {
                std::string paramName(param_node.attribute("name").value());
                std::string paramVal(param_node.attribute("lowbound").value());
                paramVal+=";" + (std::string)param_node.attribute("upbound").value() + ";"
                        + (std::string)param_node.attribute("optimal").value();
                mProfileApis[apiUid][paramName] = paramVal;
            }
        }
    }
    else
    {
        LOG4CPLUS_FATAL(mLogger, "Service manifest load error");
        killProcess(1);
    }
}

ServiceInfo::~ServiceInfo()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
}

const Service::Uid & ServiceInfo::uid () const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    return mUid;
}

const Service::Uid & ServiceInfo::pairUid () const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    return mPairUid;
}

void ServiceInfo::getProfileApis(std::set<Profile::ApiUid> & profileApis) const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    profileApis.clear();
    for (ProfileApiMap::const_iterator it = mProfileApis.begin();
            mProfileApis.end() != it; ++it)
    {
        profileApis.insert(it->first);
    }
}

UInt32 ServiceInfo::priority() const
{
    return mPriority;
}

bool ServiceInfo::getProfileParameters(const Profile::ApiUid & profileApi,
        std::map<std::string, std::string> paramatersMap) const
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__ );
    paramatersMap.clear();
    ProfileApiMap::const_iterator it = mProfileApis.find(profileApi);
    if (mProfileApis.end() == it)
    {
        LOG4CPLUS_WARN(mLogger, "Profile API '" + profileApi.value() + "'");
        return false;
    }

    paramatersMap = it->second;
    return true;
}

void ServiceInfo::print() const
{
    std::string str = "ServiceUid: '" + uid().value() + "', " +
            "PairUid: '" + pairUid().value();

    for (ProfileApiMap::const_iterator it = mProfileApis.begin();
            mProfileApis.end() != it; ++it)
    {
        str += "\n\t Profile :: ApiUid: " + it->first.value();
    }

    LOG4CPLUS_INFO(mLogger,  str);
}

} /* namespace PMP */
} /* namespace iviLink */
