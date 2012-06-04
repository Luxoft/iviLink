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





#include "utils/misc/include/byteOrder.hpp"


UInt16 ByteOrder::hton16(UInt16 data)
{
   // TODO: real endianness detection

#if 1 // Little-endian

   // Code from http://www.codeproject.com/KB/cpp/endianness.aspx
   return (((data >> 8)) |
            (data << 8));

#elif 2 // Big-endian

   return data;

#else // Middle-endian

   #error "Middle-endian system or endianness is not detected"

#endif // Endianness detection
}

UInt16 ByteOrder::ntoh16(UInt16 data)
{
   return hton16(data);
}

UInt32 ByteOrder::hton32(UInt32 data)
{
   // TODO: real endianness detection

#if 1 // Little-endian

   // Code from http://www.codeproject.com/KB/cpp/endianness.aspx
   return (((data & 0x000000FF) << 24) |
           ((data & 0x0000FF00) << 8) |
           ((data & 0x00FF0000) >> 8) |
           ((data & 0xFF000000) >> 24));

#elif 2 // Big-endian

   return data;

#else // Middle-endian

   #error "Middle-endian system or endianness is not detected"

#endif // Endianness detection

}

UInt32 ByteOrder::ntoh32(UInt32 data)
{
   return hton32(data);
}
