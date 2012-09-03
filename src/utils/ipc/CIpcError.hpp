/* 
 * 
 * iviLINK SDK, version 1.1.2
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











#ifndef CIPCERROR_HPP_
#define CIPCERROR_HPP_

#include "utils/misc/CError.hpp"

namespace iviLink {
namespace Ipc {

/**
 * Simplifies error construction in IPC library.
 * Class contains error codes for errors of CIpc and CIpcSocket classes.
 */
class CIpcError: public CError
{
public:
   enum eCodes
   {
      ERROR_LISTEN_TIMEOUT = 1,
      ERROR_REQUEST_TIMEOUT,
      ERROR_RESPONSE_TIMEOUT,
      ERROR_COMMUNICATION,
      ERROR_INVALID_PARAMS,
      ERROR_SMALL_BUFFER,
      ERROR_CONNECTION_LOST,
      ERROR_UNKNOWN_DIRECTION,
      ERROR_OTHER,
      ERROR_WRONG_THREAD
   };

   explicit CIpcError(eCodes code, const char* pDescription = "", eSeverity severity = ERROR) :
      CError((ErrorCode)code, "CIpc", severity, pDescription)
   {}

   explicit CIpcError(eCodes code, std::string const& description, eSeverity severity = ERROR) :
      CError((ErrorCode)code, "CIpc", severity, description.c_str())
   {}

   CIpcError(CIpcError const& ref) :
      CError(ref)
   {}

   virtual ~CIpcError()
   {}

   /**
    * Constructs NO_ERROR message
    * @param pDescription message description
    * @return instance of CError class with NO_ERROR state
    */
   static CError NoIpcError(const char* pDescription = "")
   {
      return CError::NoError("CIpc", pDescription);
   }
};

}  // namespace Ipc
}  // namespace AXIS

#endif /* CIPCERROR_HPP_ */
