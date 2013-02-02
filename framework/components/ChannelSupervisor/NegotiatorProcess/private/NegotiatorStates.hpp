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


#ifndef NEGOTIATORSTATES_HPP_
#define NEGOTIATORSTATES_HPP_

#include <map>
#include <iostream>
#include <string>

#include "ErrorCode.hpp"
#include "CRWMutex.hpp"
#include "CMutex.hpp"
#include "CCondVar.hpp"
#include "CSError.hpp"
#include "Notification.hpp"
#include "Logger.hpp"
#include "CSignalSemaphore.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{

class NegotiatorStates
{

public:
    friend class NegotiatorIPCHandler;

    enum ChannelState
    {
        IDLE, 
        NEGOTIATED, 
        CHECKEDINMAP, 
        CAALLOCATED,
    };
    typedef std::map<std::string, std::pair<ChannelState, Messages::Notification*> > tStateMap;

    static std::string stateToString(ChannelState state);

    void processTubeNotification(UInt8* rawNotification);

    /**
     * Waits for the specified tag to reach the specified state. Waits no 
     * more than OTHER_SIDE_RESP_TIMEOUT_MS milliseconds. Used by the NegotiatorIPCHandler. 
     * @param tag the tag that should be in the specified state after wait
     * @param stateToWait 
     * @param channelID channel id from the remote notification (0 in case of an error)
     * @param remoteError error code from the remote notification (ERRORCODE_OTHER in case of an error)
     * @return NoError in case the wait was successful, error with the ERROR_TIMEOUT code if the wait has timed out
     */
    BaseError waitTagState(const std::string& tag, const NegotiatorStates::ChannelState stateToWait, 
                                        UInt32& channelID, Messages::ErrorCode& remoteError);
private:
    /**
     * Parse responseStr into pugi::xml_document doc
     */
    void parseResponse(pugi::xml_document* doc, const char* responseStr);

    struct WaitData
    {    
        std::string mTag;
        ChannelState mState;
        Messages::ErrorCode mRemoteError;
        UInt32 mChannelID;
        CSignalSemaphore * mWaitSema; // is signaled when the state for mTag is equal to mState
        WaitData()
        {
            mTag = std::string("");
            mState = IDLE;
            mRemoteError = Messages::ERRORCODE_OTHER;
            mChannelID = 0;
            mWaitSema = new CSignalSemaphore();
        }

        ~WaitData()
        {
            delete mWaitSema;
        }
    };
    typedef std::map<std::string, WaitData*> tWaitMap;

    /**
     * Obtains WaitData for the specified tag-state combination from the mWaitMap (if there was no
     * data in the map, then new data is created, initialized with tag and state,
     * stored in the map and returned as the result).
     */
    WaitData * getDataForTagState(const std::string& tag, const NegotiatorStates::ChannelState& state);

    /**
     * Deletes and erases WaitData for the specified tag-state combination from the mWaitMap if there is any.
     */
    void eraseDataForTagState(const std::string& tag, const NegotiatorStates::ChannelState& state);

    /**
     * Generates key for mWaitMap based on state and tag. For two identical combinations of
     * state and tag the results will be the same.
     */
    std::string genWaitMapKey(const std::string& tag, const NegotiatorStates::ChannelState& state);

    /**
     * When a Notification comes from the NegotiatorTube, WaitData from mWaitMap for the corresponding
     * tag-state combination is initialized with the notification's error code and channel ID
     * and the semaphore is signaled. 
     */
    void signalTagState(const std::string& tag, const NegotiatorStates::ChannelState& state,
                        const UInt32 channelID, const Messages::ErrorCode remoteError);

    Messages::ErrorCode getErrorCodeFromNotification(Messages::Notification * noti);

    static Logger mLogger;

    ChannelState getTagState(const std::string& tag);

    CRWMutex mMapMutex;
    tStateMap mStateMap;
    tWaitMap mWaitMap;
    CMutex mWaitMapMutex;
};

}  // namespace ChannelSupervisor
}  // namespace iviLink

#endif /* NEGOTIATORSTATES_HPP_ */
