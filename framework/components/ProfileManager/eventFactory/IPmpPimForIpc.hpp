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


#ifndef IPMPPIMTOIPC_HPP_
#define IPMPPIMTOIPC_HPP_

#include "ipc_common.hpp"
#include "UID.hpp"

namespace iviLink
{
namespace PMP
{

class IPmpPimForIpc
{
public:

    virtual void generatePIUID(iviLink::Ipc::DirectionID const& dirId) = 0;

    virtual void releasePIUID(iviLink::Profile::IUid const& piuid,
            iviLink::Ipc::DirectionID const& dirId) = 0;

    virtual void createProfile(iviLink::BaseUid const& profileUid,
            iviLink::Profile::IUid const& piuid,
            iviLink::Service::Uid const& sid,
            iviLink::Ipc::DirectionID const& dirId) = 0;

    virtual void profileDied(iviLink::Profile::IUid const& piuid,
            iviLink::Ipc::DirectionID const& dirId) = 0;

    virtual void readyToServe(iviLink::Service::SessionUid const& sesUid,
            iviLink::Ipc::DirectionID const& dirId) = 0;

    virtual void onConnection(iviLink::Ipc::DirectionID const& dirId) = 0;

    virtual void onConnectionLost(iviLink::Ipc::DirectionID const& dirId) = 0;

protected:
    virtual ~IPmpPimForIpc()
    {
    };
};


}  // namespace ProfileManager
}  // namespace AXIS

#endif /* IPMPPIMTOIPC_HPP_ */
