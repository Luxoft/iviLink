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


#include <map>
#include <iostream>
#include <string>

#include "CSError.hpp"
#include "Common.hpp"
#include "Notification.hpp"
#include "AllocateNotification.hpp"
#include "MapCheckNotification.hpp"
#include "CAAllocationNotification.hpp"
#include "UpdateMapNotification.hpp"
#include "DeallocateNotification.hpp"
#include "UpdateChannelInfoNotification.hpp"
#include "NegotiatorStates.hpp"

#include "Types.hpp"

using namespace std;
using namespace iviLink::ChannelSupervisor;
using namespace iviLink::ChannelSupervisor::Messages;

Logger NegotiatorStates::mLogger = Logger::getInstance(
    LOG4CPLUS_TEXT("NegotiatorProcess.NegotiatorStates"));

void NegotiatorStates::processTubeNotification(UInt8* rawNotification)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    BaseError err = CSError(CSError::ERROR_OTHER);
    pugi::xml_document* doc = new pugi::xml_document();
    parseResponse(doc, (char*) rawNotification);
    //obtain tag
    string tag = Notification::GetNotificationTag(doc);
    LOG4CPLUS_INFO(mLogger, "Remote notification tag: " + tag);
    //get the state
    ChannelState currentState = getTagState(tag);
    LOG4CPLUS_INFO(mLogger, "Current state: " + stateToString(currentState));

    switch (Notification::GetNotificationType(doc))
    {
    case Notification::NOTIFICATIONTTYPE_ALLOCATE:
    {
        LOG4CPLUS_TRACE(mLogger,
                "NegotiatorStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_ALLOCATE");
        mMapMutex.lockWrite();
        tStateMap::iterator it = mStateMap.find(tag);

        if (it != mStateMap.end())
        {
            LOG4CPLUS_INFO(mLogger,
                    "NegotiatorStates::ProcessTubeNotification()=>tag found, removing it");
            Notification * noti = (*it).second.second;
            if (noti)
                delete noti;
            mStateMap.erase(it);
        }

        Notification * noti = new AllocateNotification(doc);
        mStateMap.insert(make_pair(tag, make_pair(NEGOTIATED, noti)));
        signalTagState(tag, NEGOTIATED, noti->GetChannelId(), getErrorCodeFromNotification(noti));
        LOG4CPLUS_INFO(mLogger,
                    "NegotiatorStates::ProcessTubeNotification()=>Go to the NEGOTIATED STATE");
        mMapMutex.unlockWrite();
        break;
    }
    case Notification::NOTIFICATIONTTYPE_CHECK_MAP:
    {
        LOG4CPLUS_INFO(mLogger,
                "NegotiatorStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_CHECK_MAP");
        //change state only if current state is NEGOTIATED
        if (currentState == NEGOTIATED)
        {
            LOG4CPLUS_INFO(mLogger,
                    "NegotiatorStates::ProcessTubeNotification()=>State is NEGOTIATED");
            mMapMutex.lockWrite();
            tStateMap::iterator it = mStateMap.find(tag);
            if (it != mStateMap.end())
            {
                Notification * noti = (*it).second.second;
                if (noti)
                    delete noti;
                LOG4CPLUS_INFO(mLogger,
                        "NegotiatorStates::ProcessTubeNotification()=>Go to the CHECKEDINMAP STATE");
                (*it).second.first = CHECKEDINMAP;
                (*it).second.second = new MapCheckNotification(doc);
                signalTagState(tag, CHECKEDINMAP, (*it).second.second->GetChannelId(), getErrorCodeFromNotification((*it).second.second));
            }
            mMapMutex.unlockWrite();
        }
        break;
    }
    case Notification::NOTIFICATIONTTYPE_CA_ALLOCATE:
    {
        LOG4CPLUS_INFO(mLogger,
                "NegotiatorStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_CA_ALLOCATE");
        if (currentState == CHECKEDINMAP)
        {
            LOG4CPLUS_INFO(mLogger,
                    "NegotiatorStates::ProcessTubeNotification()=>State is CHECKEDINMAP");
            mMapMutex.lockWrite();
            tStateMap::iterator it = mStateMap.find(tag);
            if (it != mStateMap.end())
            {
                Notification * noti = (*it).second.second;
                if (noti)
                    delete noti;
                LOG4CPLUS_INFO(mLogger,
                        "NegotiatorStates::ProcessTubeNotification()=>Go to the CAALLOCATED STATE");
                (*it).second.first = CAALLOCATED;
                (*it).second.second = new CAAllocationNotification(doc);
                signalTagState(tag, CAALLOCATED, (*it).second.second->GetChannelId(), getErrorCodeFromNotification((*it).second.second));
            }
            mMapMutex.unlockWrite();
        }
        break;
    }
    case Notification::NOTIFICATIONTTYPE_MAP_ADD_CID:
    {
        LOG4CPLUS_INFO(mLogger,
                "NegotiatorStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_MAP_ADD_CID");
        //change state only if current state is CAALLOCATED
        if (currentState == CAALLOCATED)
        {
            LOG4CPLUS_INFO(mLogger,
                    "NegotiatorStates::ProcessTubeNotification()=>State is CAALLOCATED");
            mMapMutex.lockWrite();
            tStateMap::iterator it = mStateMap.find(tag);
            if (it != mStateMap.end())
            {
                Notification * noti = (*it).second.second;
                if (noti)
                    delete noti;
                (*it).second.first = IDLE;
                (*it).second.second = new UpdateMapNotification(doc);
                LOG4CPLUS_INFO(mLogger,
                        "NegotiatorStates::ProcessTubeNotification()=>Go to the IDLE STATE");
                signalTagState(tag, IDLE, (*it).second.second->GetChannelId(), getErrorCodeFromNotification((*it).second.second));
            }
            mMapMutex.unlockWrite();
        }
        break;
    }
    case Notification::NOTIFICATIONTTYPE_DEALLOCATE:
    {
        LOG4CPLUS_INFO(mLogger,
                "NegotiatorStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_DEALLOCATE");
        //change state only if current state is IDLE
        if (currentState == IDLE)
        {
            LOG4CPLUS_INFO(mLogger,
                    "NegotiatorStates::ProcessTubeNotification()=>State is IDLE");
            mMapMutex.lockWrite();
            tStateMap::iterator it = mStateMap.find(tag);
            if (it != mStateMap.end())
            {
                Notification * noti = (*it).second.second;
                if (noti)
                    delete noti;
                (*it).second.first = IDLE;
                (*it).second.second = new DeallocateNotification(doc);
                signalTagState(tag, IDLE, (*it).second.second->GetChannelId(), getErrorCodeFromNotification((*it).second.second));
                LOG4CPLUS_INFO(mLogger,
                        "NegotiatorStates::ProcessTubeNotification()=>Go to the IDLE STATE");
            }
            mMapMutex.unlockWrite();
        }
        break;
    }
    case Notification::NOTIFICATIONTTYPE_UPDATE_CHANNEL_INFO:
    {
        LOG4CPLUS_INFO(mLogger,
                "NegotiatorStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_UPDATE_CHANNEL_INFO");
        //change state only if current state is IDLE
        if (currentState == IDLE)
        {
            LOG4CPLUS_INFO(mLogger,
                    "NegotiatorStates::ProcessTubeNotification()=>State is IDLE");
            mMapMutex.lockWrite();
            tStateMap::iterator it = mStateMap.find(tag);
            if (it != mStateMap.end())
            {
                Notification * noti = (*it).second.second;
                if (noti)
                {
                    delete noti;
                }
                LOG4CPLUS_INFO(mLogger,
                        "NegotiatorStates::ProcessTubeNotification()=>Go to the IDLE STATE");
                (*it).second.first = IDLE;
                (*it).second.second = new UpdateChannelInfoNotification(doc);
                signalTagState(tag, IDLE, (*it).second.second->GetChannelId(), getErrorCodeFromNotification((*it).second.second));
            }
            mMapMutex.unlockWrite();
        }
        break;
    }

    default:
    {
        LOG4CPLUS_ERROR(mLogger,
                "NegotiatorStates::ProcessTubeNotification()=>Unknown Notification type!!!");
        break;
    }
    }
}

