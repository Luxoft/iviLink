/* 
 * 
 * iviLINK SDK, version 1.0
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
 * 
 */







#include <map>
#include <iostream>
#include <string>

#include "framework/components/ChannelSupervisor/common/CSError.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/Notification.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/AllocateNotification.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/MapCheckNotification.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/CAAllocationNotification.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/UpdateMapNotification.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/DeallocateNotification.hpp"
#include "NegotiatorStates.hpp"

#include "utils/misc/Types.hpp"
#include "utils/misc/logging.hpp"

using namespace std;
using namespace iviLink::ChannelSupervisor;
using namespace iviLink::ChannelSupervisor::Messages;

Logger NegotiaterStates::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("NegotiatorProcess.NegotiaterStates"));

CError NegotiaterStates::ProcessTubeNotification(UInt8* rawNotification)
{
  LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()");

   CError err = CSError(CSError::ERROR_OTHER);

   pugi::xml_document* doc = new pugi::xml_document();
   ParseResponse__(doc, (char*) rawNotification);

   //obtain tag
   string tag = Notification::GetNotificationTag(doc);
   //get the state
   STATE currentState = GetTagState(tag.c_str());

   switch (Notification::GetNotificationType(doc))
   {
      case Notification::NOTIFICATIONTTYPE_ALLOCATE:
      {
         LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_ALLOCATE");

         if (currentState == IDLE)
         {
            LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>State is IDLE");
            mStateCond.lock();
            m_mapMutex.lockWrite();
            StateMap::iterator it = m_StateMap.find(tag);

            if (it != m_StateMap.end())
            {
               LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>tag found, removing it");
               DeallocateNotification * noti = (DeallocateNotification *) (*it).second.second;
               delete noti;

               m_StateMap.erase(it);
            }

            Notification * noti = new AllocateNotification(doc);
            LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>Allocation Notification message created");
            m_StateMap.insert(make_pair(tag, make_pair(NEGOTIATED, noti)));
            LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>Go to the NEGOTIATED STATE");
            mStateCond.broadcast();

            m_mapMutex.unlock();
            mStateCond.unlock();
         }   //change state only if current state is IDLE
         break;
      }
      case Notification::NOTIFICATIONTTYPE_CHECK_MAP:
      {
         LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_CHECK_MAP");
         if (currentState == NEGOTIATED)
         {

            LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>State is NEGOTIATED");
            mStateCond.lock();
            m_mapMutex.lockWrite();
            StateMap::iterator it = m_StateMap.find(tag);
            if (it != m_StateMap.end())
            {
               AllocateNotification * noti = (AllocateNotification *) (*it).second.second;
               if (noti)
                  delete noti;
               noti = 0;
               LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>Go to the CHECKEDINMAP STATE");
               (*it).second.first = CHECKEDINMAP;
               (*it).second.second = new MapCheckNotification(doc);
               mStateCond.broadcast();
            }
            m_mapMutex.unlock();
            mStateCond.unlock();
         }
         break;
      }
         //change state only if current state is NEGOTIATED
      case Notification::NOTIFICATIONTTYPE_CA_ALLOCATE:
      {
         LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_CA_ALLOCATE");
         if (currentState == CHECKEDINMAP)
         {
            LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>State is CHECKEDINMAP");
            mStateCond.lock();
            m_mapMutex.lockWrite();
            StateMap::iterator it = m_StateMap.find(tag);
            if (it != m_StateMap.end())
            {

               MapCheckNotification * noti = (MapCheckNotification *) (*it).second.second;
               if (noti)
                  delete noti;
               noti = 0;
               LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>Go to the CAALLOCATED STATE");
               (*it).second.first = CAALLOCATED;
               (*it).second.second = new CAAllocationNotification(doc);
               mStateCond.broadcast();
            }
            m_mapMutex.unlock();
            mStateCond.unlock();
         }
         break;      
      }
      case Notification::NOTIFICATIONTTYPE_MAP_ADD_CID:
      {
         LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_MAP_ADD_CID");
         //change state only if current state is CAALLOCATED
         if (currentState == CAALLOCATED)
         {
            LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>State is CAALLOCATED");
            mStateCond.lock();
            m_mapMutex.lockWrite();
            StateMap::iterator it = m_StateMap.find(tag);
            if (it != m_StateMap.end())
            {
               CAAllocationNotification * noti = (CAAllocationNotification *) (*it).second.second;
               if (noti)
                  delete noti;
               noti = 0;

               //(*it).second.first = ALLOCDONE;
               (*it).second.first = IDLE;
               (*it).second.second = new UpdateMapNotification (doc);
               LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>Go to the ALLOCDONE/IDLE STATE");
               mStateCond.broadcast();
            }
            m_mapMutex.unlock();
            mStateCond.unlock();
         }
         break;
      }
      case Notification::NOTIFICATIONTTYPE_DEALLOCATE:
      {
         LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>NOTIFICATIONTTYPE_DEALLOCATE");
         //change state only if current state is ALLOCDONE
         if (currentState == ALLOCDONE)
         {
            LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>State is ALLOCDONE");
            mStateCond.lock();
            m_mapMutex.lockWrite();
            StateMap::iterator it = m_StateMap.find(tag);
            if (it != m_StateMap.end())
            {

               UpdateMapNotification * noti = (UpdateMapNotification *) (*it).second.second;
               if (noti)
                  delete noti;
               noti = 0;

               (*it).second.first = IDLE;
               (*it).second.second = new DeallocateNotification(doc);
               LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>Go to the IDLE STATE");
               mStateCond.broadcast();
            }
            m_mapMutex.unlock();
            mStateCond.unlock();
         }
         break;
      }
      default:
      {
         LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>Unknown Notification type!!!");
         break;
      }
   }
   LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::ProcessTubeNotification()=>END");
   return err;
}

NegotiaterStates::STATE NegotiaterStates::GetTagState(const char * tag)
{
   LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::GetTagState()=>=>tag"+ (std::string)tag);
   NegotiaterStates::STATE ret = IDLE;
   m_mapMutex.lockRead();
   StateMap::iterator it = m_StateMap.find(tag);
   if (it != m_StateMap.end())
   {
      ret =  ((*it).second).first;
      LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::GetTagState()=>=>State found"+ convertIntegerToString((int)ret));
   }
   else
   {
      LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::GetTagState()=>ERROR!! State not found!");
   }
   m_mapMutex.unlock();
   return ret;
}

Notification * NegotiaterStates::GetNotification(const char * tag)
{
   Notification* ret = NULL;

   m_mapMutex.lockRead();
   StateMap::iterator it = m_StateMap.find(tag);
   if (it != m_StateMap.end())
   {
      LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::GetNotification()=>Notification found!");
      ret = ((*it).second).second;
   }
   else
   {
      LOG4CPLUS_TRACE(msLogger, "NegotiaterStates::GetNotification()=>Notification not found!!!");
   }
   m_mapMutex.unlock();

   return ret;
}

void NegotiaterStates::ParseResponse__(pugi::xml_document* doc, char* responseStr)
{
   LOG4CPLUS_TRACE(msLogger,"NegotiaterStates::ParseResponse__() \n /////////////// \n" +(std::string)responseStr + "\n///////////////\n" );
   pugi::xml_parse_result res;
   res = doc->load((const char *) responseStr);
}

