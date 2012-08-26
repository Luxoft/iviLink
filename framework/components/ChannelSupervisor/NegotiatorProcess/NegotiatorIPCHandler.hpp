/* 
 * 
 * iviLINK SDK, version 1.0.1
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









#ifndef NEGOTIATORIPCHANDLER_HPP_
#define NEGOTIATORIPCHANDLER_HPP_

#include <cassert>
#include <iostream>
#include <string>
#include <sstream>

#include "utils/xml/pugixml.hpp"
#include "framework/components/ChannelSupervisor/common/CSError.hpp"

#include "utils/threads/CSignalSemaphore.hpp"
#include "utils/threads/CThread.hpp"
#include "utils/threads/CMutex.hpp"
#include "utils/misc/Types.hpp"
#include "utils/ipc/CIpc.hpp"
#include "utils/ipc/ICallbackHandler.hpp"

#include "framework/components/ChannelSupervisor/Map/Map.hpp"
#include "utils/misc/Logger.hpp"

namespace iviLink {
namespace ChannelSupervisor {


const char ipcNegotiatorId[] = "ipc-negotiator";

void ParseResponse(pugi::xml_document* doc, char* responseStr);

class NegotiatorTube;
class NegotiaterStates;

class NegotiatorIPCHandler: public iviLink::Ipc::ICallbackHandler
{
public:

   NegotiatorIPCHandler(NegotiatorTube * tube, NegotiaterStates * states, std::string const& sock_addr);

   virtual ~NegotiatorIPCHandler();

   //IPC functionality
   void IPCWait();
   void IPCDisconnect();
   bool IPCConnect();         //wait for incoming connection
   virtual void OnConnection(iviLink::Ipc::DirectionID dirId){}
   virtual void OnConnectionLost(iviLink::Ipc::DirectionID dirId);  //callback
   virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
               UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID dirId); //incoming data

   void ProcessIPCClientMessage(std::string message, std::string & response);

   CError NegotiateChannel(std::string tag, UInt32 &cid);
   CError UpdateMap(std::string tag, UInt32 cid);
   CError ChannelDeallocated(std::string tag, UInt32 cid);
   void dismissSema();
private:

   enum State
   {
      IDLE,
      NEGOTIATED,
      COMPLETED
   };

   iviLink::Ipc::CIpc * m_ipc;            //ipc object
   CSignalSemaphore  m_connLostSem;    //ipc connection lost sema
   iviLink::Ipc::MsgID  m_id;             //ipc message id. is not used
   CSignalSemaphore  m_reqSem;         //sema: data is processed and response is sent

   NegotiatorTube *  m_negotiatorTube; //tube 0

   Map *             m_map;            //map containing all the allocated channel ids and tags

   NegotiaterStates* m_negotiatorStates;

   State             m_state;
   std::string       m_currentTag;
   static Logger     msLogger;
};

}  // namespace ChannelSupervisor
}  // namespace AXIS

#endif /* NEGOTIATORIPCHANDLER_HPP_ */
