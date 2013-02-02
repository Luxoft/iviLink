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


#ifndef APPLIB_TEST_PROFILE_HPP
#define APPLIB_TEST_PROFILE_HPP

#include "Logger.hpp"
//profile library header, must be included
#include "ProfileLibCommon.hpp"
//abstract profile API class header, must be included
#include "IAppLibTestProfileAPI.hpp"

//namespace required for reading data was sending by other side
using iviLink::Buffer;
//namespace required for working with channels
using iviLink::Channel::tChannelId;

IVILINK_PROFILE_BEGIN

// AppLibTestProfile class inherits IAppLibTestProfile_API and CChannelHandler classes
class AppLibTestProfile: public IAppLibTestProfile_API
//the following class provides access to allocating and deallocatiing channels
,
        public iviLink::Channel::ChannelHandler

{

    //Profile declaration: name, version, UID, API UID
    IVILINK_PROFILE_DECLARE
    IVILINK_PROFILE_NAME(AppLibTestProfileImpl) IVILINK_PROFILE_VERSION(1)
    IVILINK_PROFILE_UID(AppLibTestProfileImpl_UID)
    IVILINK_PROFILE_API_UID(AppLibTest_PAPI_UID)

    AppLibTestProfile(iviLink::Profile::IProfileCallbackProxy* pCbProxy);
    virtual ~AppLibTestProfile();

    // Profile domain section

    virtual void sendData(std::string data);

    /**
     * Callback that should be invoked when the data is received
     * @param channel - channel ID
     * @param buffer - buffer with data
     * @return none
     */
    virtual void onBufferReceived(const tChannelId channel, Buffer const& buffer);

    /**
     * Callback that should be invoked when the channel is deleted
     * @param channel_id - channel ID
     * @return none
     */
    virtual void onChannelDeleted(const UInt32 channel_id);

    /**
     * Callback that should be invoked when the connection is lost
     * @return none
     */
    virtual void onConnectionLost();

    /**
     * Allocating channel
     * @return none
     */
    virtual void onEnable();

    /**
     * Deallocating channel
     * @return none
     */
    virtual void onDisable();

    //Channel ID
    tChannelId mChannelId;
    std::string mChannelName;
    IAppLibTestProfile_API::Callbacks* mApplicationCallbacks;  //Pointer to instance of class describing callbacks

    static Logger mLogger;

};

IVILINK_PROFILE_END(AppLibTestProfile)

#endif // APPLIB_TEST_PROFILE_HPP
