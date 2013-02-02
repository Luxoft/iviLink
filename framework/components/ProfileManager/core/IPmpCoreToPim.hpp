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


#ifndef IPMPCORETOPIM_HPP_
#define IPMPCORETOPIM_HPP_


#include "PmpError.hpp"
#include "UID.hpp"


namespace iviLink
{
namespace PMP
{


class IPmpCoreToPim
{
public:

    /**
    * Gets uid of complementary profile for given profile
    * 
    * @param profUid uid of profile for which complemetary will be get
    * @param[out] complProfUid uid of complemetary profile
    *
    * @retval true complementary profile found and returned in complProfUid
    * @retval false unable to find complementary profile
    */
    virtual bool getComplementary(Profile::Uid const& profUid,
            Profile::Uid& complProfUid) const = 0;

    /**
    * Obtains the Profile API uid for given profile.
    *
    * @param profUid is uid of profile for which ProfileAPI uid will be get
    * @param[out] papiUid is Profile API uid
    *
    * @retval true ProfileAPI uid is found and returned in @c papiUid
    * @retval false unable to find ProfileAPI uid
    */
    virtual bool getPapiUid(iviLink::Profile::Uid const& profUid,
            iviLink::Profile::ApiUid& papiUid) const = 0;

protected:

    virtual ~IPmpCoreToPim()
    {
    }

};


} // namespace PMP
} // namespace iviLink


#endif // IPMPCORETOPIM_HPP_
