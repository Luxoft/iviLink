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


#ifndef PUBLIC_ERROR_HPP
#define PUBLIC_ERROR_HPP

#include <string>
#include <sstream>

#include "Types.hpp"

namespace iviLink
{

// Don't forget to update Java enum otherwise serialization will fail
enum ErrorCode
{
    IVILINK_NO_ERROR = 0,
    UNKNOWN_ERROR,
    INITIALIZATION_ERROR,
    SERVICE_NOT_FOUND,
    SERVICE_ALREADY_LOADED,
    XML_PARSING_FAILED,
    CALLBACKS_NOT_REGISTERED,
    NO_PROFILES_FOUND,
    IPC_REQUEST_TIMEOUT,
    IPC_REQUEST_FAILED,
    PIM_ERROR, // for now: waiting for PMAL/PIM refactoring
    CHANNEL_NOT_FOUND,
    CHANNEL_ALLOCATION_WRONG_PRIORITY,
    CHANNEL_ALLOCATION_FATAL_ERROR,
    CHANNEL_DEALLOCATION_ERROR,
    SEND_TO_NOT_ALLOCATED_CHANNEL,
};

class Error
{
public:
    /**
     * @param err error code
     * @param message string, describing the error
     * @param recoverable true if retries should be made
     */
    Error(const ErrorCode err, const std::string & message, const bool recoverable)
    {
        mErrorCode = err;
        mMessage = message;
        mIsRecoverable = recoverable;
    }

    static Error NoError()
    {
        return Error(IVILINK_NO_ERROR, "Everything is ok", true);
    }

    virtual operator std::string() const
    {
        return toString();
    }

    /**
     *  Converts the error to human-readable format.
     */
    std::string toString() const
    {
        std::stringstream ss;
        ss << "Error code: " << mErrorCode << "; message: " << mMessage ;
        ss << " Can be fixed by reattempting? " << mIsRecoverable;
        return ss.str();
    }

    std::string serializeForJNI() const
    {
        std::stringstream ss;
        ss <<mErrorCode << ":" << static_cast<int>(mIsRecoverable) << ":" << mMessage;
        return ss.str();
    }

    ErrorCode getCode()
    {
        return mErrorCode;
    }

    bool isNoError()
    {
        return mErrorCode == IVILINK_NO_ERROR;
    }

    /**
     * @return true if this error can be fixed with retrying
     */
    bool isRecoverable()
    {
        return mIsRecoverable;
    }

    std::string getMessage()
    {
        return mMessage;
    }

    void setErrorCode(ErrorCode newCode)
    {
        mErrorCode = newCode;
    }

    virtual ~Error()
    {
    }

private:
    ErrorCode mErrorCode;
    std::string mMessage;
    bool mIsRecoverable;
};
}

#endif //PUBLIC_ERROR_HPP
