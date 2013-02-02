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


#ifndef CSEATCLIENTPROFILE_HPP_
#define CSEATCLIENTPROFILE_HPP_

#include <queue>

#include "Logger.hpp"
#include "ISeatProfiles.hpp"
#include "ISender.hpp"
#include "CSenderThread.hpp"
#include "CMutex.hpp"
#include "CSignalSemaphore.hpp"

#include "ProfileLibCommon.hpp"

using iviLink::Buffer;
using iviLink::Channel::tChannelId;

class CSenderThread;
class CMutex;
class CSignalSemaphore;

IVILINK_PROFILE_BEGIN

class CSeatSenderProfile   : public iviLink::Channel::ChannelHandler
                           , public ISeatSenderProfile
                           , public ISender
{
    IVILINK_PROFILE_DECLARE
    IVILINK_PROFILE_NAME(SeatSenderProfile)
    IVILINK_PROFILE_VERSION(1)
    IVILINK_PROFILE_UID(SeatSenderProfile_UID)
    IVILINK_PROFILE_API_UID(SeatSenderProfile_PAPI_UID)

    // from ISeatSenderProfile
    virtual void send_state(const ISeatSenderProfile::state_t& state);

    virtual void onEnable();
    virtual void onDisable();

    //from CChannelHandler
    virtual void onBufferReceived(const iviLink::Channel::tChannelId channel, iviLink::Buffer const& buffer);
    virtual void onChannelDeleted(const UInt32 channel_id);
    virtual void onConnectionLost();

    //from ISender
    virtual void senderLoop();

    bool hasRequests();
    void handleRequest();

    // Pointer to proxy used as a bool flag, indicates the "master" state
    explicit CSeatSenderProfile(iviLink::Profile::IProfileCallbackProxy*);
    virtual ~CSeatSenderProfile();

    typedef std::string buffer_t;
    typedef std::queue<buffer_t>                 tReqQueue;

    iviLink::Channel::tChannelId                 mChannelID; 

    profile_callback_t                          *mpAppCallbacks;
    CSenderThread                                mSenderThread;
    tReqQueue                                    mReqQueue;
    CMutex                                       mReqMutex;
    CSignalSemaphore                             mReqSemaphore;
    bool                                         mBe;
    static Logger                                msLogger;
};


#endif /* CSEATCLIENTPROFILE_HPP_ */