NegotiatorStates::ChannelState NegotiatorStates::getTagState(const std::string& tag)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, "NegotiatorStates::GetTagStateInternal()=>=>tag = " + tag);
    NegotiatorStates::ChannelState ret = IDLE;
    mMapMutex.lockRead();
    tStateMap::iterator it = mStateMap.find(tag);
    if (it != mStateMap.end())
    {
        ret = ((*it).second).first;
        LOG4CPLUS_INFO(mLogger,
                "NegotiatorStates::GetTagState()=>=>State found " + stateToString(ret));
    } 
    else
    {
        LOG4CPLUS_WARN(mLogger, "NegotiatorStates::GetTagState() => state not found!");
    }
    mMapMutex.unlockRead();
    return ret;
}

void NegotiatorStates::parseResponse(pugi::xml_document* doc, const char* responseStr)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    LOG4CPLUS_INFO(mLogger, "Remote response: " + std::string(responseStr));
    doc->load(responseStr);
}

std::string NegotiatorStates::stateToString(ChannelState state)
{
    switch (state)
    {
    case IDLE:
        return "IDLE";
    case NEGOTIATED:
        return "NEGOTIATED";
    case CHECKEDINMAP:
        return "CHECKEDINMAP";
    case CAALLOCATED:
        return "CAALLOCATED";
    }
    return "";
}


