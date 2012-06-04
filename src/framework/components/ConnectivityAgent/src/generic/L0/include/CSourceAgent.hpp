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




#ifndef CSOURCEAGENT_HPP
#define CSOURCEAGENT_HPP
/********************************************************************
 *
 * System includes
 *
 ********************************************************************/
#include <list>
/********************************************************************
 *
 * Other includes
 *
 ********************************************************************/ 
#include "utils/misc/include/Types.hpp"
#include "../../HAL/include/IBufferProducer.hpp"
#include "utils/threads/include/CMutex.hpp"

class CChannelAllocator;
class IBufferConsumer;
class CProfile;

///CSourceAgent class
/**
 * The CSourceAgent implements IBufferReceiver interface and represents data source at L0 level. 
 * It provides filled buffers to CSource, i.e. source at HAL level, and by callback receives the free buffers back from CSource.
 * On channel closing CSource assosiated with Source Agent calls destroy() function.
 */

class CSourceAgent: public IBufferProducer
{
private:
   static Logger logger;

   friend class CSourceAgentTest;
public:
   // Methods section
   /**
    * Constructor
    * @param channelID - logical channel ID
    */
   explicit CSourceAgent(UInt32 channelID );
   /**
    * Destructor
    */   
   virtual ~CSourceAgent();

   //from IBufferProducer
   /**
    * Interface function inherited from IBufferProducer. It provides callback for CSource for getting the free buffers
    * @param buffer is the pointer to freed data buffer
    */
   virtual ERROR_CODE returnBufferBack(Buffer* buffer);
   /**
    * Interface function inherited from IBufferProducer. It registers IBufferConsumer data sender in HAL for providing filled buffers.
    * @param pConsumer is the pointer to the associated CSource object
    */
   virtual ERROR_CODE registerConsumer(IBufferConsumer* pConsumer);
   
   /**
    * Interface function inherited from IBufferProducer. It unregisters IBufferConsumer and then calls this->destroy().
    * @param pConsumer
    * @return
    */
   virtual ERROR_CODE unregisterConsumer(IBufferConsumer* pConsumer);

   /**
    * It is called on channel closing by associated CSource object for destruction
    */      
   void destroy();

   /**
    * @return channel number of current source agent 
    */
   const UInt32 getChannelID()
   {
      return mChannelID;
   }

   ERROR_CODE fillBuffer(Buffer& buffer);

   /**
    * Accessor to the queue property empty()
    * @return true if queue is empty
    */
   bool isQueueEmpty() const;

private:
   // Methods section 

   /**
    * Copying constructor
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is constructing on
    */   
   CSourceAgent(const CSourceAgent& );

   /**
    * Assigning operator
    *
    * Unimplemented to avoid missusing
    *
    * @param classRef The reference to the object the class is assiging from
    */   
   CSourceAgent& operator=(const CSourceAgent& );

   /**
    * Function provides the filled buffer from buffer queue to the Sender i.e CSource and removes the buffer from queue
    */   
   void sendData();

private:

   // Members section
   std::list<Buffer*>                              mBufferQueue;
   UInt32                                          mChannelID;
   IBufferConsumer*                                mpBufferConsumer;

   mutable CMutex                                  mBufferQueueMutex;
};
#endif
