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


#ifndef CBUFFERMANAGER_HPP_
#define CBUFFERMANAGER_HPP_

#include "CMutex.hpp"
#include "Logger.hpp"
#include "Types.hpp"

namespace iviLink {
namespace Ipc {
namespace Helpers {

class CBufferManager;


/**
 * Class for representing buffer from buffer pool.
 * Object of class will hold pointer to buffer.
 * Only one object will hold pointer, so pointer will be passed to copy of object.
 * Last of the objects will return itself to master buffer pool.
 */
class CBuffer
{
public:

   ~CBuffer();

   CBuffer(CBuffer const& buf);

   CBuffer& operator=(CBuffer const& buf);

   /**
    * Checks if this object really have the pointer to data buffer
    * @retval true if object have pointer to buffer
    * @retval false otherwise
    */
   bool isOk() const;

   /**
    * @return pointer to data buffer
    */
   UInt8* get() const;

   /**
    * @return size of data buffer
    */
   UInt32 getSize() const;

private:

   CBuffer(UInt8* pBuf, UInt32 size, CBufferManager * pMaster);

   mutable UInt8* mpBuf;
   UInt32 mSize;
   CBufferManager * mpMaster;

   friend class CBufferManager;
};


/**
 * Manger class for organizing the pool of buffers for IpcProtocol classes
 *
 * @todo current implementation is the most simple. But it is possible to make
 * better implementation.
 *
 */
class CBufferManager
{
private:
   static Logger logger;

public:

   CBufferManager();

   ~CBufferManager();

   /**
    * Reserves buffer
    *
    * @todo pass size of requested buffer
    *
    * @return object representing the buffer
    */
   CBuffer getBuffer();

   /**
    * Returns buffer, obtained from getBuffer(), to pool
    *
    * @param buffer
    */
   void utilizeBuffer(CBuffer buffer);

private:

   enum {
      BUFFER_SIZE = 4096
   };
   UInt8 mBuffer[BUFFER_SIZE];
   CMutex mBufferMutex;
};


}  // namespace Helpers
}  // namespace Ipc
}  // namespace AXIS


#endif /* CBUFFERMANAGER_HPP_ */
