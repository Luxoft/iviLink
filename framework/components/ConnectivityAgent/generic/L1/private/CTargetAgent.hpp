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


#ifndef CTARGETAGENT_HPP
#define CTARGETAGENT_HPP

#include <list>

#include "Types.hpp"
#include "CMutex.hpp"
#include "IBufferConsumer.hpp"

namespace iviLink
{
   namespace ConnectivityAgent
   {
      class Buffer;

      namespace L1
      {
         class IBufferProducer;

         ///CTargetAgent class
         /**
          * The CTargetAgent implements IDataReceiver interface and represents data targets at L0 level.
          * It receives from CTarget, i.e. target at HAL level, callback with filled buffer, and provides the free buffers back to CTarget.
          * On channel closing CTarget calls destroy() function.
          */
         class CTargetAgent: public IBufferConsumer
         {
         private:
            static Logger logger;

            friend class CTargetAgentTest;
         public:

             // Methods section
            /**
             * Constructor
             * @param channelID - logical channel ID
             */
            explicit CTargetAgent(UInt32 channelID);

            /**
             * Destructor
             */
            virtual  ~CTargetAgent();

            //from IDataConsumer

            /**
             * Interface function inherited from IDataConsumer. It provides callback for IDataProducer for getting the data
             * @param buffer is the pointer to filled data buffer
             */
            virtual ConnectivityAgentError consumeBuffer(iviLink::ConnectivityAgent::Buffer* buffer);

            /**
             * Interface function inherited from IBufferConsumer. It registers IBufferProducer data receiver in HAL for providing free buffers.
             * @param pProducer is the pointer to the associated IBufferProducer object
             */
            virtual void registerProducer(IBufferProducer* pProducer);

            virtual void unregisterProducer(IBufferProducer* pProducer);
            /**
             * Interface function inherited from IBufferConsumer. It returns all buffers to registered IBufferProducer.
             * @return
             */
            virtual void flushBuffers();

            /**
             * Interface function inherited from IDataReceiver. It is called on channel closing by associated CTarget object for destruction
             */
            virtual void destroy();

            /**
             * @return channel number of current target agent
             */
            const UInt32 getChannelID() const
            {
               return mChannelID;
            }

            ConnectivityAgentError visitStub();

         private:
            // Methods section

            /**
             * Copying constructor
             *
             * Unimplemented to avoid missusing
             *
             * @param classRef The reference to the object the class is constructing on
             */
            CTargetAgent(const CTargetAgent& );

            /**
             * Assigning operator
             *
             * Unimplemented to avoid missusing
             *
             * @param classRef The reference to the object the class is assiging from
             */
            CTargetAgent& operator=(const CTargetAgent& );

         private:
            // Members section

            typedef std::list<iviLink::ConnectivityAgent::Buffer*> tBufferQueue;
            tBufferQueue                           mBufferQueue;
            UInt32                                 mChannelID;
            IBufferProducer*                       mpBufferProducer;
            mutable CMutex                         mBufferQueueMutex;
         };

      }
   }
}

#endif
