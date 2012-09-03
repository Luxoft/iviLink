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





#include "utils/misc/CError.hpp"

#ifndef CTRUSTLISTERROR_HPP__
#define CTRUSTLISTERROR_HPP__

namespace iviLink
{
class CTrustListError : public CError
{
public:
   enum eCodes
   {
      ERROR_OTHER          = 1,
      ERROR_UNKNOWN_UID    = 2,
      ERROR_STORAGE        = 3
   };

   explicit CTrustListError(eCodes code, char const* pModuleID = "", const char* pDescription = "", eSeverity severity = ERROR) :
      CError((ErrorCode)code, pModuleID, severity, pDescription)
   {}

   CTrustListError(eCodes code, char const* pModuleID, std::string const& description, eSeverity severity = ERROR) :
      CError((ErrorCode)code, pModuleID, severity, description.c_str())
   {}

   CTrustListError(CTrustListError const& ref) :
      CError(ref)
   {}

   explicit CTrustListError(CError const& ref) :
      CError(ref)
   {}

   virtual ~CTrustListError()
   {}

   static CTrustListError NoTLError(char const* pModuleID="", char const* pDescription = "")
   {
      return CTrustListError(CError::NoError(pModuleID, pDescription));
   }
};


}  // namespace iviLink

#endif //  CTRUSTLISTERROR_HPP__
