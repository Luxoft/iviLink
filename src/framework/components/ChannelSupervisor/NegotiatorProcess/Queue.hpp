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





#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <queue>
#include "utils/threads/include/CMutex.hpp"
#include "utils/threads/include/CSignalSemaphore.hpp"
#include "utils/misc/include/Types.hpp"
#include "../common/Messages/NegotiatorMessage.hpp"

#define MAXQUEUESIZE 10

class MessageQueue
{
public:
	MessageQueue(CSignalSemaphore* sema) : 
		m_queueSize(0),
		m_sema(sema)
	{
	}

	int queueSize()
	{
		return m_queueSize;
	}

	bool enqueue(NegotiatorMessage* msg)
	{
		m_queueMutex.lock();
		if ( messageQueue.size() < MAXQUEUESIZE )
		{
			messageQueue.push(msg);
			m_queueSize++;
			m_queueMutex.unlock();
			m_sema->signal();
			return true;
		}
		m_queueMutex.unlock();
		return false;
	}

	NegotiatorMessage* dequeue()
	{
		m_queueMutex.lock();
		NegotiatorMessage* msg = NULL;
		if ( messageQueue.size() > 0 )
		{
			msg = messageQueue.front();	//take the message
			messageQueue.pop();			//remove from the q
			m_queueSize--;
		}
		m_queueMutex.unlock();
		return msg;
	}

private:
	std::queue<NegotiatorMessage*> messageQueue;
	int                            m_queueSize;
	CMutex						       m_queueMutex;
	CSignalSemaphore*			       m_sema;
};

#endif /* QUEUE_HPP */
