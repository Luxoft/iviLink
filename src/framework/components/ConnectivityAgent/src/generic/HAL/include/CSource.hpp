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




#ifndef SOURCE_H_
#define SOURCE_H_

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
#include "../include/IBufferConsumer.hpp"
#include "../include/ITransmitter.hpp"
#include "utils/threads/include/CMutex.hpp"

/********************************************************************
 *
 * Forward declarations
 *
 ********************************************************************/
struct Buffer;
struct Frame;
class IBufferProducer;

/// CSource class
/**
 * This class is HAL-level channel sending part. It fills data frames from passed buffers.
 */
class CSource  : public IBufferConsumer
               , public ITransmitter
{
private:
   static Logger logger;

public:
   // Methods section

   /**
    * Constructor
    * @param channel_id - logical channel ID
    */
   explicit CSource(const UInt32 channel_id);

   /**
    * Destructor
    */
   virtual ~CSource();

   // From IBufferConsumer
   /**
    * This method is used to register delegate that will receive free buffers
    * @param channel_agent - delegate that will receive free buffers
    * @return
    */
   virtual ERROR_CODE registerProducer(IBufferProducer* pProducer);

   virtual ERROR_CODE unregisterProducer(IBufferProducer* pProducer);

   /**
    * Copies buffers to queue of buffers waiting to send
    * @param data_buffer - buffer to be send
    * @return
    */
   virtual ERROR_CODE consumeBuffer(Buffer* data_buffer);

   virtual ERROR_CODE flushBuffers();

   // From ITransmitter

   /**
    * Fills frame with data from buffers and gives free buffers to registered IBufferReceiver
    * @param frame_to_fill - data frame to be filled
    * @param quote - maxim size in frame to be filled
    * @return
    */
   virtual ERROR_CODE OnDispatch(Frame& frame_to_fill, const UInt32 quote);

   /**
    * Logical channel ID getter
    * @return channel ID
    */
   inline const UInt32 getChannelID() const
   {
      return mChannelID;
   }

   /**
    * Status of buffers queue
    * @return true if there are buffers to be sended
    */
   bool hasData() const;

private:
   // Methods section

   /**
    * Method copies buffers to frame one by one until quote and returns the list of empty buffers.
    * @param frame - frame to be filled
    * @param quote - maximum space in frame to be filled
    * @return list of buffers that was copied to frame completely
    */
   std::list<Buffer*> CopyToFrame(Frame& frame, const UInt32 quote);

private:
   // Members section

   /// Logical channel ID
   UInt32 mChannelID;

   /// Internal frame counter
   UInt32 mFrameCounter;

   /// Pointer to class that will receive buffers that freed after sending
   IBufferProducer* mpBufferProducer;

   /// Queue of buffers waiting to send
   std::list<Buffer*> mBuffersQueue;

   /// Mutex used to protect queue of buffers
   mutable CMutex mBufferQueueMutex;
};

#endif /* SOURCE_H_ */
