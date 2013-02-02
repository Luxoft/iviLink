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


#ifndef CHANNELOBSERVER_HPP_
#define CHANNELOBSERVER_HPP_

#include <string>

#include "CSignalSemaphore.hpp"
#include "Types.hpp"
#include "Logger.hpp"

#include "IChannelObserver.hpp"
#include "Common.hpp"
#include "IChannelSupervisorObserver.hpp"

namespace iviLink
{
namespace ChannelSupervisor
{
static Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("ChannelSupervisor.ChannelObserver"));

//realization of IChannelObserver interface
class ChannelObserver: public IChannelObserver
{

public:
	explicit ChannelObserver(IChannelSupervisorTubeObserver * tubeObserver)
			: m_tubeObserver(tubeObserver)
	{
		m_sema = new CSignalSemaphore();
		mMutex = new CMutex();
	}

	virtual ~ChannelObserver()
	{
		delete m_sema;
		delete mMutex;
	}

	void semaWait()
	{
		m_sema->wait();
	}

	void semaSignal()
	{
		m_sema->signal();
	}

	void dataReceivedCallback(const UInt32 channel_id, const UInt32 read_size)
	{
		LOG4CPLUS_TRACE_METHOD(logger,
				"ChannelObserver::dataReceivedCallback()=> channel"
						+ convertIntegerToString(channel_id) + "size"
						+ convertIntegerToString(read_size));
		if (m_tubeObserver)
		{
			MutexLocker lock(*mMutex);
			m_tubeObserver->onDataReceived(channel_id, read_size);
		}
	}

	void bufferOverflowCallback(const UInt32 channel_id)
	{
		LOG4CPLUS_TRACE_METHOD(logger,
				"ChannelObserver::bufferOverflowCallback()=> channel"
						+ convertIntegerToString(channel_id));
		if (m_tubeObserver)
			m_tubeObserver->onBufferOverflow(channel_id);
	}

	void channelDeletedCallback(const UInt32 channel_id)
	{
		LOG4CPLUS_TRACE_METHOD(logger,
				"ChannelObserver::channelDeletedCallback()=> channel"
						+ convertIntegerToString(channel_id));

		if (m_tubeObserver)
		{
			semaSignal();
			m_tubeObserver->onChannelDeleted(channel_id);
		}

		m_tubeObserver = 0;
	}

	void connectionLostCallback()
	{
		LOG4CPLUS_TRACE_METHOD(logger, __PRETTY_FUNCTION__);
		if (m_tubeObserver)
			m_tubeObserver->onConnectionLost();
	}

	CMutex * getMutex()
	{
		return mMutex;
	}
private:
	IChannelSupervisorTubeObserver * m_tubeObserver;
	CSignalSemaphore * m_sema;
	/* is locked in allocateChannel and in dataReceivedCallback (so that dataReceived will
	 * never come before channel allocation is done) */
	CMutex * mMutex;
};

}
}

#endif /* CHANNELOBSERVER_HPP_ */
