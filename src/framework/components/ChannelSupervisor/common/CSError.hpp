/* 
 * iviLINK SDK, version 1.1.19
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 

#ifndef CSERROR_HPP_
#define CSERROR_HPP_

#include "CError.hpp"

const char moduleId[] = "ChannelSupervisor";

namespace iviLink
{
namespace ChannelSupervisor
{

/**
 * Simplifies error construction in IPC library.
 * Class contains error codes for errors of CIpc and CIpcSocket classes.
 */
class CSError: public CError
{
public:
    enum eCodes
    {
        ERROR_OTHER = 1,
        ERROR_TIMEOUT,
        IPC_NO_DATA_ERROR,
        NEGOTIATION_CHANNEL_ERROR,
        NEGOTIATION_CHANNEL_TIMEOUT,
        NO_FREE_CID_IN_MAP,
        UPDATE_MAP_TIMEOUT,
        UPDATE_MAP_ERROR,
        DEALLOCATION_CHANNEL_MAP_TIMEOUT,
        DEALLOCATION_CHANNEL_MAP_ERROR,
        DEALLOCATION_CHANNEL_MAP_WRONG_CID,
        UPDATE_MAP_WRONG_CID
    };

    explicit CSError(eCodes code, const char* pDescription = "", eSeverity severity = ERROR)
            : CError((ErrorCode) code, moduleId, severity, pDescription)
    {
    }

    explicit CSError(eCodes code, std::string const& description, eSeverity severity = ERROR)
            : CError((ErrorCode) code, moduleId, severity, description.c_str())
    {
    }

    CSError(CSError const& ref)
            : CError(ref)
    {
    }

    virtual ~CSError()
    {
    }

    /**
     * Constructs NO_ERROR message
     * @param pDescription message description
     * @return instance of CError class with NO_ERROR state
     */
    static CError NoCSError(const char* pDescription = "")
    {
        return CError::NoError(moduleId, pDescription);
    }
};

}  // namespace ChannelSupervisor
}  // namespace AXIS

#endif /* CSERROR_HPP_ */
