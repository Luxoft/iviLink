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


#ifndef FRAME_HPP_
#define FRAME_HPP_

#include "Types.hpp"
 
namespace iviLink
{
   namespace ConnectivityAgent
   {
      namespace HAL
      {
         struct Frame
         {
            struct FrameHeader
            {
               // 15 bit - ACK flag.
               UInt16 sizeAndFlags;
               UInt32 channel_id;
               UInt32 number;
            } __attribute__((packed)) mFrameHeader;

            UInt8 data[MAX_SIZE];

            void setSize(UInt16 size)
            {
               this->mFrameHeader.sizeAndFlags = (size & 0x7fff);
            }

            UInt16 getSize() const
            {
               return (this->mFrameHeader.sizeAndFlags & 0x7fff);
            }

            bool isACK() const
            {
               // 15 bit - ACK flag
               return (mFrameHeader.sizeAndFlags & 0x8000) != 0;
            }

            void setACK()
            {
               mFrameHeader.sizeAndFlags |= 0x8000;
            }
         } __attribute__((packed));
      }
   }
}
#endif /* FRAME_HPP_ */
