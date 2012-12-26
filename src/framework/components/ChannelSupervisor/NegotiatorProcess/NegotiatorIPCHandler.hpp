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
class NegotiaterStates;

class NegotiatorIPCHandler: public iviLink::Ipc::ICallbackHandler
{
public:

	NegotiatorIPCHandler(NegotiatorTube * tube, NegotiaterStates * states,
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

	CError NegotiateChannel(std::string tag, UInt32 &cid);
	CError UpdateMap(std::string tag, UInt32 cid);
	CError ChannelDeallocated(const UInt32 cid);
	void dismissSema();

private:
	/**
	 * Blocking call. Waits until there is the requested state for the specified tag in the
	 * NegotiaterStates' map.
	 * @param stateToWait state for the channel tag we are waiting for
	 * @param tag channel tag
	 * @param timeoutMillis will wait no longer than this time slot.
	 */
	void waitStateForTag(NegotiaterStates::STATE stateToWait, std::string tag,
			UInt32 timeoutMillis);

	iviLink::Ipc::CIpc * m_ipc;            //ipc object
	CSignalSemaphore m_connLostSem;    //ipc connection lost sema
	iviLink::Ipc::MsgID m_id;             //ipc message id. is not used
	CSignalSemaphore m_reqSem;         //sema: data is processed and response is sent

	NegotiatorTube * m_negotiatorTube; //tube 0

	Map * m_map;            //map containing all the allocated channel ids and tags

	NegotiaterStates* m_negotiatorStates;

	std::string m_currentTag;
	static Logger msLogger;
};

}  // namespace ChannelSupervisor
}  // namespace AXIS

#endif /* NEGOTIATORIPCHANDLER_HPP_ */
