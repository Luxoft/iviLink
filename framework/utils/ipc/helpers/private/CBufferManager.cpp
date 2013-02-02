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


#include <cassert>

#include "CBufferManager.hpp"


namespace iviLink {
namespace Ipc {
namespace Helpers {


CBuffer::~CBuffer()
{
   if (mpBuf != NULL)
   {
      // last copy
      assert(mpMaster);
      mpMaster->utilizeBuffer(*this);
   }
}

CBuffer::CBuffer(CBuffer const& buf)
{
   if (this != &buf)
   {
      mpBuf = buf.mpBuf;
      buf.mpBuf = 0;

      mSize = buf.mSize;
      mpMaster = buf.mpMaster;
   }
}

CBuffer& CBuffer::operator=(CBuffer const& buf)
{
   if (this != &buf)
   {
      mpBuf = buf.mpBuf;
      buf.mpBuf = 0;

      mSize = buf.mSize;
      mpMaster = buf.mpMaster;
   }
   return *this;
}

bool CBuffer::isOk() const
{
   return mpBuf != 0;
}

UInt8* CBuffer::get() const
{
   return mpBuf;
}

UInt32 CBuffer::getSize() const
{
   return mSize;
}

CBuffer::CBuffer(UInt8* pBuf, UInt32 size, CBufferManager * pMaster) :
   mpBuf(pBuf),
   mSize(size),
   mpMaster(pMaster)
{
}

CBufferManager::CBufferManager()
{

}

CBufferManager::~CBufferManager()
{

}

CBuffer CBufferManager::getBuffer()
{
   mBufferMutex.lock();
   return CBuffer(mBuffer, BUFFER_SIZE, this);
}

void CBufferManager::utilizeBuffer(CBuffer buffer)
{
   assert(buffer.mpBuf == mBuffer);
   buffer.mpBuf = NULL;
   mBufferMutex.unlock();
}


}  // namespace Helpers
}  // namespace Ipc
}  // namespace AXIS
