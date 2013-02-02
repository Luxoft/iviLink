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


#ifndef PROFILE_HOLDER_HPP__
#define PROFILE_HOLDER_HPP__

#include <map>
#include <string>
#include <set>
#include <pthread.h>

#include "UID.hpp"

#include "ProfileInternal.hpp"

namespace iviLink
{
namespace Profile
{

class ProfileHolder : public ProfileInternal
{
public:

    ProfileHolder(IUid const& id, Service::Uid const& sid, BaseProfileApi * profileInstance)
        : ProfileInternal(id, profileInstance)
        , mSid(sid)
    {
    }

    Service::Uid const& getServiceUID() const
    {
        return mSid;
    }

    virtual BaseProfileApi * profileInstance()
    {
        mLockSet.insert(pthread_self());
        return ProfileInternal::profileInstance();
    }

    void release()
    {
        mLockSet.erase(pthread_self());
    }

    bool isLocked() const
    {
        return !mLockSet.empty();
    }

protected:

    virtual ~ProfileHolder()
    {
    }

private:

    Service::Uid const  mSid;
    std::set<pthread_t> mLockSet;

};

}
}


#endif // IPROFILE_HPP__
