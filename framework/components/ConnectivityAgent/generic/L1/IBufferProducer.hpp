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


#ifndef IBUFFERPRODUCER_HPP_
#define IBUFFERPRODUCER_HPP_

/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/
#include "Types.hpp"
//#include "framework/components/ConnectivityAgent/src/generic/L0/include/IBufferConsumer.hpp"
/********************************************************************
 *
 * Forward declarations
 *
 ********************************************************************/
namespace iviLink
{
   namespace ConnectivityAgent
   {
      class Buffer;

      namespace L0
      {
         class IBufferConsumer;
      }
      namespace L1
      {
         using iviLink::ConnectivityAgent::L0::IBufferConsumer;

         /// IBufferProducer interface
         /**
          * This interface is for producing part of buffer producer-consumer pair.
          * Producer must create buffers, fill them, pass them to consumer and retrieve them back.
          *
          * Producer must be destroyed after consumer destruction so all the buffers will be destroyed properly.
          *
          */
         class IBufferProducer
         {
         public:
            // Methods section

            /**
             * Method called by consumer to return back processed buffers, so producer can refill them.
             * @param pBuffer - pointer to free buffer
             * @return
             */
            virtual void returnBufferBack(iviLink::ConnectivityAgent::Buffer* pBuffer) = 0;

            /**
             * Method is used to establish direct link between producer and consumer.
             * @param pConsumer - pointer to consumer.
             * @return
             */
            virtual void registerConsumer(IBufferConsumer* pConsumer) = 0;

            /**
             * Method is called on consumer destruction.
             * @param pConsumer - pointer to consumer
             * @return
             */
            virtual void unregisterConsumer(IBufferConsumer* pConsumer) = 0;

            /**
             * @todo: may be we need to make here additional methods to manipulate buffers queue:
             * enlarge, shrink, e.t.c.
             */
         protected:
            // Methods section

            virtual ~IBufferProducer() {}
         };
      }
   }
}


#endif /* IBUFFERPRODUCER_HPP_ */
