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


#ifndef NEGOTIATORIPCHANDLER_HPP_
#define NEGOTIATORIPCHANDLER_HPP_

#include <cassert>
#include <iostream>
#include <string>
#include <sstream>

#include "pugixml.hpp"
#include "CSError.hpp"
#include "NegotiatorStates.hpp"

#include "CSignalSemaphore.hpp"
#include "CThread.hpp"
#include "CMutex.hpp"
#include "Types.hpp"
#include "CIpc.hpp"
#include "ICallbackHandler.hpp"
#include "Map.hpp"
#include "Logger.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{

const char ipcNegotiatorId[] = "ipc-negotiator";

void ParseResponse(pugi::xml_document* doc, char* responseStr);

class NegotiatorTube;
class NegotiatorStates;

class NegotiatorIPCHandler: public iviLink::Ipc::ICallbackHandler
{
public:

	NegotiatorIPCHandler(NegotiatorTube * tube, NegotiatorStates * states,
			std::string const& sock_addr);

	virtual ~NegotiatorIPCHandler();

	//IPC functionality
	void IPCWait();
	void IPCDisconnect();
	bool IPCConnect();         //wait for incoming connection
	virtual void OnConnection(iviLink::Ipc::DirectionID dirId)
	{
	}
	virtual void OnConnectionLost(iviLink::Ipc::DirectionID dirId);  //callback
	virtual void OnRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
			UInt8* const pResponseBuffer, UInt32& bufferSize, iviLink::Ipc::DirectionID dirId); //incoming data
	virtual void OnAsyncRequest(iviLink::Ipc::MsgID id, UInt8 const* pPayload, UInt32 payloadSize,
			 iviLink::Ipc::DirectionID dirId);

	void ProcessIPCClientMessage(std::string message, std::string & response);

	BaseError NegotiateChannel(std::string tag, UInt32 &cid);
	BaseError UpdateMap(std::string tag, UInt32 cid);
	BaseError ChannelDeallocated(const UInt32 cid);
	BaseError UpdateChannelInfo(std::string const& tag, UInt32 & channelId);
	void dismissSema();

private:
	/**
	 * Serializes provided notification and sends it to the
	 * other side via NegotiatorTube
	 */
	void sendData(Messages::Notification * noti);

	iviLink::Ipc::CIpc * m_ipc;            //ipc object
	CSignalSemaphore m_connLostSem;    //ipc connection lost sema
	CSignalSemaphore m_reqSem;         //sema: data is processed and response is sent

	NegotiatorTube * m_negotiatorTube; //tube 0

	Map * m_map;            //map containing all the allocated channel ids and tags

	NegotiatorStates* m_negotiatorStates;

	std::string m_currentTag;
	static Logger msLogger;
};

}  // namespace ChannelSupervisor
}  // namespace AXIS

#endif /* NEGOTIATORIPCHANDLER_HPP_ */
