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


// Here we are using non-standard set of functions for byte endianess conversion
#include <endian.h>
#include "byteOrder.hpp"


UInt16 ByteOrder::hton16(UInt16 data)
{
  return htobe16(data);
}

UInt16 ByteOrder::ntoh16(UInt16 data)
{
#ifndef ANDROID
   return be16toh(data);
#else
   return betoh16(data);
#endif //ANDROID
}

UInt32 ByteOrder::hton32(UInt32 data)
{
   return htobe32(data);
}

UInt32 ByteOrder::ntoh32(UInt32 data)
{
#ifndef ANDROID
   return be32toh(data);
#else
   return betoh32(data);
#endif //ANDROID
}

UInt64 ByteOrder::hton64(UInt64 data)
{
   return htobe64(data);
}

UInt64 ByteOrder::ntoh64(UInt64 data)
{
#ifndef ANDROID
   return be64toh(data);
#else
   return betoh64(data);
#endif //ANDROID
}
