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







#ifndef NEGOTIATOR_HPP_
#define NEGOTIATOR_HPP_


#include "utils/xml/pugixml.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/misc/Types.hpp"
#include "utils/threads/CSignalSemaphore.hpp"

#include "NegotiatorIPCHandler.hpp"
#include "NegotiatorStates.hpp"
#include "CNegotiatorToSysCtrlProxy.hpp"
#include "Queue.hpp"
#include "utils/misc/Logger.hpp"

namespace iviLink {
namespace ChannelSupervisor {

class Negotiator
{
public:
   NegotiatorIPCHandler* getHandler();

   void onShutDown()
   {
      m_ipcHandler->dismissSema();
   }

   Negotiator(std::string const& sock_addr);
   ~Negotiator();

private:

   NegotiatorIPCHandler * m_ipcHandler;

   CNegotiatorToSysCtrlProxy mSysCtrlProxy;
   static Logger msLogger;
};

}  // namespace ChannelSupervisor
}  // namespace AXIS

#endif /* NEGOTIATOR_HPP_ */
