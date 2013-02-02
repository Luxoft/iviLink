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

 
/**
 * \file BaseProfileApi.hpp
 * This header contains declaration of a base class
 * for all profile API classes.
 */

/**
 @mainpage APPLICATION LIBRARY
 	Application Library (AppLib) provides APIs required for using iviLink functionality in applications.
 This library communicates with Service Manager and Application Manager and provides simple API 
 for application initialization in iviLink system and for iviLink services using.
*/

#ifndef CPROFILEAPIBASE_HPP_
#define CPROFILEAPIBASE_HPP_

#include "UID.hpp"
#include "BaseError.hpp"

/**
 * \def IVILINK_PROFILE_API_UID(PROFILE_API_UID)
 * This macro should be placed in every profile API implementation
 * @param PROFILE_API_UID UID of a profile API
 */

#define IVILINK_PROFILE_API_UID(PROFILE_API_UID) \
    iviLink::Profile::ApiUid getProfileApiUid() const \
    { return iviLink::Profile::ApiUid(#PROFILE_API_UID ); }

namespace iviLink
{
namespace Profile
{

class ProfileInternal;

class IProfileCallbackProxy: public IErrorHandler
{
public:
    virtual ~IProfileCallbackProxy()
    {
    }
};
} // namespace Profile


/**
* \class CProfileApiBase
* A base class for all profile API classes.
*/

class BaseProfileApi
{
    friend class Profile::ProfileInternal;

public:

    /**
     * Get profile name
     * Macro IVILINK_PROFILE_NAME(NAME) should be used
     * in class declaration instead of this method implementation
     * (see framework/libraries/profileLib/defines.hpp
     * and BasicSample for example)
     */
    virtual std::string getName() const = 0;

    /**
     * Get profile version
     * Macro IVILINK_PROFILE_VERSION(VERSION) should be used
     * in class declaration instead of this method implementation
     */
    virtual UInt32 getVersion() const = 0;

    /**
     * Returns Profile UID
     * Macro IVILINK_PROFILE_UID(PROFILE_UID) should be used
     * in class declaration instead of this method implementation
     */
    virtual Profile::Uid getUid() const = 0;

    /**
     * Get profileApi UID
     * Macroacro IVILINK_PROFILE_API_UID(PROFILE_API_UID)
     * in class declaration instead of this method implementation.
     */
    virtual Profile::ApiUid getProfileApiUid() const = 0;

    /**
     * Get latest error.
     * @return in case wasError() returned true getError returns more details about the error
     */
    BaseError getError() const
    {
        return mError;
    }

    /**
     * Called when the profile is loading
     */
    virtual void onEnable()
    {
    }

    /**
     * Called when the profile is unloading
     */
    virtual void onDisable()
    {
    }

    /**
     * Check whether the profile is enabled
     * @return true if enabled, otherwise false
     */
    bool isEnabled() const
    {
        return mEnabled;
    }


protected:

    BaseProfileApi()
        : mEnabled(false)
        , mError(BaseError::NoError("CProfileApiBase", "getError - no error"))
    {
    }

    virtual ~BaseProfileApi()
    {
    }

    void setError(const BaseError & error) const
    {
        mError = error;
    }

private:
    bool mEnabled; ///< this value is set only by CProfileInternal
    mutable BaseError mError; ///< stores latest error
};
}


#endif /* CPROFILEAPIBASE_HPP_ */
