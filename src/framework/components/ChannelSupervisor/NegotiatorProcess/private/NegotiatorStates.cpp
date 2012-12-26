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
 

#include <map>
#include <iostream>
#include <string>

#include "CSError.hpp"
#include "Notification.hpp"
#include "AllocateNotification.hpp"
#include "MapCheckNotification.hpp"
#include "CAAllocationNotification.hpp"
#include "UpdateMapNotification.hpp"
#include "DeallocateNotification.hpp"
#include "NegotiatorStates.hpp"

#include "Types.hpp"

using namespace std;
using namespace iviLink::ChannelSupervisor;
using namespace iviLink::ChannelSupervisor::Messages;

Logger NegotiaterStates::msLogger = Logger::getInstance(
        LOG4CPLUS_TEXT("NegotiatorProcess.NegotiaterStates"));

CError NegotiaterStates::ProcessTubeNotification(UInt8* rawNotification)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    CError err = CSError(CSError::ERROR_OTHER);
    pugi::xml_document* doc = new pugi::xml_document();
    ParseResponse__(doc, (char*) rawNotification);
    //obtain tag
    string tag = Notification::GetNotificationTag(doc);
    //get the state
    STATE currentState = GetTagStateInternal(tag.c_str());

    switch (Notification::GetNotificationType(doc))
    {
    case Notification::NOTIFICATIONTTYPE_ALLOCATE:
    {
        LOG4CPLUS_TRACE(msLogger,
                "NegotiaterStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_ALLOCATE");
        if (currentState == IDLE)
        {
            LOG4CPLUS_INFO(msLogger, "NegotiaterStates::ProcessTubeNotification()=>State is IDLE");
            mStateCond.lock();
            m_mapMutex.lockWrite();
            StateMap::iterator it = m_StateMap.find(tag);

            if (it != m_StateMap.end())
            {
                LOG4CPLUS_INFO(msLogger,
                        "NegotiaterStates::ProcessTubeNotification()=>tag found, removing it");
                DeallocateNotification * noti = (DeallocateNotification *) (*it).second.second;
                if (noti)
                    delete noti;
                m_StateMap.erase(it);
            }
            Notification * noti = new AllocateNotification(doc);
            m_StateMap.insert(make_pair(tag, make_pair(NEGOTIATED, noti)));
            LOG4CPLUS_INFO(msLogger,
                    "NegotiaterStates::ProcessTubeNotification()=>Go to the NEGOTIATED STATE");
            mStateCond.broadcast();
            m_mapMutex.unlockWrite();
            mStateCond.unlock();
        }   //change state only if current state is IDLE
        break;
    }
    case Notification::NOTIFICATIONTTYPE_CHECK_MAP:
    {
        LOG4CPLUS_INFO(msLogger,
                "NegotiaterStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_CHECK_MAP");
        //change state only if current state is NEGOTIATED
        if (currentState == NEGOTIATED)
        {
            LOG4CPLUS_INFO(msLogger,
                    "NegotiaterStates::ProcessTubeNotification()=>State is NEGOTIATED");
            mStateCond.lock();
            m_mapMutex.lockWrite();
            StateMap::iterator it = m_StateMap.find(tag);
            if (it != m_StateMap.end())
            {
                AllocateNotification * noti = (AllocateNotification *) (*it).second.second;
                if (noti)
                    delete noti;
                LOG4CPLUS_INFO(msLogger,
                        "NegotiaterStates::ProcessTubeNotification()=>Go to the CHECKEDINMAP STATE");
                (*it).second.first = CHECKEDINMAP;
                (*it).second.second = new MapCheckNotification(doc);
                mStateCond.broadcast();
            }
            m_mapMutex.unlockWrite();
            mStateCond.unlock();
        }
        break;
    }
    case Notification::NOTIFICATIONTTYPE_CA_ALLOCATE:
    {
        LOG4CPLUS_INFO(msLogger,
                "NegotiaterStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_CA_ALLOCATE");
        if (currentState == CHECKEDINMAP)
        {
            LOG4CPLUS_INFO(msLogger,
                    "NegotiaterStates::ProcessTubeNotification()=>State is CHECKEDINMAP");
            mStateCond.lock();
            m_mapMutex.lockWrite();
            StateMap::iterator it = m_StateMap.find(tag);
            if (it != m_StateMap.end())
            {
                MapCheckNotification * noti = (MapCheckNotification *) (*it).second.second;
                if (noti)
                    delete noti;
                LOG4CPLUS_INFO(msLogger,
                        "NegotiaterStates::ProcessTubeNotification()=>Go to the CAALLOCATED STATE");
                (*it).second.first = CAALLOCATED;
                (*it).second.second = new CAAllocationNotification(doc);
                mStateCond.broadcast();
            }
            m_mapMutex.unlockWrite();
            mStateCond.unlock();
        }
        break;
    }
    case Notification::NOTIFICATIONTTYPE_MAP_ADD_CID:
    {
        LOG4CPLUS_INFO(msLogger,
                "NegotiaterStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_MAP_ADD_CID");
        //change state only if current state is CAALLOCATED
        if (currentState == CAALLOCATED)
        {
            LOG4CPLUS_INFO(msLogger,
                    "NegotiaterStates::ProcessTubeNotification()=>State is CAALLOCATED");
            mStateCond.lock();
            m_mapMutex.lockWrite();
            StateMap::iterator it = m_StateMap.find(tag);
            if (it != m_StateMap.end())
            {
                CAAllocationNotification * noti = (CAAllocationNotification *) (*it).second.second;
                if (noti)
                    delete noti;
                //(*it).second.first = ALLOCDONE;
                (*it).second.first = IDLE;
                (*it).second.second = new UpdateMapNotification(doc);
                LOG4CPLUS_INFO(msLogger,
                        "NegotiaterStates::ProcessTubeNotification()=>Go to the ALLOCDONE/IDLE STATE");
                mStateCond.broadcast();
            }
            m_mapMutex.unlockWrite();
            mStateCond.unlock();
        }
        break;
    }
    case Notification::NOTIFICATIONTTYPE_DEALLOCATE:
    {
        LOG4CPLUS_INFO(msLogger,
                "NegotiaterStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_DEALLOCATE");
        //change state only if current state is ALLOCDONE
        if (currentState == ALLOCDONE)
        {
            LOG4CPLUS_INFO(msLogger,
                    "NegotiaterStates::ProcessTubeNotification()=>State is ALLOCDONE");
            mStateCond.lock();
            m_mapMutex.lockWrite();
            StateMap::iterator it = m_StateMap.find(tag);
            if (it != m_StateMap.end())
            {
                UpdateMapNotification * noti = (UpdateMapNotification *) (*it).second.second;
                if (noti)
                    delete noti;
                (*it).second.first = IDLE;
                (*it).second.second = new DeallocateNotification(doc);
                LOG4CPLUS_INFO(msLogger,
                        "NegotiaterStates::ProcessTubeNotification()=>Go to the IDLE STATE");
                mStateCond.broadcast();
            }
            m_mapMutex.unlockWrite();
            mStateCond.unlock();
        }
        break;
    }
    default:
    {
        LOG4CPLUS_ERROR(msLogger,
                "NegotiaterStates::ProcessTubeNotification()=>Unknown Notification type!!!");
        break;
    }
    }

    return err;
}