ErrorCode NegotiatorStates::getErrorCodeFromNotification(Notification * noti)
{
    ErrorCode errc;
    const char * errstr;
    noti->GetError(errc, errstr);
    return errc;
}

BaseError NegotiatorStates::waitTagState(const std::string& tag, const NegotiatorStates::ChannelState stateToWait, 
                                        UInt32& channelID, ErrorCode& remoteError)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    WaitData * data = getDataForTagState(tag, stateToWait);
    int waitResult = data->mWaitSema->waitTimeout(OTHER_SIDE_RESP_TIMEOUT_MS);
    if (waitResult == -1)
    {
        LOG4CPLUS_ERROR(mLogger, "Wait for tag " + tag + " for state: " + stateToString(stateToWait) + " has timed out!");
        eraseDataForTagState(tag, stateToWait);
        channelID = 0;
        remoteError = ERRORCODE_OTHER;
        return CSError(CSError::ERROR_TIMEOUT);
    }
    channelID = data->mChannelID;
    remoteError = data->mRemoteError;
    eraseDataForTagState(tag, stateToWait);
    LOG4CPLUS_INFO(mLogger, "Wait for tag " + tag + " for state: " + stateToString(stateToWait) + " successful!");
    return CSError::NoCSError();
}


void NegotiatorStates::signalTagState(const std::string& tag, const NegotiatorStates::ChannelState& state,
                                        const UInt32 channelID, const ErrorCode remoteError)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    std::string complementingTag = Notification::GetComplementingTag(tag);
    WaitData * data = getDataForTagState(complementingTag, state);
    data->mChannelID = channelID;
    data->mRemoteError = remoteError;
    data->mWaitSema->signal();
}

NegotiatorStates::WaitData * NegotiatorStates::getDataForTagState(const std::string& tag, const NegotiatorStates::ChannelState& state)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mWaitMapMutex);
    std::string mapKey = genWaitMapKey(tag, state);
    tWaitMap::iterator iter = mWaitMap.find(mapKey);    
    if (iter != mWaitMap.end())
    {
        LOG4CPLUS_INFO(mLogger, "Found data for key: " + mapKey);
        return (*iter).second;
    }
    else
    {
        LOG4CPLUS_INFO(mLogger, "Not found for key: " + mapKey + ", creating!");
        WaitData * newData = new WaitData();
        newData->mTag = tag;
        newData->mState = state;
        mWaitMap[mapKey] = newData;
        return newData;
    }
}

void NegotiatorStates::eraseDataForTagState(const std::string& tag, const NegotiatorStates::ChannelState& state)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    MutexLocker lock(mWaitMapMutex);
    std::string mapKey = genWaitMapKey(tag, state);
    tWaitMap::iterator iter = mWaitMap.find(mapKey);    
    if (iter != mWaitMap.end())
    {
        LOG4CPLUS_INFO(mLogger, "Found data for key: " + mapKey + ", deleting");
        delete (*iter).second;
        mWaitMap.erase(iter);
    }
    else
    {
        LOG4CPLUS_ERROR(mLogger, "Data for key not found: " + mapKey);
    }
}

std::string NegotiatorStates::genWaitMapKey(const std::string& tag, const NegotiatorStates::ChannelState& state)
{
    return tag + convertIntegerToString((int)state);
}
