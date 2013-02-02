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


#ifndef CPMPERROR_HPP_
#define CPMPERROR_HPP_


#include "BaseError.hpp"
#include "Types.hpp"

namespace iviLink
{
namespace PMP
{

/**
* Class with PMP errors
*/
class CPmpError: public BaseError
{
public:
    /**
    * Enumeration with codes of exceptions
    */
    enum ErrorCodes
    {
        ERROR_NETWORK           = 1,
        ERROR_FUNCTION_FAILED   = 2
    };

    /**
    * Constructor
    */
    explicit CPmpError(ErrorCodes code, const char* pDescription = "", eSeverity severity = IVILINK_ERROR)
        : BaseError((ErrorCode)code, "ProfileManagerProcess", severity, pDescription)
    {
    }

    /**
    * Constructor
    */
    explicit CPmpError(ErrorCodes code, std::string const& description, eSeverity severity = IVILINK_ERROR)
        : BaseError((ErrorCode)code, "ProfileManagerProcess", severity, description.c_str())
    {
    }

    /**
    * Copy constructor
    */
    CPmpError(CPmpError const& ref)
        : BaseError(ref)
    {
    }

    /**
    * Destructor
    */
    virtual ~CPmpError()
    {
    }

    /**
    * Returns object of CError with no error
    */
    static BaseError NoPmpError(const char* pDescription = "")
    {
        return BaseError::NoError("ProfileManagerProcess", pDescription);
    }
};

}
}


#endif /* CPMPERROR_HPP_ */
