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


#ifndef CCONNECTIVITYAGENTERROR_HPP_
#define CCONNECTIVITYAGENTERROR_HPP_

#include "BaseError.hpp"

namespace iviLink {
namespace ConnectivityAgent {

/**
 * Simplifies error construction in Connectivity Agent
 * Class contains error codes for Connectivity Agent errors.
 */
class ConnectivityAgentError: public BaseError
{
public:
   enum AgentErrorCodes
   {
      // 0 is no error
      ERROR_OTHER = 1,
      ERROR_BROKEN_PIPE,
      ERROR_INCONSISTENT_DATA,
      ERROR_HANDSHAKE_FAILED,
      ERROR_CONNECT_FAILED,
      ERROR_RESEND_ACK,
      ERROR_CHANNEL_BUSY,
      ERROR_WRONG_FRAME_SEQUENCE,
      ERROR_DEFERRED,
      ERROR_ALLOCATION_IN_PROGRESS,
      ERROR_NOT_FOUND,
      ERROR_REQUEST_FAILED,
      ERROR_NO_FREE_BUFFERS_LEFT,
      ERROR_TIMEOUT,
   };

   explicit ConnectivityAgentError(AgentErrorCodes code, eSeverity severity = IVILINK_ERROR, char const* pModuleID = "Connectivity Agent", const char* pDescription = "") :
      BaseError((ErrorCode)code, pModuleID, severity, pDescription)
   {}


   explicit ConnectivityAgentError() :
      BaseError((ErrorCode)ERROR_OTHER, "ConnectivityAgent", IVILINK_ERROR, "")
   {}


   ConnectivityAgentError(ConnectivityAgentError const& ref) :
      BaseError(ref)
   {}

   void setErrorCode(AgentErrorCodes newCode)
   {
      changeErrorCode(static_cast<ErrorCode>(newCode));
   }

   void setNoError()
   {
      changeErrorCode(static_cast<ErrorCode>(IVILINK_NO_ERROR));
   }

   explicit ConnectivityAgentError(BaseError const& ref) :
      BaseError(ref)
   {}

   virtual ~ConnectivityAgentError()
   {}

   static ConnectivityAgentError NoError()
   {
      return ConnectivityAgentError(BaseError::NoError("Connectivity Agent", ""));
   }

};

}  // namespace ConnectivityAgent
}  // namespace iviLink


#endif /* CPMALERROR_HPP_ */
