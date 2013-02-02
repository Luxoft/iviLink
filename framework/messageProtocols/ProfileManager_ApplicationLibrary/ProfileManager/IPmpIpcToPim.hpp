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


#ifndef IPMPIPCTOPIM_HPP_
#define IPMPIPCTOPIM_HPP_

#include "BaseError.hpp"
#include "UID.hpp"
#include "ipc_common.hpp"

namespace iviLink
{
namespace PMP
{

class IPmpIpcToPim
{
public:

    /**
     * @todo description
     *
     * @param profileUid
     * @param piuid
     * @param serviceUID
     * @param dirId id of direction of connection
     *
     * @return true if IPC request success
     */
    virtual bool incomingProfile(iviLink::BaseUid const& profileUid,
            iviLink::Profile::ApiUid const& papiUid,
            iviLink::Profile::IUid const& piuid,
            iviLink::Service::Uid const& serviceUID,
            iviLink::Ipc::DirectionID const& dirId) = 0;

    /**
     * @todo description
     *
     * @param piuid
     * @param dirId id of direction of connection
     *
     * @return true if IPC request success
     */
    virtual bool profileDied(iviLink::Profile::IUid const& piuid,
            iviLink::Ipc::DirectionID const& dirId) = 0;


    virtual bool generatePIUidResponse(const iviLink::Profile::IUid& piuid) = 0;

protected:

    virtual ~IPmpIpcToPim()
    {
    }
};

}  // namespace ProfileManager
}  // namespace iviLink

#endif /* IPMPIPCTOPIM_HPP_ */
