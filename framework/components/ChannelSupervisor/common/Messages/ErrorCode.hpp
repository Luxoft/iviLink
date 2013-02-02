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


#ifndef ERRORCODE_HPP
#define ERRORCODE_HPP

namespace iviLink
{
namespace ChannelSupervisor
{
namespace Messages
{

enum ErrorCode
{
    ERRORCODE_SUCCESS = 0,  //operation successful
    ERRORCODE_CHANNELID_ALREADY_OCCUPIED,        //channel id was or is being used
    ERRORCODE_WRONG_CHANNELID,                   //incorrect channel id
    ERRORCODE_TIMEOUT_OCCURRED,                  //timeout occurred during operation
    ERRORCODE_FREE_CID_NOT_FOUND_IN_MAP,         //free cid was not found in map
    ERRORCODE_UPDATE_CHANNEL_ERROR,
    ERRORCODE_UPDATE_CHANNEL_WRONG_CID,
    ERRORCODE_OTHER,                              //other error occurred
};

}  // Messages
}  // ChannelSupervisor
}  // iviLink
#endif // ERRORCODE_HPP
