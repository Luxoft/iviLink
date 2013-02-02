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


#ifndef BYTE_ORDER_HPP_
#define BYTE_ORDER_HPP_

#include "Types.hpp"


namespace ByteOrder
{
   // Converts byte order of frame's fields before sending and after receiving
   // Data will be always transmitted using big-endian byte order
   UInt16 hton16(UInt16 data);
   UInt32 hton32(UInt32 data);
   UInt64 hton64(UInt64 data);

   // Conversion from network (big-endian) to host byte order
   UInt16 ntoh16(UInt16 data);
   UInt32 ntoh32(UInt32 data);
   UInt64 ntoh64(UInt64 data);
}


#endif /* BYTE_ORDER_HPP_ */
