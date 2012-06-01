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
 * ChannelSupervisorMsgProxy.hpp
 *
 *  Created on: Mar 19, 2012
 *      Author: Kramchaninov Pavel
 */

#ifndef CHANNEL_SUPERVISOR_MSG_PROXY_HPP
#define CHANNEL_SUPERVISOR_MSG_PROXY_HPP

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

class ChannelSupervisorMsgProxy : public AXIS::Ipc::ICallbackHandler
{
private:

   static Logger logger;

   const char* getName() {return "ChannelSupervisorMsgProxy";}

   AXIS::Ipc::CIpc* mpIpc;

public:
   explicit ChannelSupervisorMsgProxy(const string connectionName);
   virtual ~ChannelSupervisorMsgProxy();

protected:

   // Outgoing messages
   //
   CError requestShutDown();

   // Incoming messages
   // should be implemented by implementation
   virtual CError onCounterCSConnected() = 0;
   virtual CError onCounterCSDisconnected() = 0;

protected:
   // from ICallbackHandler

   virtual void OnConnection(AXIS::Ipc::DirectionID);
   virtual void OnConnectionLost(AXIS::Ipc::DirectionID);
   virtual void OnRequest(AXIS::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize, UInt8* const pResponseBuffer, UInt32& bufferSize, AXIS::Ipc::DirectionID);

   virtual void onChannelSupervisorAvailable() = 0;
   virtual void onChannelSupervisorNotAvailable() = 0;

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

   CError connectChannelSupervisor();

   bool isConnected() const;

   enum
   {
      BUFFER_SIZE = 4096
   };

   UInt8 mReadBuffer[BUFFER_SIZE];
   UInt8 mWriteBuffer[BUFFER_SIZE];
};

}
#endif /* CHANNEL_SUPERVISOR_MSG_PROXY_HPP */
