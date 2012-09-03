/* 
 * 
 * iviLINK SDK, version 1.1.2
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











#ifndef NEGOTIATORSTATES_HPP_
#define NEGOTIATORSTATES_HPP_

#include <map>
#include <iostream>
#include <string>

#include "utils/threads/CRWMutex.hpp"
#include "utils/threads/CCondVar.hpp"
#include "framework/components/ChannelSupervisor/common/CSError.hpp"
#include "framework/components/ChannelSupervisor/common/Messages/Notification.hpp"
#include "utils/misc/Logger.hpp"

namespace iviLink {
namespace ChannelSupervisor {

class NegotiaterStates
{
public:
   friend class NegotiatorIPCHandler;
   enum STATE
   {
      IDLE,
      NEGOTIATED,
      CHECKEDINMAP,
      CAALLOCATED,
      ALLOCDONE
   };

   typedef std::map< std::string, std::pair < STATE, Messages::Notification* > > StateMap;
   CError ProcessTubeNotification( UInt8* rawNotification);
   STATE GetTagState(const char * tag);
   Messages::Notification * GetNotification(const char * tag);
   void ParseResponse__(pugi::xml_document* doc, char* responseStr);
private:
   CCondVar       mStateCond;
   bool           mWaitFlag;
   CRWMutex       m_mapMutex;
   StateMap       m_StateMap;
   static Logger        msLogger;
};

}  // namespace ChannelSupervisor
}  // namespace AXIS

#endif /* NEGOTIATORSTATES_HPP_ */
