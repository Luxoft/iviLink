/* 
 *  iviLINK SDK, version 0.9 (for preview only)                                      
 *    http://www.ivilink.net                                                         
 *  Cross Platform Application Communication Stack for In-Vehicle Applications       
 *                                                                                   
 *  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
 *                                                                                   
 *  This library is free software; you can redistribute it and/or                    
 *  modify it under the terms of the GNU Lesser General Public                       
 *  License as published by the Free Software Foundation; version 2.1.               
 *                                                                                   
 *  This library is distributed in the hope that it will be useful,                  
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
 *  Lesser General Public License for more details.                                  
 *                                                                                   
 *  You should have received a copy of the GNU Lesser General Public                 
 *  License along with this library; if not, write to the Free Software              
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
 * 
 * 
 * 
 * 
 */

/*
 * ConnectivityAgentMsgProxy.hpp
 *
 *  Created on: Mar 19, 2012
 *      Author: Kramchaninov Pavel
 */

#ifndef CONNECTIVITY_AGENT_MDG_PROXY_HPP
#define CONNECTIVITY_AGENT_MDG_PROXY_HPP

#include <string>
#include "utils/misc/include/CError.hpp"
#include "utils/ipc/include/ICallbackHandler.hpp"
#include "utils/ipc/include/common.hpp"
#include "utils/ipc/include/CIpc.hpp"

#include "utils/misc/include/Logger.hpp"

using namespace std;
using namespace AXIS::Ipc;

namespace SystemControllerMsgProtocol
{

class ConnectivityAgentMsgProxy : public AXIS::Ipc::ICallbackHandler
{
private:
   static Logger logger;

   const char* getName() {return "ConnectivityAgentMsgProxy";}

   AXIS::Ipc::CIpc* mpIpc;

public:
   explicit ConnectivityAgentMsgProxy(const string connectionName);
   virtual ~ConnectivityAgentMsgProxy();

protected:

   // Outgoing messages
   //
   CError requestShutDown();

   // Incoming messages
   // should be implemented by implementation
   virtual CError onCounterCAConnected() = 0;
   virtual CError onCounterCADisconnected() = 0;

protected:
   // from ICallbackHandler

   virtual void OnConnection(AXIS::Ipc::DirectionID);
   virtual void OnConnectionLost(AXIS::Ipc::DirectionID);

   virtual void OnRequest(AXIS::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, AXIS::Ipc::DirectionID);

   virtual void onConnectivityAgentAvailable() = 0;
   virtual void onConnectivityAgentNotAvailable() = 0;

   class CMsgIdGen
   {
   public:
      CMsgIdGen();
      ~CMsgIdGen();
      AXIS::Ipc::MsgID getNext();
   private:
      AXIS::Ipc::MsgID mId;
   };

   CMsgIdGen mMsgIdGen;

   CError connectConnectivityAgent();

   bool isConnected() const;

   enum
   {
      BUFFER_SIZE = 4096
   };

   UInt8 mReadBuffer[BUFFER_SIZE];
   UInt8 mWriteBuffer[BUFFER_SIZE];
};

}
#endif /* CONNECTIVITY_AGENT_MDG_PROXY_HPP */
