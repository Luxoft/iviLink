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


#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <cassert>
#include <cstring>
#include "Types.hpp"
#include "Logger.hpp"

namespace iviLink
{
   namespace ConnectivityAgent
   {

         class Buffer
         {
         public:
            Buffer() :
               allocated_size(0),
               filled_size(0),
               data(0)
            {

            }

            ~Buffer()
            {
               if (data)
               {
                  delete[] data;
               }
            }

            UInt32& getFilledSize()
            {
               return filled_size;
            }

            UInt32 getFilledSize() const
            {
               return filled_size;
            }

            void setFilledSize(UInt32 size)
            {
               filled_size = size;
            }

            UInt32 getAllocatedSize() const
            {
               return allocated_size;
            }
            UInt8* const getData() const
            {
               return data;
            }

            void reserveSize(UInt32 size)
            {
               assert(size <= MAX_SIZE);
               if (!data || allocated_size < size)
               {
                  delete[] data;
                  data = new UInt8[size];
                  allocated_size = size;
               }
            }

            UInt32 appendData(UInt8 *pData,UInt32 size)
            {
               if (size > (allocated_size-filled_size))
               {
                  return 0;
               }
               memcpy(data+filled_size,pData, size);

               filled_size +=size;

               return 1;
            }

            void forgetData()
            {
               data = 0;
               allocated_size = 0;
            }

            UInt32 getMaxSize() const
            {
               return MAX_SIZE;
            }

         private:

            UInt32 allocated_size;
            UInt32 filled_size;
            UInt8* data;
         };
         
   }
}

#endif /* BUFFER_HPP_ */
