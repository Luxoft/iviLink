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


#ifndef SERVICEINFO_HPP_
#define SERVICEINFO_HPP_

#include <map>
#include <set>

#include "UID.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace PMP
{

class ServiceInfo
{
public:
    typedef std::map<Profile::ApiUid, std::map<std::string, std::string> > ProfileApiMap;

    ServiceInfo(std::string & xmlPath, UInt32 priority);
    ~ServiceInfo();

    const Service::Uid & uid() const;
    const Service::Uid & pairUid() const;
    void getProfileApis(std::set<Profile::ApiUid> & profileApis) const;
    bool getProfileParameters(const Profile::ApiUid & profileApi,
            std::map<std::string, std::string> paramatersMap) const;
    UInt32 priority() const;
    void print() const;

private:

    //not implemented
    ServiceInfo(const ServiceInfo &);
    ServiceInfo & operator = (const ServiceInfo &);

    Logger mLogger;
    Service::Uid mUid;
    Service::Uid mPairUid;
    ProfileApiMap mProfileApis;
    UInt32 mPriority;
};

} /* namespace PMP */
} /* namespace iviLink */
#endif /* SERVICEINFO_HPP_ */
