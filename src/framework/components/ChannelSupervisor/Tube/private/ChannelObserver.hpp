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











#ifndef CHANNELOBSERVER_HPP_
#define CHANNELOBSERVER_HPP_

#include <string>

#include "utils/threads/CSignalSemaphore.hpp"
#include "utils/misc/Types.hpp"
#include "utils/misc/Logger.hpp"

#include "framework/components/ConnectivityAgent/generic/common/IChannelObserver.hpp"
#include "framework/components/ChannelSupervisor/common/Common.hpp"
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
   }
   virtual ~ChannelObserver()
   {
      if(m_sema)
         delete m_sema;
      m_sema = 0;
   }

   void semaWait()
   {
      m_sema->wait();
   }

   void semaSignal()
   {
      m_sema->signal();
   }


   void dataReceivedCallback(const unsigned int channel_id,
         const unsigned int read_size)
   {
      if(m_tubeObserver) m_tubeObserver->dataReceivedCallback(channel_id, read_size);
      LOG4CPLUS_TRACE(logger, "ChannelObserver::dataReceivedCallback()=> channel"
            +convertIntegerToString(channel_id) + "size"+convertIntegerToString(read_size));
   }

   void bufferOverflowCallback(const unsigned int channel_id)
   {
      if(m_tubeObserver) m_tubeObserver->bufferOverflowCallback(channel_id);
      LOG4CPLUS_TRACE(logger, "ChannelObserver::bufferOverflowCallback()=> channel"
         +convertIntegerToString(channel_id));
   }

   void channelDeletedCallback(const unsigned int channel_id)
   {
      LOG4CPLUS_TRACE(logger, "ChannelObserver::channelDeletedCallback()=> channel"
         +convertIntegerToString(channel_id));

      if(m_tubeObserver)
      {
         semaSignal();
         m_tubeObserver->channelDeletedCallback(channel_id);
      }

      m_tubeObserver = 0;
   }

   void connectionLostCallback()
   {
      if(m_tubeObserver)
         m_tubeObserver->connectionLostCallback();
      LOG4CPLUS_TRACE(logger, "NegotiatorTube::connectionLostCallback()");
   }
private:
   IChannelSupervisorTubeObserver *    m_tubeObserver;
   CSignalSemaphore*                   m_sema;
};

}
}


#endif /* CHANNELOBSERVER_HPP_ */
