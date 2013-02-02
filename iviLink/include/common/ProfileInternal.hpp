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


#ifndef CPROFILEINTERNAL_HPP__
#define CPROFILEINTERNAL_HPP__

#include <map>
#include <string>

#include "UID.hpp"
#include "BaseError.hpp"
#include "BaseProfileApi.hpp"

namespace iviLink
{

namespace PMAL
{
namespace PIM
{

class CProfileDestroyer;
class CProfileEnabler;

} // namespace PMAL
} // namespace PIM

namespace Profile
{

/// Base profile interface, available to profile-manager
class ProfileInternal
{
public:

    /// Returns profile instance UID
    IUid getProfileID() const
    {
        return mProfileID;
    }

    BaseError getError() const
    {
        return mpInstance->getError();
    }

    bool isEnabled() const
    {
        return mIsEnabled;
    }

    virtual BaseProfileApi * profileInstance()
    {
        return mpInstance;
    }

protected:

    /// This is only class can destroy the profile.
    friend class iviLink::PMAL::PIM::CProfileDestroyer;
    friend class iviLink::PMAL::PIM::CProfileEnabler;

    explicit ProfileInternal(IUid const& id, BaseProfileApi * pProfileInstance)
        : mProfileID(id)
        , mIsEnabled(false)
        , mpInstance(pProfileInstance)
    {
    }

    // Only CProfileManager can destroy profile
    virtual ~ProfileInternal()
    {
        delete mpInstance;
    }

    void setEnabled(bool value)
    {
        if (mIsEnabled != value)
        {
            mIsEnabled = value;
            onEnabledChange();
        }
    }

private:
    void onEnabledChange()
    {
        mpInstance->mEnabled = mIsEnabled;
        if (mIsEnabled)
        {
            mpInstance->onEnable();
        }
        else
        {
            mpInstance->onDisable();
        }
    }

    ProfileInternal(ProfileInternal const&);
    ProfileInternal& operator=(ProfileInternal const&);

    const Profile::IUid mProfileID;
    bool mIsEnabled;
    BaseProfileApi * mpInstance;
};

} // namespace Profile
} // namespace iviLink

#endif // CPROFILEINTERNAL_HPP__
