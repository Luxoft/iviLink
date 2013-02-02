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


/// @todo This is obsolete code, but a lot of other code depends on it. 
/// Those places must be rewritten using iviLink::CBuffer functionality.


#ifndef SERIALISE_HPP_
#define SERIALISE_HPP_

#include <string>

#include "Types.hpp"

namespace iviLink
{
   /**
    * Serializes std::string into buffer
    * @param str - object of std::string to serialize
    * @param pBuffer pointer to out buffer, memory is allocated before function call
    */
   void stringToBuffer(const std::string & str, UInt8* pBuffer, bool forNetwork = false);

   /**
    * @return size of serialized string
    */
   int stringInBufSize(const std::string & str);

   /**
    * De-serializes string from buffer
    * @param pBuffer is buffer that contains serialized string
    * @return std::string object with deserialized string
    */
   std::string bufferToString(const UInt8* const pBuffer, bool forNetwork = false);
}

#endif /* SERIALISE_HPP_ */
