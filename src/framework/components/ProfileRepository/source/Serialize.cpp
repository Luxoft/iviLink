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

/***************************************************************************
 * Project           AXIS
 * (c) copyright     2012
 * Company           LUXOFT
 * @file             Serialize.hpp
 * @brief            Source file for serialization functionality
 * @author           VPlachkov
 ***************************************************************************/

#include <cstring>

#include "framework/components/ProfileRepository/include/Serialize.hpp"
#include "utils/misc/include/logging.hpp"
#include "utils/misc/include/byteOrder.hpp"

namespace AXIS
{
   void stringToBuffer(const std::string & str, UInt8* pBuffer, bool forNetwork)
   {
      const char * cstr = str.c_str();
      UInt32 size = str.size()+1;

      memcpy (pBuffer+sizeof(size),cstr,size);
      if (forNetwork)
      {
         size = ByteOrder::hton32(size);
      }
      memcpy (pBuffer, &size, sizeof(size));
   }

   int stringInBufSize(const std::string & str)
   {
      return str.length() + 1 + sizeof(UInt32);
   }

   std::string bufferToString(const UInt8* const pBuffer, bool forNetwork)
   {
      UInt32 size;
      memcpy(&size, pBuffer, sizeof(size));
      if (forNetwork)
      {
         size = ByteOrder::ntoh32(size);
      }
      const char * const cstr = reinterpret_cast<const char* const>(pBuffer+sizeof(size));
      return std::string(cstr, size - 1);
   }
}
