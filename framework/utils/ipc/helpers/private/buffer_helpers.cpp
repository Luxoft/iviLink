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


#include "BaseUid.hpp"

#include "buffer_helpers.hpp"

namespace iviLink {
namespace Ipc {
namespace Helpers {

char const * const CBufferWriter::moduleName = "CBufferWriter";
char const * const CBufferReader::moduleName = "CBufferReader";


template <>
BaseError CBufferWriter::write<iviLink::BaseUid>(iviLink::BaseUid const& val)
{
   UInt8 const* uid = NULL;
   UInt32 uidSize = val.getByteArray(uid);
   BaseError err = write(uidSize);
   if (!err.isNoError())
      return err;
   if (uidSize > mFullSize - mUsedSize)
      return BaseError(1, moduleName, BaseError::IVILINK_ERROR, "insufficient buffer size (from CUid write)");

   memcpy(mpBuffer + mUsedSize, uid, uidSize);
   mUsedSize += uidSize;
   return BaseError::NoError(moduleName);
}

template <>
BaseError CBufferReader::read<iviLink::BaseUid>(iviLink::BaseUid & val)
{
   UInt32 uidSize = 0;
   BaseError err = read(uidSize);
   if (!err.isNoError())
      return err;
   if (uidSize > mFullSize - mUsedSize)
      return BaseError(1, moduleName, BaseError::IVILINK_ERROR, "insufficient buffer size (from CUid write)");

   err = val.fromByteArray(mpBuffer + mUsedSize, uidSize);
   mUsedSize += uidSize;

   return err;
}

template <>
BaseError CBufferWriter::write<std::string>(std::string const& val)
{
   UInt32 valSize = val.size();
   BaseError err = write(valSize);
   if (!err.isNoError())
      return err;
   if (valSize > mFullSize - mUsedSize)
      return BaseError(1, moduleName, BaseError::IVILINK_ERROR, "insufficient buffer size (from std::string write)");

   memcpy(mpBuffer + mUsedSize, val.c_str(), valSize);
   mUsedSize += valSize;
   return BaseError::NoError(moduleName);
}

template <>
BaseError CBufferReader::read<std::string>(std::string & val)
{
   UInt32 valSize = 0;
   BaseError err = read(valSize);
   if (!err.isNoError())
      return err;
   if (valSize > mFullSize - mUsedSize)
      return BaseError(1, moduleName, BaseError::IVILINK_ERROR, "insufficient buffer size (from std::string write)");

   val.assign(reinterpret_cast<char const*>(mpBuffer + mUsedSize), valSize);
   mUsedSize += valSize;

   return err;
}

}  // namespace Helpers
}  // namespace Ipc
}  // namespace AXIS
