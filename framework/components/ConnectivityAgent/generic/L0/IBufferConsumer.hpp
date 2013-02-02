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


#ifndef IBUFFERCONSUMER_HPP_
#define IBUFFERCONSUMER_HPP_

#include "Types.hpp"
#include "ConnectivityAgentBuffer.hpp"
#include "IBufferProducer.hpp"
#include "ConnectivityAgentError.hpp"

namespace iviLink
{
   namespace ConnectivityAgent
   {
      class Buffer;

      namespace L0
      {
         /// IBufferConcumer interface
         /**
          * This interface is for consuming part of buffer producer-consumer pair.
          * Consumer must take filled buffers, process them and return to producer.
          *
          * On destruction, consumer MUST return all unprocessed buffers to producer so it can free them.
          *
          * @todo: may be we need to rewrite this form callbacks to multithreading synchronization patterns.
          */
         class IBufferConsumer
         {
         public:
            // Methods section

            /**
             * Method called by producer to take filled buffer to consumer.
             * @param pBuffer - pointer to filled buffer.
             * @return
             */
            virtual ConnectivityAgentError consumeBuffer(iviLink::ConnectivityAgent::Buffer* pBuffer) = 0;

            /**
             * Method is used to establish revert link between consumer and producer.
             * @param pProducer - pointer to producer.
             * @return
             */
            virtual void registerProducer(iviLink::ConnectivityAgent::L1::IBufferProducer* pProducer) = 0;

            /**
             * Method is called on producer destruction.
             * @param pProducer - pointer to producer.
             * @return
             */
            virtual void unregisterProducer(iviLink::ConnectivityAgent::L1::IBufferProducer* pProducer) = 0;

            /**
             * When this method called, consumer MUST discard all buffer processing and return buffers to producer.
             * @return
             */
            virtual void flushBuffers() = 0;

            virtual ~IBufferConsumer() {}
         };
      }
   }
}
#endif /* IBUFFERCONSUMER_HPP_ */
