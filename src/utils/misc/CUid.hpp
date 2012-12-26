/* 
 * iviLINK SDK, version 1.1.19
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 

#ifndef CUID_HPP_
#define CUID_HPP_

#include <string>

#include "CError.hpp"
#include "Types.hpp"

namespace iviLink
{

class CUid
{
public:

   CUid()
   {
   }


   CUid(std::string const& uid)
      : mValue(uid)
   {
   }

   CUid(const CUid & uid)
      : mValue(uid.mValue)
   {
   }

   bool operator<(CUid const& uid) const
   {
      return mValue < uid.mValue;
   }

   bool operator==(CUid const& uid) const
   {
      return mValue == uid.mValue;
   }

   bool operator!=(CUid const& uid) const
   {
      return mValue != uid.mValue;
   }

   bool empty() const
   {
      return mValue.empty();
   }
   CUid& operator= (const char* val)
   {
      mValue = std::string(val);
      return *this;
   }
   std::string value() const
   {
      return mValue;
   }

   UInt32 getByteArray(UInt8 const* & pArr) const
   {
      /// @todo better serialization
      pArr = reinterpret_cast<UInt8 const*>(mValue.c_str());
      return mValue.size() + 1;
   }

   CError fromByteArray(UInt8 const* pArr, UInt32 size)
   {
      /// @todo better deserialization
      char const* val = reinterpret_cast<char const*>(pArr);
      if (val[size - 1] != '\0')
         return CError(1, "CUid", CError::ERROR, "wrong data");
      mValue = std::string(val);
      return CError::NoError("CUid");
   }

private:

   std::string mValue;
};

/// Generates new UID
CUid generateUid();

}

#endif /* CUID_HPP_ */
