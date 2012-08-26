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









#ifndef NEGOTIATORTUBE_HPP_
#define NEGOTIATORTUBE_HPP_

#include "framework/components/ChannelSupervisor/Tube/ChannelSupervisorTube.hpp"
#include "framework/components/ChannelSupervisor/Tube/IChannelSupervisorObserver.hpp"
#include "NegotiatorStates.hpp"
#include "CNegotiatorToSysCtrlProxy.hpp" 
#include "utils/misc/Logger.hpp"

namespace iviLink {
namespace ChannelSupervisor {

class NegotiatorTube: public IChannelSupervisorTubeObserver
{

public:

	NegotiatorTube(NegotiaterStates * states, CNegotiatorToSysCtrlProxy* pMsgProxy) :
		m_negotiatorStates(states), mpMsgProxy(pMsgProxy)
	{
		allocateZeroChannel(this, "Channel supervisor");
		m_sender = new Sender();
		mpMsgProxy->sendConnectionEstablished();
	}
	~NegotiatorTube()
	{
		deallocateChannel(CS_SERVICE_CHANNEL, "Channel supervisor");
		if(m_sender)
			delete m_sender;
		m_sender = 0;
		mpMsgProxy->sendConnectionLost();
	}

	void dataReceivedCallback(const unsigned int channelId, const unsigned int readSize);
	void bufferOverflowCallback(const unsigned int channelId);
	void channelDeletedCallback(const unsigned int channelId);
	void connectionLostCallback();

	void sendDataTube(const char *);

private:
	class Sender
	{
	public:
		Sender() : m_CSChannelID(CS_SERVICE_CHANNEL), m_CSTag("Channel Supervisor"){}
		virtual ~Sender(){}
		void sendDataRemote(std::string data);	//send the data to the remote device

	private:
		unsigned int	m_CSChannelID;
		std::string		m_CSTag;
	};

	Sender*                    m_sender;
	NegotiaterStates*          m_negotiatorStates;
	CNegotiatorToSysCtrlProxy* mpMsgProxy;
	static Logger        		msLogger;

};

}  // namespace ChannelSupervisor
}  // namespace AXIS

#endif /* NEGOTIATORTUBE_HPP_ */
