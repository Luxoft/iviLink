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


#ifndef PMMESSAGE_HPP_
#define PMMESSAGE_HPP_


#include "Types.hpp"


namespace iviLink {
namespace ProfileManager {
namespace Ipc {


/**
 * Messages from PMAL to PMP
 */
enum ePmalPmpMessages
{
   PMAL_PMP_GENERATE_PIUID = 1,//!< PMAL_PMP_GENERATE_PIUID
   PMAL_PMP_RELEASE_PIUID,     //!< PMAL_PMP_RELEASE_PIUID
   PMAL_PMP_CREATE_PROFILE,    //!< PMAL_PMP_CREATE_PROFILE
   PMAL_PMP_PROFILE_DIED,       //!< PMAL_PMP_PROFILE_DIED
   PMAL_PMP_DISABLE_BY_UID,
   PMAL_PMP_ENABLE_BY_UID,
   PMAL_PMP_ENABLE_ALL,
   PMAL_PMP_GET_MANIFEST,
   PMAL_PMP_GET_PROFILE_LIB_PATH,
   PMAL_PMP_FIND_PROFILES,
   PMAL_PMP_READY_TO_SERVE
};

/**
 * Messages from PMP to PMAL
 */
enum ePmpPmalMessages
{
   PMP_PMAL_GENERATE_PIUID = 1,//!< PMP_PMAL_GENERATE_PIUID
   //PMP_PMAL_RELEASE_PIUID,     //!< PMP_PMAL_RELEASE_PIUID
   PMP_PMAL_CREATE_PROFILE,    //!< PMP_PMAL_CREATE_PROFILE
   PMP_PMAL_PROFILE_DIED,      //!< PMP_PMAL_PROFILE_DIED
   PMP_PMAL_INCOMMING_PROFILE, //!< PMP_PMAL_INCOMMING_PROFILE
   PMP_PMAL_DISABLE_BY_UID,
   PMP_PMAL_ENABLE_BY_UID,
   PMP_PMAL_GET_MANIFEST,
   PMP_PMAL_GET_PROFILE_LIB_PATH,
   PMP_PMAL_FIND_PROFILES
};

struct MessageHeader
{
   UInt16 type;
   UInt32 size;
} __attribute__((__packed__));

struct PmMessage
{
   MessageHeader header;
   UInt8 data[0];
} __attribute__((__packed__));

}  // namespace Ipc
}  // namespace ProfileManager
}  // namespace AXIS


#endif /* PMMESSAGE_HPP_ */