NegotiaterStates::STATE NegotiaterStates::GetTagState(const char * tag)
{
    LOG4CPLUS_TRACE_METHOD(msLogger,
            "NegotiaterStates::GetTagState()=>=>tag = " + (std::string) tag);
    std::string complementingTag = Notification::GetComplementingTag(tag);
    LOG4CPLUS_INFO(msLogger,
            "GetTagState():Complementing Tag: " + complementingTag + " for tag " + tag);
    return GetTagStateInternal(complementingTag.c_str());
}

NegotiaterStates::STATE NegotiaterStates::GetTagStateInternal(const char * tag)
{
    LOG4CPLUS_TRACE_METHOD(msLogger,
            "NegotiaterStates::GetTagStateInternal()=>=>tag = " + (std::string) tag);
    NegotiaterStates::STATE ret = IDLE;
    m_mapMutex.lockRead();
    StateMap::iterator it = m_StateMap.find(tag);
    if (it != m_StateMap.end())
    {
        ret = ((*it).second).first;
        LOG4CPLUS_INFO(msLogger,
                "NegotiaterStates::GetTagState()=>=>State found " + StateToString(ret));
    } else
    {
        LOG4CPLUS_INFO(msLogger, "NegotiaterStates::GetTagState()=>ERROR!! State not found!");
    }
    m_mapMutex.unlockRead();
    return ret;
}

Notification * NegotiaterStates::GetNotification(const char * tag)
{
    LOG4CPLUS_TRACE_METHOD(msLogger,
            "NegotiaterStates::GetNotification(): tag = " + (std::string) tag);
    std::string complementingTag = Notification::GetComplementingTag(tag);
    LOG4CPLUS_INFO(msLogger,
            "GetNotification():Complementing Tag: " + complementingTag + " for tag " + tag);
    return GetNotificationInternal(complementingTag.c_str());
}

Notification * NegotiaterStates::GetNotificationInternal(const char * tag)
{
    LOG4CPLUS_TRACE_METHOD(msLogger,
            "NegotiaterStates::GetNotification(): tag = " + (std::string) tag);
    Notification* ret = NULL;
    m_mapMutex.lockRead();
    StateMap::iterator it = m_StateMap.find(tag);
    if (it != m_StateMap.end())
    {
        LOG4CPLUS_INFO(msLogger, "NegotiaterStates::GetNotification()=>Notification found!");
        ret = ((*it).second).second;
    } else
    {
        LOG4CPLUS_INFO(msLogger, "NegotiaterStates::GetNotification()=>Notification not found!!!");
    }
    m_mapMutex.unlockRead();
    return ret;
}

void NegotiaterStates::ParseResponse__(pugi::xml_document* doc, char* responseStr)
{
    LOG4CPLUS_TRACE_METHOD(msLogger,
            "NegotiaterStates::ParseResponse__() \n /////////////// \n" + (std::string) responseStr
                    + "\n///////////////\n");
    pugi::xml_parse_result res;
    res = doc->load((const char *) responseStr);
}

std::string NegotiaterStates::StateToString(STATE state)
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
    case ALLOCDONE:
        return "ALLOCDONE";
    }
    return "";
}
