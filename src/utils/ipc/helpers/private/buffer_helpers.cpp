/* 
 * 
 * iviLINK SDK, version 1.0
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
 * 
 */






#include "utils/misc/CUid.hpp"
#include "utils/misc/logging.hpp"

#include "buffer_helpers.hpp"

namespace iviLink {
namespace Ipc {
namespace Helpers {

char const * const CBufferWriter::moduleName = "CBufferWriter";
char const * const CBufferReader::moduleName = "CBufferReader";


template <>
CError CBufferWriter::write<iviLink::CUid>(iviLink::CUid const& val)
{
   UInt8 const* uid = NULL;
   UInt32 uidSize = val.getByteArray(uid);
   CError err = write(uidSize);
   if (!err.isNoError())
      return err;
   if (uidSize > mFullSize - mUsedSize)
      return CError(1, moduleName, CError::ERROR, "insufficient buffer size (from CUid write)");

   memcpy(mpBuffer + mUsedSize, uid, uidSize);
   mUsedSize += uidSize;
   return CError::NoError(moduleName);
}

template <>
CError CBufferReader::read<iviLink::CUid>(iviLink::CUid & val)
{
   UInt32 uidSize = 0;
   CError err = read(uidSize);
   if (!err.isNoError())
      return err;
   if (uidSize > mFullSize - mUsedSize)
      return CError(1, moduleName, CError::ERROR, "insufficient buffer size (from CUid write)");

   err = val.fromByteArray(mpBuffer + mUsedSize, uidSize);
   mUsedSize += uidSize;

   return err;
}

template <>
CError CBufferWriter::write<std::string>(std::string const& val)
{
   UInt32 valSize = val.size();
   CError err = write(valSize);
   if (!err.isNoError())
      return err;
   if (valSize > mFullSize - mUsedSize)
      return CError(1, moduleName, CError::ERROR, "insufficient buffer size (from std::string write)");

   memcpy(mpBuffer + mUsedSize, val.c_str(), valSize);
   mUsedSize += valSize;
   return CError::NoError(moduleName);
}

template <>
CError CBufferReader::read<std::string>(std::string & val)
{
   UInt32 valSize = 0;
   CError err = read(valSize);
   if (!err.isNoError())
      return err;
   if (valSize > mFullSize - mUsedSize)
      return CError(1, moduleName, CError::ERROR, "insufficient buffer size (from std::string write)");

   val.assign(reinterpret_cast<char const*>(mpBuffer + mUsedSize), valSize);
   mUsedSize += valSize;

   return err;
}

}  // namespace Helpers
}  // namespace Ipc
}  // namespace AXIS
